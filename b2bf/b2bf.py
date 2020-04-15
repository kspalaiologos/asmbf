#!/usr/bin/env python3
# B-like to Brainfuck compiler.
# Copyright (C) 2020 Maviek
# License: MIT
# TODO:
#  * Add more optimizations
#  * Add argument parser
#  * Add support for switch/case
#  * Fix bitwise ops

import sys, string

try:
    import lark
except:
    print('Error: install lark using: `pip3 install lark-parser`')


class GrammarBuilder:

    def __init__(self, grammar):
        self.grammar = grammar + '\n'

    def __call__(self, line):
        self.grammar += line + '\n'


_ = GrammarBuilder('''
COMMENT: /\\/\\*(\\*(?!\\/)|[^*])*\\*\\//
NAME: /[a-zA-Z_]\\w*/

constant: /(0x[A-Za-z0-9]+)|([0-9]+)/ -> int
        | /'([^']|(\\'))'/            -> chr
        | /".*?(?<!\\\\)"/            -> str

?rvalue: or_test | ternary
?ternary: or_test "?" rvalue ":" rvalue
?or_test: and_test (_or_test_op and_test)*
?and_test: not_test (_and_test_op not_test)*
?not_test: "!" not_test -> not
         | comparison
?comparison: expr (_comp_op expr)*
?expr: xor_expr (_or_op xor_expr)*
?xor_expr: and_expr (_xor_op and_expr)*
?and_expr: shift_expr (_and_op shift_expr)*
?shift_expr: arith_expr (_shift_op arith_expr)*
?arith_expr: term (_add_op term)*
?term: factor (_mul_op factor)*
?factor: _add_op factor | lvalue

!_or_test_op: "||"
!_and_test_op: "&&"
!_or_op: "|"
!_xor_op: "^"
!_and_op: "&"
!_add_op: "+"|"-"
!_shift_op: "<<"|">>"
!_mul_op: "*"|"/"|"%"
!_comp_op: "<"|">"|"=="|">="|"<="|"!="
!_inc_dec: "++"|"--"
!_assign_op: "="|"=+"|"=-"|"=<<"|"=>>"|"=*"|"=/"|"=%"

?lvalue: NAME "(" [arguments] ")"  -> funccall
       | lvalue "[" rvalue "]"     -> get_item
       | "*" atom                  -> star_expr
       | "&" atom                  -> get_addr
       | lvalue _assign_op rvalue  -> assign
       | inc_dec_expr
       | atom

inc_dec_expr: (_inc_dec NAME) | (NAME _inc_dec)
?atom: NAME -> var | constant | "(" rvalue ")"

arguments: rvalue ("," rvalue)*

%ignore /[\\t \\f\\n]+/
%ignore COMMENT
''')


