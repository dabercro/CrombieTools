#! /usr/bin/env python

import os
import sys
import re
import copy


TYPES = {
    'I': 'Int_t',
    'l': 'Long_t',
    'F': 'Float_t',
    'O': 'Bool_t'
}

DEFAULT_TYPE = 'F'
DEFAULT_VAL = '0'
RESET_SIGNATURE = 'reset()'


class Branch:
    def __init__(self, name, data_type, default_val, is_saved):
        self.name = name
        self.data_type = data_type
        self.default_val = default_val
        self.is_saved = is_saved


class Function:
    def __init__(self, signature, prefixes):
        self.enum_name = re.match(r'(.*)\(.*\)', signature).group(1)
        self.signature = 'set_' + signature.replace('(', '(const %s base, ' % self.enum_name).replace(', ', ', const ')
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
  enum class %s : int {
    %s
  };
  const std::vector<std::string> %s_names = {
    %s
  };
  """ % (self.enum_name, ',\n    '.join(['%s = %s' % (pref, index) for index, pref in enumerate(self.prefixes)]),
         self.enum_name, ',\n    '.join(['"%s"' % pref for pref in self.prefixes])))


    def implement(self, classname):
        incr = ['++', '--']
        return """void %s::%s {
  %s
}
""" % (classname, self.signature,
       '\n  '.join(['{3}*({2}*)(t->GetBranch(({1}_names[static_cast<int>(base)] + "{0}").data())->GetAddress());'. format(var, self.enum_name, t, val)
                    for var, val, t in self.variables if val in incr] +
                   ['set({3}_names[static_cast<int>(base)] + "{0}", static_cast<{1}>({2}));'.format(var, t, val, self.enum_name)
                    for var, val, t in self.variables if val not in incr]
       ))


if __name__ == '__main__':
    classname = os.path.basename(sys.argv[1]).split('.')[0]

    includes = ['<string>', '<vector>', '"TObject.h"', '"TFile.h"', '"TTree.h"']
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
            match = re.match(r'([<"].*[">])', line)
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

            match = re.match(r'(\+?[\w,]*)\s?-->\s?([\w_]*\(.*\))?', line)
            if match:
                # Pass off previous function as quickly as possible to prevent prefix changing
                if in_function:
                    functions.append(copy.deepcopy(in_function))
                    in_function = None

                components = match.group(1).split(',') if match.group(1) else []
                if components:
                    if match.group(1).startswith('+'):
                        components[0] = components[0].lstrip('+')
                        prefixes += components
                    else:
                        prefixes = sum([['%s%s' % (pref, comp) for comp in components] for pref in prefixes], []) or components

                function_sig = match.group(2)
                if function_sig:
                    in_function = Function(function_sig, prefixes)
                continue

            match = re.match(r'(\#\s*)?(\w*)(/(\w))?(\s?=\s?([\w\.\(\)]+))?(\s?->\s?(.*))?', line)
            if match:
                var = match.group(2)
                data_type = match.group(4) or DEFAULT_TYPE
                val = match.group(6) or DEFAULT_VAL
                is_saved = not bool(match.group(1))

                branches = [Branch(('%s_%s' % (pref, var)).rstrip('_'), data_type, val, is_saved) for pref in prefixes] or [Branch(var, data_type, val, is_saved)]

                all_branches.extend(branches)
                if in_function is not None:
                    in_function.add_var(var, match.group(8), data_type)
                continue

    # Let's put the branches in some sort of order
    all_branches.sort(lambda x, y: cmp(x.name, y.name))

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
  void fill() { t->Fill(); }
  void write(TObject* obj) { f->WriteTObject(obj, obj->GetName()); }
  %s

 private:
  TFile* f;
  TTree* t;  

  template <typename T>
  void set(std::string name, T val) { *(T*)(t->GetBranch(name.data())->GetAddress()) = val; }
};
""" % (RESET_SIGNATURE, '\n  '.join([f.declare(functions) for f in functions])))

        # Initialize the class
        write("""%s::%s(const char* outfile_name, const char* name) {
  f = new TFile(outfile_name, "CREATE");
  t = new TTree(name, name);
  %s
}
""" % (classname, classname, '\n  '.join(['t->Branch("{0}", &{0}, "{0}/{1}");'.format(b.name, b.data_type) for b in all_branches if b.is_saved])))

        # reset function
        write("""
void %s::%s {
  %s
}
""" % (classname, RESET_SIGNATURE, '\n  '.join(['{0} = {1};'.format(b.name, b.default_val) for b in all_branches])))

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
            if prev_func and func.prefixes != prev_func.prefixes:
                if False not in [prev in func.prefixes for prev in prev_func.prefixes]:
                    write_convert(prev_func, func)
                    write_convert(func, prev_func)

            prev_func = func

        write('#endif')
