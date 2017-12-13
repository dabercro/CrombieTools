#! /usr/bin/env python

import os
import sys
import re
import copy


TYPES = {
    'I': 'int',
    'l': 'long',
    'F': 'float',
    'O': 'bool'
}

DEFAULT_TYPE = 'F'
DEFAULT_VAL = '0'
RESET_SIGNATURE = 'reset()'


class Branch:
    def __init__(self, name, data_type, default_val):
        self.name = name
        self.data_type = data_type
        self.default_val = default_val


class Function:
    def __init__(self, signature):
        self.signature = signature.replace('(', '(std::string base, ')
        self.vars = []

    def add_var(self, variable, value):
        self.vars.append((variable, value))

    def declare(self):
        return 'void %s;' % self.signature

    def implement(self):
        pass


if __name__ == '__main__':
    classname = os.path.basename(sys.argv[1]).split('.')[0]

    includes = ['<string>', '<map>', '"TObject.h"', '"TFile.h"', '"TTree.h"']
    prefixes = []
    all_branches = []
    functions = []
    in_function = None

    with open(sys.argv[1], 'r') as config_file:
        for raw_line in config_file:
            line = raw_line.strip()

            # Skip empty lines or comments (!)
            if not line or line[0] == '!':
                continue

            if line == '<--':
                if in_function:
                    functions.append(in_function)
                in_function = None
                prefixes = []
                continue

            # Check for includes
            match = re.match(r'(".*")', line)
            if match:
                includes.append(match.group(1))
                continue

            # Check for default values or reset function signature
            match = re.match(r'{(/(\w))?(-?\d+)?(reset\(.*\))?}', line)
            if match:
                if match.group(2):
                    DEFAULT_TYPE = match.group(2)
                elif match.group(3):
                    DEFAULT_VAL = match.group(3)
                elif match.group(4):
                    RESET_SIGNATURE = match.group(4)
                continue

            match = re.match(r'([\w,]*)\s?-->\s?([\w_]*\(.*\))?', line)
            if match:
                components = match.group(1).split(',')
                prefixes = sum([['%s%s' % (pref, comp) for comp in components] for pref in prefixes], []) or components
                function_sig = match.group(2)
                if function_sig:
                    in_function = Function(function_sig)
                continue

            match = re.match(r'(\w*)(/(\w))?(\s?=\s?(\w+))?(\s?->\s?(.*))?', line)
            if match:
                var = match.group(1)
                data_type = match.group(3) or DEFAULT_TYPE
                val = match.group(5) or DEFAULT_VAL

                branches = [Branch('%s_%s' % (pref, var), data_type, val) for pref in prefixes] or [Branch(var, data_type, val)]

                all_branches.extend(branches)
                if in_function is not None:
                    in_function.add_var(var, match.group(7))
                continue

    # Let's put the branches in some sort of order
    all_branches.sort(lambda x, y: cmp(x.name, y.name))

    with open('%s.h' % classname, 'w') as output:
        write = lambda x: output.write('%s\n' % x)

        write("""#ifndef CROMBIE_{0}_H
#define CROMBIE_{0}_H
""".format(classname.upper()))
        for inc in includes:
            write('#include %s' % inc)

        write('\nclass %s {' % classname)
        write("""
 public:
  {0}(const char* name, const char* outfile_name);
  ~{0}() {1}
""".format(classname, '{ write(t); f->Close(); }'))

        for branch in all_branches:
            write('  %s %s;' % (TYPES[branch.data_type], branch.name))

        write("""
  void %s;
  void fill() { t->Fill(); }
  void write(TObject* obj) { f->WriteTObject(obj, obj->GetName()); }

 private:
  TFile* f;
  TTree* t;  
""" % RESET_SIGNATURE)

        write('};')

        write("""
%s::%s(const char* name, const char* outfile_name) {
  f = new TFile(outfile_name, "RECREATE");
  t = new TTree(name, name);

  %s
}
""" % (classname, classname, '\n  '.join(['t->Branch("{0}", &{0}, "{0}/{1}");'.format(b.name, b.data_type) for b in all_branches])))



        write('\n#endif')
