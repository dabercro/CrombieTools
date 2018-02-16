#! /usr/bin/env python

import os
import sys
import re
import copy
import subprocess

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
FILTER = {'--': [], '++': []}

PREF_HOLDER = '[]'

class MyXMLParser:
    def __init__(self, tag, spec_tag, member):
        self.tag = tag
        self.spec_tag = spec_tag
        self.member = member
        self.output = []
        self.spectators = []
    def start(self, tag, attr):
        if tag == self.tag:
            self.output.append(attr[self.member])
        elif tag == self.spec_tag:
            self.spectators.append(attr[self.member])
    def end(self, _):
        pass
    def data(self, _):
        pass
    def close(self):
        return self.output, self.spectators


class Parser:
    def __init__(self, comment='!'):
        self.defs = {}
        self.comment = comment

    def parse(self, raw_line):
        input_line = raw_line.split(self.comment)[0].strip()

        # Skip empty lines or comments (!)
        if not input_line:
            return []

        start = ''
        while input_line != start:
            start = input_line
            input_line = input_line.replace('->', '^^^')
            for matches in list(re.finditer(r'<([^<>{}]*){(.*?)}([^<>]*)>', input_line)) + list(re.finditer(r'<([^<>{}]*)\|([^\|]*?)\|([^<>\|]*)>', input_line)):
                beg, end = ('<', '>') if '|' in matches.group(1) or '{' in matches.group(3) and '}' in matches.group(3) else ('', '')
                input_line = input_line.replace(
                    matches.group(0),
                    beg + matches.group(1) +
                    ('%s%s, %s%s' % (matches.group(3), end, beg, matches.group(1))).join([suff.strip() for suff in matches.group(2).split(',')]) +
                    matches.group(3) + end
                )
            input_line = input_line.replace('^^^', '->')


        match = re.match(r'(.*)\|([^\s])?\s+(.*)', input_line)  # Search for substitution
        if match:
            char = match.group(2) or '$'
            lines = [match.group(1).replace(char * 3, var.strip().upper()).replace(char * 2, var.strip().title()).replace(char, var.strip())
                     for var in match.group(3).split(',')]
            return [line for l in lines for line in self.parse(l)]  # Recursively parse lines in case multiple expansions are present

        match = re.match(r'.*(\`(.*)\`).*', input_line)   # Search for shell commands
        if match:
            input_line = input_line.replace(match.group(1), subprocess.check_output(match.group(2).split()).strip())
            
        return [input_line]
        


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
    floats = set()
    def __init__(self, pref, name, data_type, default_val, is_saved):
        self.prefix = pref
        self.name = name
        self.data_type = data_type
        self.default_val = default_val
        self.is_saved = is_saved
        self.branches[self.name] = self


class Reader:
    readers = []
    def __init__(self, weights, prefix, output, inputs, specs, subs):
        self.weights = weights
        self.output = ('%s_%s' % (prefix, output)).strip('_')
        sub_pref = lambda x: [(label, subs.get(inp, inp).replace(PREF_HOLDER, prefix) if prefix else label)
                              for label, inp in x]
        self.inputs = sub_pref(inputs)
        self.specs = sub_pref(specs)
        self.name = 'reader_%s' % self.output
        self.method = 'method_%s' % self.output
        self.floats = []
        self.readers.append(self)

    def float_inputs(self, mod_fill):
        for index, inp in enumerate(self.inputs):
            address = inp[1]
            if Branch.branches[address].data_type != 'F':
                newaddress = '_tmva_float_%s' % address
                self.inputs[index] = (inp[0], newaddress)
                if address not in Branch.floats:
                    Branch.floats.add(address)
                    mod_fill.insert(0, '%s = %s;' % (newaddress, address))
                    self.floats.append(newaddress)


