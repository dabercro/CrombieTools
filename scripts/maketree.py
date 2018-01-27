#! /usr/bin/env python

import os
import sys
import re
import copy

from xml.etree import ElementTree

TYPES = {
    'B': 'Char_t',
    'b': 'UChar_t',
    'S': 'Short_t',
    's': 'UShort_t',
    'I': 'Int_t',
    'i': 'UInt_t',
    'F': 'Float_t',
    'D': 'Double_t',
    'L': 'Long64_t',
    'l': 'ULong64_t',
    'O': 'Bool_t'
}

DEFAULT_TYPE = 'F'
DEFAULT_VAL = '0'
RESET_SIGNATURE = 'reset()'
FILL_SIGNATURE = 'fill()'


class MyXMLParser:
    def __init__(self, tag, member):
        self.tag = tag
        self.member = member
        self.output = []
    def start(self, tag, attr):
        if tag == self.tag:
            self.output.append(attr[self.member])
    def end(self, _):
        pass
    def data(self, _):
        pass
    def close(self):
        return self.output


class Parser:
    def __init__(self, comment='!'):
        self.defs = {}
        self.comment = comment

    def parse(self, raw_line):
        input_line = raw_line.strip()

        # Skip empty lines or comments (!)
        if not input_line or input_line[0] == self.comment:
            return []

        for matches in re.finditer(r'<([^\|<>]*)\|([^\|<>]*)\|([^<>]*)>', input_line):
            input_line = input_line.replace(matches.group(0),
                                            matches.group(1) +
                                            ('%s, %s' % (matches.group(3), matches.group(1))).join([suff.strip() for suff in matches.group(2).split(',')]) +
                                            matches.group(3)
                                            )

        match = re.match(r'(.*\$.*)\|\s*(.*)', input_line)
        lines = [match.group(1).replace('$$', var.strip().title()).replace('$', var.strip()) for var in match.group(2).split(',')] if match else [input_line]
        return lines


class Branch:
    branches = {}
    def __init__(self, pref, name, data_type, default_val, is_saved):
        self.prefix = pref
        self.name = name
        self.data_type = data_type
        self.default_val = default_val
        self.is_saved = is_saved
        self.branches[self.name] = self


class Reader:
    readers = []
    def __init__(self, weights, prefix, output, inputs):
        self.weights = weights
        self.output = '%s_%s' % (prefix, output)
        self.inputs = [(label, inp.replace('<>', prefix)) for label, inp in inputs]
        self.name = 'reader_%s' % self.output
        self.method = 'method_%s' % self.output
        self.floats = []
        self.readers.append(self)

    def float_inputs(self, mod_fill):
        for index, inp in enumerate(self.inputs):
            address = inp[1]
            if Branch.branches[address].data_type != 'F':
                newaddress = '_tmva_float_%s' % address
                mod_fill.insert(0, '%s = %s;' % (newaddress, address))
                self.floats.append(newaddress)
                self.inputs[index] = (inp[0], newaddress)


class Function:
    def __init__(self, signature, prefixes):
        self.enum_name = re.match(r'(.*)\(.*\)', signature).group(1)
        self.signature = 'set_' + signature.replace('(', '(const %s base, ' % self.enum_name).replace(', ', ', const ').replace(', const )', ')')
        self.prefixes = prefixes
        self.variables = []

    def add_var(self, variable, value, data_type):
        self.variables.append((('_%s' % variable).rstrip('_'), value, TYPES[data_type]))

    def declare(self, functions):
        output = '{0}void %s;' % (self.signature)

        for func in functions:
            # Just use the first occurance of an identical enum class
            if func.enum_name == self.enum_name:
                break
            if func.prefixes == self.prefixes:
                output = output.replace('const %s' % self.enum_name, 'const %s' % func.enum_name)
                self.signature = self.signature.replace('const %s' % self.enum_name, 'const %s' % func.enum_name)
                self.enum_name = func.enum_name

                return output.format('')

        return output.format("""
  enum class %s : unsigned {
    %s
  };
 private:
  const std::vector<std::string> %s_names = {
    %s
  };
 public:
  """ % (self.enum_name, ',\n    '.join(['%s = %s' % (pref, index) for index, pref in enumerate(self.prefixes)]),
         self.enum_name, ',\n    '.join(['"%s"' % pref for pref in self.prefixes])))


    def implement(self, classname):
        for index in range(len(self.variables)):
            var, value, t = self.variables[index]
            for replace in re.findall(r'<>[_\w]*', value):
                suff = replace.lstrip('<>')
                value = value.replace(replace, '(*(%s*)(addresses.at(base_name + "%s")))' % (TYPES[Branch.branches[self.prefixes[0] + suff].data_type], suff))

            self.variables[index] = (var, value, t)

        incr = ['++', '--']
        return """void %s::%s {
  auto& base_name = %s_names[static_cast<unsigned>(base)];
  %s
}
""" % (classname, self.signature, self.enum_name,
       '\n  '.join(['{2}*({1}*)(t->GetBranch((base_name + "{0}").data())->GetAddress());'. format(var, t, val)
                    for var, val, t in self.variables if val in incr] +
                   ['set(base_name + "{0}", static_cast<{1}>({2}));'.format(var, t, val)
                    for var, val, t in self.variables if val not in incr]
       ))


