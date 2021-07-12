#!/usr/bin/env python
import os
import sys
import argparse


class MetaFileParser:

    def __init__(self):
        self._flags = []
        self._vars = {}
        self.file_buffer = None
        self.line_cnt = None

    def read_argparse(self, flags_list, var_list):
        self._flags = flags_list
        for attr, val in var_list:
            self._vars[attr] = val

    def parse_file(self, file_in):
        self.file_buffer = []
        self.line_cnt = 0
        try:
            if not self._parse_file(file_in, True, 0):
                raise ValueError('Unexpected closing bracket')
        except ValueError as e:
            raise ValueError('Line %d: %s' % (self.line_cnt, e))

    def _parse_file(self, file_in, do_print, depth):
        while True:
            try:
                line = next(file_in)
                self.line_cnt += 1
                dbl_bracket_start = line.find('[[')
                dbl_bracket_end = line.find(']]')
                if dbl_bracket_start != -1:
                    condition = line[dbl_bracket_start + 1:]
                    if do_print:
                        do_print_ = self._eval_condition(condition)
                    else:
                        do_print_ = False
                    self._parse_file(file_in, do_print_, depth+1)
                elif dbl_bracket_end != -1:
                    return False
                else:
                    if do_print:
                        self._parse_line(line)
            except StopIteration:
                if depth == 0:
                    return True
                else:
                    raise ValueError('Unexpected end of file, expected ]]')

    def _parse_line(self, line):
        bracket_start = line.find('[')
        bracket_end = line.find(']')
        if bracket_start != -1 and bracket_end != -1:
            condition = line[bracket_start+1:bracket_end]
            line = line[:bracket_start] + line[bracket_end+1:]
            if not self._eval_condition(condition):
                return

        self.file_buffer.append(line)

    def _eval_condition(self, condition):
        cond_ = condition.replace('==', ' == ').replace('!=', ' != ')
        fields = cond_.split()
        return self._eval_fields(fields)

    def _eval_fields(self, fields):
        if 'or' in fields:
            idx = fields.index('or')
            left = self._eval_fields(fields[:idx])
            right = self._eval_fields(fields[idx + 1:])
            return left or right
        if 'and' in fields:
            idx = fields.index('and')
            left = self._eval_fields(fields[:idx])
            right = self._eval_fields(fields[idx + 1:])
            return left and right
        if len(fields) > 0 and fields[0] == 'not':
            return not self._eval_fields(fields[1:])

        if len(fields) == 1:
            return self._eval_flag(fields[0])

        if len(fields) == 3:
            if fields[1] in ['==', '!=', 'startswith', 'endswith', 'contains']:
                left = self._eval_expression(fields[0])
                if fields[1] == '==':
                    return left == fields[2]
                elif fields[1] == '!=':
                    return left != fields[2]
                elif fields[1] == 'startswith':
                    return left.startswith(fields[2])
                elif fields[1] == 'endswith':
                    return left.endswith(fields[2])
                elif fields[1] == 'contains':
                    return fields[2] in left

        raise ValueError('Syntax Error')

    def _eval_flag(self, name):
        if '%' in name:
            raise ValueError('Flags can\'t have default values')
        else:
            return name in self._flags

    def _eval_expression(self, name):
        if name == 'platform':
            return sys.platform
        else:
            return self._eval_var(name)

    def _eval_var(self, name):
        name_cleaned = name.split('%')[0]
        if name_cleaned not in self._vars:
            if '%' in name:
                return name.split('%')[1]
            else:
                raise ValueError('Custom variable \'%s\' neither set with command line options '
                                 'nor given default value' % name)
        else:
            return self._vars[name_cleaned]

    def write_result(self, file_out):
        for line in self.file_buffer:
            file_out.write(line)


def main():
    # Read command line arguments
    argparser = argparse.ArgumentParser()
    argparser.add_argument('-i', '--input',
                           help='specify meta environment input file')
    argparser.add_argument('-o', '--output',
                           help='specify environment output file')
    argparser.add_argument('-f', '--flag', type=str, action='append', default=[],
                           help='set custom flag for parsing')
    argparser.add_argument('-v', '--variable', type=str, nargs=2, action='append', default=[],
                           help='define custom variable for parsing')
    argparser.add_argument('-q', '--quiet', action='store_true',
                           help='quietly overwrite output if already exists')
    argparser.add_argument('-p', '--parse-only', action='store_true',
                           help='do not invoke conda afterwards')
    argparser.add_argument('-c', '--no-comment', action='store_true',
                           help='do not add auto-generated comment to output')
    args = argparser.parse_args()

    # Create parser instance
    metafileparser = MetaFileParser()
    metafileparser.read_argparse(args.flag, args.variable)

    # Get input file
    dir_path = os.path.dirname(os.path.realpath(__file__))
    if args.input is None:
        meta_file_name = 'environment.yml.meta'
        meta_file = os.path.join(dir_path, meta_file_name)
        if not os.path.exists(meta_file):
            meta_file = os.path.join(dir_path, '..', meta_file_name)
    else:
        meta_file_name = os.path.basename(args.input)
        meta_file = args.input

    if not os.path.exists(meta_file):
        sys.stderr.write('Error: Could not find input file')
        sys.exit(1)

    # Get output file
    if args.output is None:
        if meta_file.endswith('.meta'):
            env_file = meta_file[:-5]
        else:
            env_file = meta_file + '_out'
    else:
        env_file = args.output

    if os.path.exists(env_file) and not args.quiet:
        input_query = 'Output file %s already exists, overwrite ([y]/n)?' % env_file
        while True:
            try:
                input_ = raw_input
            except NameError:
                input_ = input
            answer = input_(input_query)
            if answer in ['', 'Y', 'y', 'yes', 'Yes', 'YES']:
                break
            elif answer in ['N', 'n', 'no', 'No', 'NO']:
                return
            input_query = 'Invalid answer, please answer yes or no ([y]/n):'

    # Read input
    with open(meta_file, 'r') as file_in:
        try:
            metafileparser.parse_file(file_in)
        except ValueError as e:
            sys.stderr.write('Error: %s\n' % e)
            sys.exit(1)

    # Write output
    with open(env_file, 'w') as file_out:
        if not args.no_comment:
            file_out.write('# This file was auto-generated from %s\n' % meta_file_name)
        metafileparser.write_result(file_out)

    sys.stdout.write('Created %s successfully\n' % env_file)

    # Invoke conda
    if not args.parse_only:
        os.system('conda env create -f %s' % env_file)


if __name__ == '__main__':
    main()