class Function:
    def __init__(self, signature, prefixes):
        self.enum_name = re.match(r'(.*)\(.*\)', signature).group(1)
        self.prefixes = prefixes
        if prefixes:
            signature = signature.replace('(', '(const %s base, ' % self.enum_name)
        else:
            signature = signature.replace('(', '(const ')
        self.signature = 'set_' + signature.replace(', ', ', const ').replace(', const )', ')')
        self.variables = []

    def add_var(self, variable, value):
        new_var = (variable, value) if value == '~~' else (('_%s' % variable).rstrip('_'), value)
        self.variables.append(new_var)

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
  enum class %s {
    %s
  };
  """ % (self.enum_name, ',\n    '.join(self.prefixes)))

    def var_line(self, var, val, pref):
        incr = ['++', '--']
        if val in incr:
            return '    %s%s%s;' % (val, pref, var)
        elif val == '~~':
            return '    if (!(%s))\n      return;' % (var)
        return'    %s%s = %s;' % (pref, var, val.replace(PREF_HOLDER, pref))

    def implement(self, classname):
        if self.prefixes:
            middle = """  switch(base) {
%s
    break;
  default:
    throw;
  }""" % ('\n    break;\n'.join(
                    ['\n'.join(['  case %s::%s::%s:' % (classname, self.enum_name, pref)] +
                               [self.var_line(var, val, pref) for var, val in self.variables])
                     for pref in self.prefixes]))
        else:
            middle = '\n'.join([self.var_line(var.lstrip('_'), val, '') for var, val in self.variables])

        return """void %s::%s {
%s
}
""" % (classname, self.signature, middle)

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
                    subs = {}
                    if trained_with and os.path.exists(trained_with):  # In this case, trained_with actually points to a file that lists substitutions
                        with open(trained_with, 'r') as sub_file:
                            for line in sub_file:
                                info = line.split()
                                subs[info[0]] = info[1]

                    default = match.group(7) or DEFAULT_VAL
                    branches = create_branches(var, 'F', default, is_saved)
                    if os.path.exists(weights) and is_saved:
                        xml_vars, xml_specs = ElementTree.parse(weights, ElementTree.XMLParser(target=MyXMLParser('Variable', 'Spectator', 'Expression'))).getroot()
                        rep_pref = lambda x: [(v, v.replace(trained_with or (Branch.branches[v].prefix if Branch.branches[v].prefix in prefixes
                                                                             else PREF_HOLDER), PREF_HOLDER))
                                              for v in x]
                        inputs = rep_pref(xml_vars)
                        specs = rep_pref(xml_specs)
                        for reader in [Reader(weights, b.prefix, var, inputs, specs, subs) for b in branches]:
                            mod_fill.append('%s = %s->GetMvaValue();' % (reader.output, reader.method))

                    continue

                # Event filter
                match = re.match(r'(\+\+|--)(.*)(\+\+|--)', line)
                if match:
                    FILTER[match.group(1)] = ['if (!(%s))' % match.group(2).strip(),       # Filter at front (--) or back (++) of fill
                                              '  return;']
                    continue

                # A cut inside a function
                match = re.match(r'~~\s*(.*?)\s*~~', line)
                if match:
                    # Only valid if in a function
                    in_function.add_var(match.group(1), '~~')
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
                            mod_fill.append('%s = %s;' % (b.name, match.group(10).replace(PREF_HOLDER, b.prefix)))
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
%s%s%s}""" % (classname, classname, '\n  '.join(['t->Branch("{0}", &{0}, "{0}/{1}");'.format(b.name, b.data_type) for b in all_branches if b.is_saved]),
              ''.join(['  %s.AddVariable("%s", &%s);\n' % (reader.name, label, var) for reader in Reader.readers for label, var in reader.inputs]),
              ''.join(['  %s.AddSpectator("%s", &%s);\n' % (reader.name, label, var) for reader in Reader.readers for label, var in reader.specs]),
              ''.join(['  %s = %s.BookMVA("%s", "%s");\n' % (reader.method, reader.name, reader.output, reader.weights) for reader in Reader.readers])))

        # reset function
        write("""
void %s::%s {
  %s
}""" % (classname, RESET_SIGNATURE, '\n  '.join(['{0} = {1};'.format(b.name, b.default_val) for b in all_branches])))

        # fill function
        mod_fill = FILTER['--'] + mod_fill + FILTER['++']
        write("""
void %s::%s {
  %s
}
""" % (classname, FILL_SIGNATURE, '\n  '.join(mod_fill + ['t->Fill();'])))

        for func in functions:
            write(func.implement(classname))

        # Template enum conversion
        def write_convert(first, second):
            write(("""{0}::{1} to_{1}({0}::{2} e_cls) {begin}
  switch (e_cls) {begin}
  case %s
  default:
    throw;
  {end}
{end}
""" % '\n  case '.join(['{0}::{2}::%s:\n    return {0}::{1}::%s;' %
                       (pref, pref) for pref in second.prefixes if pref in first.prefixes])).format(
                           classname, first.enum_name, second.enum_name, first.prefixes[0], begin='{', end='}'))
            

        prev_func = None
        for func in functions:
            if prev_func and func.prefixes != prev_func.prefixes and prev_func.prefixes:
                if False not in [prev in func.prefixes for prev in prev_func.prefixes]:
                    write_convert(prev_func, func)
                    write_convert(func, prev_func)

            prev_func = func

        write('#endif')

    print 'Created %s class with %i branches' % (classname, len(all_branches))