class Transformer(lark.Transformer):

    _('start: (gvar | func)*')

    def start(self, _):
        return _

    def inc_dec_expr(self, _):
        return ('inc_dec_expr', _[0], _[1])

    def var(self, _):
        return ('var', _[0].value)

    def get_item(self, _):
        return ('get_item', _[0], _[1])

    def int(self, _):
        return eval(_[0].value)

    def chr(self, _):
        return ord(eval(_[0].value))

    def str(self, _):
        return eval(_[0].value)

    _('gvar: NAME (garr | ival) ";"')

    def gvar(self, _):
        _[0] = _[0].value
        if 'arr' in _[1]:
            return ('arr', _[0], _[1][1], _[1][2])
        else:
            if len(_[1]) > 1:
                raise Exception(
                    'cannot assign more than 1 value to single variable')
            return ('var', _[0], _[1][0] if len(_[1]) == 1 else 0)

    _('garr: "[" constant "]" ival')

    def garr(self, _):
        return ('arr', _[0], _[1])

    _('ival: [constant ("," constant)*]')

    def ival(self, _):
        return _

    _('func: NAME "(" fargs ")" statement')

    def func(self, _):
        return ('func', _[0].value, _[1], _[2])

    _('fargs: (NAME ("," NAME)*)?')

    def fargs(self, _):
        return [e.value for e in _]

    _('block: "{" statement* "}"')

    def block(self, _):
        return ('block', _)

    _('statement: auto | extrn | label | goto | block | asm | rval_stmt | ret | switch | case | if_stmt | while_stmt | for_stmt')

    def statement(self, _):
        return _[0]

    _('extrn: "extrn" NAME ("," NAME)* ";"')

    def extrn(self, _):
        return ('extrn', [e.value for e in _])

    _('case: "case" constant ":" statement')

    def case(self, _):
        return ('case', _[0], _[1])

    _('rval_stmt: [rvalue] ";"')

    def rval_stmt(self, _):
        return ('rvalue', _[0] if len(_) > 0 else None)

    _('switch: "switch" rvalue statement')

    def switch(self, _):
        return ('switch', _[0], _[1])

    _('if_stmt: "if" "(" rvalue ")" statement ["else" statement]')

    def if_stmt(self, _):
        if len(_) == 2:
            _.append([])
        return ('if', _[0], _[1], _[2])

    _('while_stmt: "while" "(" rvalue ")" statement')

    def while_stmt(self, _):
        return ('while', _[0], _[1])

    _('for_stmt: "for" "(" rvalue ";" rvalue ";" rvalue ")" statement')

    def for_stmt(self, _):
        return ('block', [
                ('rvalue', _[0]),
                ('while', _[1], ('block',
                    [_[3], ('rvalue', _[2])]
                ))])

    _('ret: "return" [rvalue] ";"')

    def ret(self, _):
        return ('ret', _[0] if len(_) > 0 else 0)

    _('label: NAME ":"')

    def label(self, _):
        return ('label', _[0])

    _('goto: "goto" NAME ";"')

    def goto(self, _):
        return ('goto', _[0])

    _('asm: "asm" "(" constant ")" ";"')

    def asm(self, _):
        if isinstance(_[0], int):
            raise Exception('inline assembly param must be constant string')
        return ('asm', _[0])

    _('auto: "auto" lvar ("," lvar)* ";"')

    def auto(self, _):
        return ('auto', _)

    _('lvar: NAME [constant]')

    def lvar(self, _):
        return (_[0].value, _[1] if len(_) == 2 else 0)


parser = lark.Lark(_.grammar,
                   parser='lalr',
                   lexer='standard',
                   propagate_positions=False,
                   maybe_placeholders=False,
                   transformer=Transformer())


