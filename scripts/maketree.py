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
        input_line = raw_line.split(self.comment)[0].strip()

        # Skip empty lines or comments (!)
        if not input_line:
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


prefixes = []   # Unfortunately, I wrote a lot of code with this global name already, so let's use it
class Prefixes:
    current = None
    def __init__(self, addition, components):
        global prefixes
        self.parent = copy.deepcopy(self.current)
        inputs = [comp.strip() for comp in components.split(',')]
        self.prefs = ['%s%s' % (pref, comp) for pref in prefixes for comp in inputs] or inputs if not addition else prefixes + inputs
        prefixes = self.prefs
        Prefixes.current = self

    def get_parent(self):
        global prefixes
        Prefixes.current = self.parent
        prefixes = self.parent.prefs if self.parent else []
        

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
        self.inputs = [(label, inp.replace('[]', prefix)) for label, inp in inputs]
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

    def add_var(self, variable, value):
        self.variables.append((('_%s' % variable).rstrip('_'), value))

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
        incr = ['++', '--']
        return """void %s::%s {
  switch(base) {
%s
    break;
  default:
    throw;
  }
}
""" % (classname, self.signature,
       '\n    break;\n'.join(
           ['\n'.join(['  case %s::%s::%s:' % (classname, self.enum_name, pref)] +
                      ['    %s%s%s;' % (val, pref, var)
                       for var, val in self.variables if val in incr] +
                      ['    %s%s = %s;' % (pref, var, val.replace('[]', pref))
                       for var, val in self.variables if val not in incr])
            for pref in self.prefixes]))


if __name__ == '__main__':
    classname = os.path.basename(sys.argv[1]).split('.')[0]

    includes = ['<string>', '<vector>', '"TObject.h"', '"TFile.h"', '"TTree.h"']
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
                match = re.match(r'(\+)?([\w,\s]*){', line)
                if match:
                    # Get the different classes that are being added
                    Prefixes(bool(match.group(1)), match.group(2))

                    continue

                # End previous enum scope
                match = re.match(r'}', line)
                if match:
                    if in_function:
                        functions.append(in_function)
                    in_function = None
                    Prefixes.current.get_parent()

                    continue

                # Get functions for setting values
                match = re.match('^(\w+\(.+\))$', line)
                if match:
                    # Pass off previous function as quickly as possible to prevent prefix changing
                    if in_function:
                        functions.append(copy.deepcopy(in_function))
                        in_function = None

                    in_function = Function(match.group(1), prefixes)
                    continue

                # Get TMVA information to evaluate
                match = re.match(r'^(\#\s*)?\[([^;]*);\s*([^;]*)(;\s*(.*))?\](\s?=\s?(.*))?$', line)
                if match:
                    if '"TMVA/Reader.h"' not in includes:
                        includes.append('"TMVA/Reader.h"')
                        includes.append('"TMVA/IMethod.h"')

                    is_saved = not bool(match.group(1))
                    var = match.group(2)
                    weights = match.group(3)
                    trained_with = match.group(5)
                    default = match.group(7) or DEFAULT_VAL
                    branches = create_branches(var, 'F', default, is_saved)
                    if os.path.exists(weights) and is_saved:
                        xml_vars = ElementTree.parse(weights, ElementTree.XMLParser(target=MyXMLParser('Variable', 'Expression'))).getroot()
                        inputs = [(v, v.replace(trained_with or Branch.branches[v].prefix, '[]')) for v in xml_vars]
                        for reader in [Reader(weights, b.prefix, var, inputs) for b in branches]:
                            mod_fill.insert(0, '%s = %s->GetMvaValue();' % (reader.output, reader.method))

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
                        in_function.add_var(var, match.group(8))

                    if match.group(9):
                        # create_branches returns a list of prefixes and the new branch names
                        branches = create_branches(var, data_type, val, is_saved)
                        for b in branches:
                            mod_fill.append('%s = %s;' % (b.name, match.group(10).replace('[]', b.prefix)))
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
%s%s
};
""" % (RESET_SIGNATURE, FILL_SIGNATURE, '\n  '.join([f.declare(functions) for f in functions]),
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