if __name__ == '__main__':
    classname = os.path.basename(sys.argv[1]).split('.')[0]

    includes = ['<string>', '<vector>', '<unordered_map>', '"TObject.h"', '"TFile.h"', '"TTree.h"']
    prefixes = []
    functions = []
    mod_fill = []
    in_function = None

    def create_branches(var, data_type, val, is_saved):
        branches = [Branch(pref, ('%s_%s' % (pref, var)).rstrip('_'), data_type, val, is_saved) for pref in prefixes] or [Branch('', var, data_type, val, is_saved)]
        return branches

    parser = Parser()

    with open(sys.argv[1], 'r') as config_file:
        for raw_line in config_file:
            for line in parser.parse(raw_line):
                line = line.strip()

                if line == '<--':
                    if in_function:
                        functions.append(in_function)
                    in_function = None
                    prefixes = []
                    continue

                # Check for includes
                match = re.match(r'^([<"].*[">])$', line)
                if match:
                    includes.append(match.group(1))
                    continue

                # Check for default values or reset function signature
                match = re.match(r'{(/(\w))?(-?\d+)?(reset\(.*\))?(fill\(.*\))?}', line)
                if match:
                    if match.group(2):
                        DEFAULT_TYPE = match.group(2)
                    elif match.group(3):
                        DEFAULT_VAL = match.group(3)
                    elif match.group(4):
                        RESET_SIGNATURE = match.group(4).replace('(', '(const ').replace(', ', ', const ')
                    elif match.group(5):
                        FILL_SIGNATURE = match.group(5).replace('(', '(const ').replace(', ', ', const ')
                    continue

                # Get "class" enums and/or functions for setting
                match = re.match(r'(\+?[\w,]*)\s?-->\s?([\w_]*\(.*\))?', line)
                if match:
                    # Pass off previous function as quickly as possible to prevent prefix changing
                    if in_function:
                        functions.append(copy.deepcopy(in_function))
                        in_function = None

                    # Get the different classes that are being added
                    components = match.group(1).split(',') if match.group(1) else []
                    if components:
                        if match.group(1).startswith('+'):
                            components[0] = components[0].lstrip('+')
                            prefixes += components
                        else:
                            prefixes = ['%s%s' % (pref, comp) for pref in prefixes for comp in components] or components

                    function_sig = match.group(2)
                    if function_sig:
                        in_function = Function(function_sig, prefixes)
                    continue

                # Get TMVA information to evaluate
                match = re.match(r'^\[(.*);\s*(.*);\s*(.*)\](\s?=\s?(.*))?$', line)
                if match:
                    if '"TMVA/Reader.h"' not in includes:
                        includes.append('"TMVA/Reader.h"')
                        includes.append('"TMVA/IMethod.h"')

                    var = match.group(1)
                    weights = match.group(2)
                    trained_with = match.group(3)
                    default = match.group(5) or DEFAULT_VAL
                    branches = create_branches(var, 'F', default, True)
                    if os.path.exists(weights):
                        xml_vars = ElementTree.parse(weights, ElementTree.XMLParser(target=MyXMLParser('Variable', 'Label'))).getroot()
                        inputs = [(v, v.replace(trained_with, '<>')) for v in xml_vars]
                        for reader in [Reader(weights, b.prefix, var, inputs) for b in branches]:
                            mod_fill.append('%s = %s->GetMvaValue();' % (reader.output, reader.method))

                    continue

                # Add branch names individually
                match = re.match(r'(\#\s*)?(\w*)(/(\w))?(\s?=\s?([\w\.\(\)]+))?(\s?->\s?(.*?))?(\s?<-\s?(.*?))?$', line)
                if match:
                    var = match.group(2)
                    data_type = match.group(4) or DEFAULT_TYPE
                    val = match.group(6) or DEFAULT_VAL
                    is_saved = not bool(match.group(1))
                    create_branches(var, data_type, val, is_saved)

                    if in_function is not None and match.group(7):
                        in_function.add_var(var, match.group(8), data_type)

                    if match.group(9):
                        # create_branches returns a list of prefixes and the new branch names
                        branches = create_branches(var, data_type, val, is_saved)
                        for b in branches:
                            mod_fill.append('%s = %s;' % (b, match.group(10).replace('<>', b.prefix)))
                    continue

    ## Finished parsing the configuration file

    # Let's put the branches in some sort of order
    all_branches = [Branch.branches[key] for key in sorted(Branch.branches)]

    # Check that all of the TMVA inputs are floats
    for reader in Reader.readers:
        reader.float_inputs(mod_fill)

    with open('%s.h' % classname, 'w') as output:
        write = lambda x: output.write('%s\n' % x)

        # Write the beginning of the file
        write("""#ifndef CROMBIE_{0}_H
#define CROMBIE_{0}_H
""".format(classname.upper()))
        for inc in includes:
            write('#include %s' % inc)

        # Start the class definition
        write('\nclass %s {' % classname)
        write("""
 public:
  {0}(const char* outfile_name, const char* name = "events");
  ~{0}() {1}
""".format(classname, '{ write(t); f->Close(); }'))

        # Write the elements for the memory of each branch
        for branch in all_branches:
            write('  %s %s;' % (TYPES[branch.data_type], branch.name))

        # Some functions, and define the private members
        write("""
  void %s;
  void %s;
  void write(TObject* obj) { f->WriteTObject(obj, obj->GetName()); }
  %s

 private:
  TFile* f;
  TTree* t;  

  template <typename T>
  void set(std::string name, T val) { *((T*)addresses.at(name)) = val; }

  const std::unordered_map<std::string, void*> addresses {
    %s
  };
%s%s
};
""" % (RESET_SIGNATURE, FILL_SIGNATURE, '\n  '.join([f.declare(functions) for f in functions]),
       ',\n    '.join(['{"%s", &%s}' % (key, key) for key in sorted(Branch.branches)]),
       ''.join(['\n  Float_t %s;' % address for reader in Reader.readers for address in reader.floats]),
       ''.join(['\n  TMVA::Reader %s {"Silent"};\n  TMVA::IMethod* %s {};' % (reader.name, reader.method) for reader in Reader.readers])))

        # Initialize the class
        write("""%s::%s(const char* outfile_name, const char* name)
: f {new TFile(outfile_name, "CREATE")},
  t {new TTree(name, name)}
{
  %s
%s%s}""" % (classname, classname, '\n  '.join(['t->Branch("{0}", &{0}, "{0}/{1}");'.format(b.name, b.data_type) for b in all_branches if b.is_saved]),
            ''.join(['  %s.AddVariable("%s", &%s);\n' % (reader.name, label, var) for reader in Reader.readers for label, var in reader.inputs]),
            ''.join(['  %s = %s.BookMVA("%s", "%s");\n' % (reader.method, reader.name, reader.output, reader.weights) for reader in Reader.readers])))

        # reset function
        write("""
void %s::%s {
  %s
}""" % (classname, RESET_SIGNATURE, '\n  '.join(['{0} = {1};'.format(b.name, b.default_val) for b in all_branches])))

        # fill function
        write("""
void %s::%s {
  %s
}
""" % (classname, FILL_SIGNATURE, '\n  '.join(mod_fill + ['t->Fill();'])))

        for func in functions:
            write(func.implement(classname))

        # Template enum conversion
        def write_convert(first, second):
            write(("""
{0}::{1} to_{1}({0}::{2} e_cls) {begin}
  switch (e_cls) {begin}
  case %s
  default:
    throw;
  {end}
{end}""" % '\n  case '.join(['{0}::{2}::%s:\n    return {0}::{1}::%s;' %
                       (pref, pref) for pref in second.prefixes if pref in first.prefixes])).format(
                           classname, first.enum_name, second.enum_name, first.prefixes[0], begin='{', end='}'))
            

        prev_func = None
        for func in functions:
            if prev_func and func.prefixes != prev_func.prefixes:
                if False not in [prev in func.prefixes for prev in prev_func.prefixes]:
                    write_convert(prev_func, func)
                    write_convert(func, prev_func)

            prev_func = func

        write('\n#endif')