class Codegen:

    def __init__(self, ast):
        # Output list of lines.
        # First line is empty and is getting removed by
        # `get_output`. We need it when some function
        # checks last line, but there are no already
        # emitted lines.
        self.output = ['']

        # Data labels
        self.data_labels = []

        # List of already emitted strings.
        # Key is string, value is data label.
        self.strings = {}

        # String ID.
        self.str_id = 0

        # Label ID.
        self.label_id = 0

        # Global variables.
        self.globals = []

        # Global variables size.
        self.mm_base = 1

        # Page size for memory allocator.
        # Minimum value is 16, but may be higher if
        # any function has more local variables than 16.
        self.page_size = 16

        # Memory sizes by function name.
        self.memory_sizes = {}

        # Generate global variables.
        for v in [v for v in ast if v[0] == 'var']:
            if isinstance(v[2], str):
                id = self.emit_string(v[2])
                for _ in range(len(v[2]) + 1):
                    self.globals.append(None)
                self.data_labels.append(f'db_ {id}')
            else:
                self.data_labels.append(f'db_ {v[2]}')
            self.globals.append(v[1])
            self.mm_base += 1

        # Generate global arrays.
        for v in [v for v in ast if v[0] == 'arr']:
            self.globals.append(v[1])
            self.mm_base += v[2]
            for e in v[3]:
                self.globals.append(None)
                self.data_labels.append(f'db_ {e}')
            fill = v[2] - len(v[3])
            if fill != 0:
                self.data_labels.append(f'#times("db_ 0", {fill})')

        # Compile each function.
        for f in [f for f in ast if f[0] == 'func']:
            self.func(f[1], f[2], f[3])

    def get_memory_size(self, function_name):
        if function_name in self.memory_sizes:
            # If function was already compiled and we
            # know its memory size, we can return it.
            return self.memory_sizes[function_name]
        else:
            return None

    def emit_string(self, str):
        # Increase string ID.
        self.str_id += 1

        if not str in self.strings:
            # Add string to dict.
            self.strings[str] = f'*str{self.str_id}'
        else:
            # If same string was already emitted,
            # we can use it.
            return self.strings[str]

        # Add label.
        self.data_labels.append(f'&str{self.str_id}')
        self.data_labels.append('\ttxt "')

        # Characters acceptable by `txt`.
        acceptable = (string.whitespace + string.ascii_letters + string.digits +
                      '!#$%&\'()*+,-./:;<=>?@[]^_`{|}~')

        for c in str:
            # Emit each char.
            if c in acceptable:
                self.data_labels[-1] += c
            else:
                self.data_labels[-1] += '"'
                self.data_labels.append(f'\tdb_ {ord(c)}')
                self.data_labels.append('\ttxt "')

        # Close last `txt` string.
        self.data_labels[-1] += '"'

        # Remove empty string in `txt`.
        self.data_labels = list(filter('\ttxt ""'.__ne__, self.data_labels))

        # Add NULL at the end of string.
        self.data_labels.append('\tdb_ 0')

        # Increase MM Base.
        self.mm_base += len(str) + 1

        # Return string data label.
        return f'*str{self.str_id}'

    def emit(self, code):
        if code.startswith('#'):
            # Inline Lua code cannot contain tab
            # before `#` character.
            self.output.append(code)
        else:
            if self.output[-1].startswith('\tpsh') and code.startswith('pop'):
                # If last line is push, and currently emitted is pop,
                # we can change this into mov instruction.
                # For example:
                #
                #| Original |  Optimized  |
                #|==========|=============|
                #| psh 123  |             |
                #| pop r1   | mov r1, 123 |
                #| psh r2   |             |
                #| pop r1   | mov r1, r2  |
                #| psh r1   |             |
                #| pop r1   |             |
                #
                # Hopefully, it won't break anything.
                # Get pushed value and register used in pop.
                pushed = self.output.pop()[4:].strip()
                reg = code[3:].strip()

                if reg != pushed:
                    # If pushed value is not the same register,
                    # we can move this value into it.
                    self.output.append(f'\tmov {reg}, {pushed}')
            elif self.output[-1].startswith('\tmov r3') and code == 'sto r6, r3':
                # As long as we don't use `r3` anymore,
                # we can store value using `sto r6, {}`.
                # For example:
                #
                #|   Original   |  Optimized  |
                #|==============|=============|
                #| mov r3, 32   |             |
                #| sto r3, r6   | sto r6, r3  |
                #
                # Get moved value.
                value = self.output[-1][9:].strip()

                # Store it directly in address from `r6`.
                self.output.append(f'\tsto r6, {value}')
            else:
                # Add tab indent for code.
                self.output.append(f'\t{code}')

    def emit_label(self, label):
        # Add @ before label name.
        self.output.append(f'@{label}')

    def lvalue(self, expr):
        # Compile left expression.

        if isinstance(expr, tuple) and expr[0] == 'var':
            if expr[1] in self.globals:
                # Get global variable address.
                addr = self.globals.index(expr[1])

                # Push address into stack.
                self.emit(f'psh {addr}')

            else:
                # Get local variable offset.
                offset = self.locals.index(expr[1])

                # Get memory address into `r1`.
                self.emit('mov r1, r4')

                # Add offset to `r1` if needed.
                if offset != 0:
                    self.emit(f'add r1, {offset}')

                # Push value into stack.
                self.emit('psh r1')

        elif isinstance(expr, tuple) and expr[0] == 'get_item':
            self.lvalue(expr[1])
            self.rvalue(expr[2])

            self.emit('pop r2')
            self.emit('pop r1')
            self.emit('add r1, r2')
            self.emit('psh r1')

        elif isinstance(expr, lark.Tree) and expr.data == 'star_expr':
            self.rvalue(expr.children[0])

        else:
            raise Exception(f'invalid lvalue: {expr}')

    def rvalue(self, expr):
        # Compile right expression.

        if isinstance(expr, int):
            # Compile integer or char.
            # ('A' is automatically translated to 65).
            self.emit(f'psh {expr}')

        elif isinstance(expr, str):
            # Emit string at begining of output
            # and get data label for it.
            str_id = self.emit_string(expr)

            # Push pointer to string into stack.
            self.emit(f'psh {str_id}')

        elif isinstance(expr, lark.Tree) and expr.data == 'funccall':
            # Get function name.
            name = expr.children[0].value

            # Get memory size if exists, when
            # it's unknown we get `None` here.
            memory_size = self.get_memory_size(name)

            # If there are no variables or parameters,
            # memory alloction for function is useless,
            # because its not used, so we allocate memory
            # only if needed.
            if memory_size != 0:

                # Store current pointer.
                self.emit('psh r4')

                # Initialize memory for local variables.
                self.emit('#call("alloc")')

            # Get parameters.
            args = expr.children[1].children if len(expr.children) > 1 else []

            # Offset for `r6`.
            offset = 0

            # Another parameters goes directly into memory.
            for i, arg in enumerate(args):
                # Compile expression.
                self.rvalue(arg)

                # Store result in next memory cell.
                self.emit('pop r3')
                self.emit('sto r6, r3')

                if i != len(args) - 1:
                    self.emit('inc r6')
                    offset += 1

            if offset != 0:
                self.emit(f'sub r6, {offset}')

            if memory_size != 0:
                self.emit('mov r4, r6')

            # Call function.
            self.emit(f'#call("_{name}")')

            # Free memory and restore pointer
            # only if it was actually allocated.
            if memory_size != 0:

                # Free allocated memory.
                # Hopefully, `r4` still
                # contains the same pointer.
                self.emit('#free("r4")')

                # Restore `r4`.
                self.emit('pop r4')

            # Result of any function is stored in `r1`.
            # We need to push this value into stack.
            self.emit('psh r1')

        elif isinstance(expr, lark.Tree) and expr.data == 'get_addr':
            self.lvalue(expr.children[0])

        elif isinstance(expr, lark.Tree) and expr.data == 'star_expr':
            self.rvalue(expr.children[0])
            self.emit('pop r1')

            # Get value which variable points to.
            self.emit('rcl r2, r1')

            # Push value into stack.
            self.emit('psh r2')

        elif isinstance(expr, lark.Tree) and expr.data in (
                'arith_expr', 'term', 'shift_expr', 'xor_expr', 'expr',
                'comparison', 'and_test', 'or_test', 'and_expr'):
            to_emit = None
            for i, e in enumerate(expr.children):
                if i == 0:
                    self.rvalue(e)
                elif i % 2 == 0:
                    self.rvalue(e)
                    for e in to_emit:
                        self.emit(e)
                else:
                    to_emit = ('pop r2', 'pop r1', {
                        '+': 'add r1, r2',
                        '-': 'sub r1, r2',
                        '*': 'mul r1, r2',
                        '/': 'div r1, r2',
                        '%': 'mod r1, r2',
                        '<': 'lt_ r1, r2',
                        '>': 'gt_ r1, r2',
                        '<=': 'le_ r1, r2',
                        '>=': 'ge_ r1, r2',
                        '==': 'eq_ r1, r2',
                        '!=': 'ne_ r1, r2',
                        '<<': '#call("shl")',
                        '>>': '#call("shr")',
                        '||': 'or_ r1, r2',
                        '&&': 'and r1, r2'
                    }[e.value], 'psh r1')

        elif isinstance(expr, lark.Tree) and expr.data == 'not':
            self.rvalue(expr.children[0])
            self.emit('pop r1')
            self.emit('not r1')
            self.emit('psh r1')

        elif isinstance(expr, lark.Tree) and expr.data == 'factor':
            self.rvalue(expr.children[1])
            if expr.children[0] == '-':
                self.emit('pop r1')
                self.emit('neg r1')
                self.emit('psh r1')

        elif isinstance(expr, lark.Tree) and expr.data == 'assign':
            # Get what will be assigned.
            what = expr.children[0]

            # If there is operator like =+,
            # Store '+' in `op`.
            op = expr.children[1].value

            # And get value.
            value = expr.children[2]

            if op == '=':
                # Compute values.
                self.lvalue(what)
                self.rvalue(value)

                # Store value.
                self.emit('pop r2')
                self.emit('pop r1')
                self.emit('sto r1, r2')
                self.emit('psh r2')

            else:
                # Compute values.
                self.lvalue(what)
                self.rvalue(what)
                self.rvalue(value)

                # Get instruction.
                inst = {
                    '=+': 'add',
                    '=-': 'sub',
                    '=*': 'mul',
                    '=/': 'div',
                    '=%': 'mod',
                }[op]

                # Store value.
                self.emit('pop r3')
                self.emit('pop r2')
                self.emit('pop r1')
                self.emit(f'{inst} r2, r3')
                self.emit('sto r1, r2')
                self.emit('psh r2')

        elif isinstance(expr, tuple) and expr[0] == 'inc_dec_expr':
            left, right = expr[1:]

            if left.value in ('++', '--'):
                left, right = right, left
                after = False
            else:
                after = True

            if left.value in self.locals:
                # Get local variable offset.
                offset = self.locals.index(left.value)

                # Add offset to `r4` if needed.
                if offset != 0:
                    self.emit(f'add r4, {offset}')

                # Increase/decrease variable value.
                self.emit('rcl r1, r4')
                self.emit('inc r1' if right.value == '++' else 'dec r1')
                self.emit('sto r4, r1')

                # Restore `r4`.
                if offset != 0:
                    self.emit(f'sub r4, {offset}')

                # Push result
                if after:
                    self.emit('dec r1' if right.value == '++' else 'inc r1')
                self.emit('psh r1')
            else:
                # Get global variable address.
                addr = self.globals.index(left.value)

                # Increase/decrease variable value.
                self.emit(f'mov r2, {addr}')
                self.emit('rcl r1, r2')
                self.emit('inc r1' if right.value == '++' else 'dec r1')
                self.emit('sto r2, r1')

                # Push result
                if after:
                    self.emit('dec r1' if right.value == '++' else 'inc r1')
                self.emit('psh r1')

        elif isinstance(expr, tuple) and expr[0] == 'get_item':
            self.lvalue(expr)
            self.emit('pop r1')
            self.emit('rcl r2, r1')
            self.emit('psh r2')

        elif isinstance(expr, tuple) and expr[0] == 'var':
            if expr[1] in self.locals:
                # Get local variable offset.
                offset = self.locals.index(expr[1])

                # Add offset to `r4` if needed.
                if offset != 0:
                    self.emit(f'add r4, {offset}')

                # Get variable value into `r1`.
                self.emit('rcl r1, r4')

                # Restore `r4`.
                if offset != 0:
                    self.emit(f'sub r4, {offset}')

                # Push value into stack.
                # This must be placed at the end, because
                # if `rvalue` is used as statement and
                # `psh` goes at the end, it can be removed,
                # as long as its useless.
                self.emit('psh r1')
            else:
                # Get global variable address.
                addr = self.globals.index(expr[1])

                # Get variable value into `r1`.
                self.emit(f'rcl r1, {addr}')

                # Push value into stack.
                self.emit('psh r1')

        else:
            print('\t\tUNSUPPORTED EXPR:', expr)

    def statement(self, stmt):
        # Get statement type.
        stmt_type = stmt[0]

        # Skip 1st element of tuple.
        stmt = stmt[1:]

        if stmt_type == 'block':
            # `stmt[0]` is list of statements.
            for s in stmt[0]:
                # Compile each statement.
                self.statement(s)

        elif stmt_type == 'rvalue':
            if stmt[0]:
                # Compile expression.
                self.rvalue(stmt[0])

                # Remove `psh` to maintain stack balance.
                if self.output[-1].startswith('\tpsh'):
                    self.output.pop()

        elif stmt_type == 'ret':
            # Compile expression.
            self.rvalue(stmt[0])

            # Function result is stored in `r1`.
            self.emit('pop r1')

            # Emit return from function.
            self.emit('ret')

        elif stmt_type == 'asm':
            # Emit assembly from string.
            self.emit(stmt[0])

        elif stmt_type == 'while':
            # Emit loop label.
            self.label_id += 1
            loop_lbl = self.label_id
            self.emit_label(f'L{loop_lbl}')

            # Compile condition.
            self.rvalue(stmt[0])

            # Skip body if 0.
            self.label_id += 1
            end_lbl = self.label_id
            self.emit('pop r1')
            self.emit(f'jz_ r1, %L{end_lbl}')

            # Emit body.
            self.statement(stmt[1])

            # Loop it.
            self.emit(f'jmp %L{loop_lbl}')

            # Emit end label.
            self.emit_label(f'L{end_lbl}')

        elif stmt_type == 'if':
            # Compile condition.
            self.rvalue(stmt[0])

            # Skip body if 0.
            self.label_id += 1
            else_lbl = self.label_id
            self.label_id += 1
            end_lbl = self.label_id
            self.emit('pop r1')
            self.emit(f'jz_ r1, %L{else_lbl}')

            # Emit `if` body.
            self.statement(stmt[1])

            if stmt[2] != []:
                # Skip else if 1.
                self.emit(f'jmp %L{end_lbl}')

            # Emit `else` label.
            self.emit_label(f'L{else_lbl}')

            if stmt[2] != []:
                # Emit else body.
                self.statement(stmt[2])
                self.emit_label(f'L{end_lbl}')

        elif stmt_type == 'auto':
            for var in stmt[0]:
                self.locals.append(var[0])

                # Get variable offset.
                offset = self.locals.index(var[0])

                if offset != 0:
                    # Add offset if needed.
                    self.emit(f'add r4, {offset}')

                if isinstance(var[1], int):
                    # Emit default value as int.
                    self.emit(f'sto r4, {var[1]}')
                else:
                    # The only constant type left is string.
                    # Emit string at begining of output
                    # and get data label for it.
                    str_id = self.emit_string(var[1])

                    # Emit default value as string.
                    self.emit(f'sto r4, {str_id}')

                if offset != 0:
                    # Restore `r4`.
                    self.emit(f'sub r4, {offset}')

        elif stmt_type == 'label':
            self.emit_label(f'L{stmt[0]}')

        elif stmt_type == 'goto':
            self.emit(f'jmp %L{stmt[0]}')

        else:
            print('\t\tUNSUPPORTED STMT:', stmt_type, stmt)

    def func(self, name, args, body):
        # Set local variable offsets.
        # self.locals.index(var)  -> Get (address of var) - r4
        # self.locals.append(var) -> Add new variable.
        self.locals = args

        # Store function name.
        self.function_name = name

        # Emit function label.
        self.emit_label('_' + name)

        # Generate output for function body.
        self.statement(body)

        if self.output[-1] != '\tret':
            # Add return statement at end of
            # function if one is not there.
            self.emit('ret')

        if len(self.locals) > self.page_size:
            # At the end of function generator,
            # `self.locals` should contain all local variables.
            # If count of local variables is greater than current
            # page size, we need to increase page size.
            self.page_size = len(self.locals)

        # Store memory size when we have all
        # local variables in `self.locals`,
        # so we know how big memory is.
        self.memory_sizes[name] = len(self.locals)

    def get_output(self):
        # Add some code before output and join everything as string.
        return (('#!/usr/bin/env bfmake\n\tstk 16\n\torg 0\n$(\n'
                 'function times(s, n) for i=1,n,1 do print(s) end end\n)\n' +
                 '\n'.join(self.data_labels) + '\n' +
                 f'#PAGE_SIZE = {self.page_size}\n#MM_BASE = {self.mm_base}\n'
                 '#call("alloc")\n\tmov r4, r6\n#call("_main")\n\tend\n'
                 '@alloc\n#alloc("r6", "r5")\n\tret\n'
                 '@shl\n\tpsh r2\n\tnav r2\n\traw .[\n'
                 '\tasl r1\n\tdec r2\n\tnav r2\n'
                 '\traw .]\n\tpop r2\n\tret\n'
                 '@shr\n\tpsh r2\n\tnav r2\n\traw .[\n'
                 '\tasr r1\n\tdec r2\n\tnav r2\n'
                 '\traw .]\n\tpop r2\n\tret\n' +
                 '\n'.join(self.output[1:])).replace('\t', ' ' * 4))


with open(sys.argv[1]) as f:
    ast = parser.parse(f.read())
    output = Codegen(ast).get_output()
    print(output)
