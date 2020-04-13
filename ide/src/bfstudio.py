#!/usr/bin/env python3
#  ___    ___   ___    ___   __  __       ___   _____   _   _   ___    ___    ___
# | _ )  | __| /   \  / __| |  \/  |     / __| |_   _| | | | | |   \  |_ _|  / _ \
# | _ \  | _|  | - |  \__ \ | |\/| |     \__ \   | |   | |_| | | |) |  | |  | (_) |
# |___/  |_|   |_|_|  |___/ |_|__|_|     |___/   |_|    \___/  |___/  |___|  \___/
#
# Simple IDE for bfasm, with syntax highlighting and more.
# Copyright (C) 2020 Maviek
# License: MIT

import time
import re
import tty
import termios
import os
import sys
import enum
import subprocess
import string
import tempfile


class Enum(enum.Enum):

    def __new__(cls):
        value = len(cls.__members__) + 1000
        obj = object.__new__(cls)
        obj._value_ = value
        return obj


class EditorKeys(Enum):
    ARROW_UP = ()
    ARROW_DOWN = ()
    ARROW_LEFT = ()
    ARROW_RIGHT = ()
    PAGE_UP = ()
    PAGE_DOWN = ()
    HOME_KEY = ()
    END_KEY = ()
    DEL_KEY = ()
    BACKSPACE = ()
    TAB_KEY = ()


def pprint(*args, file=sys.stdout):
    print(*args, end='', file=file, flush=True)


def bf_exit(message=None):
    pprint("\x1b[2J")
    pprint("\x1b[H")
    if message:
        pprint(message, file=sys.stderr)
    sys.exit(0)


def is_ctrl(char):
    return 0 <= ord(char) <= 31


def ctrl_key(char):
    return chr(ord(char) & 0x1F)


def getch():
    fd = sys.stdin.fileno()
    old = termios.tcgetattr(fd)
    ch = None
    try:
        tty.setraw(fd)
        ch = sys.stdin.read(1)
    except:
        bf_exit("error reading from stdin")
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old)
    return ch


def get_terminal_size():
    s = list(map(int, os.popen('stty size', 'r').read().split()))
    if not s:
        s = [24, 80]
    return s


def convert_string_to_rows(s):
    return s.split('\n')


def convert_rows_to_string(rows):
    return '\n'.join(rows)


syntax = {
    "asm": {
        "keywords": [
            'add', 'add', 'and', 'amp', 'amp', 'asr', 'asl', 'clr', 'db_',
            'dec', 'div', 'div', 'end', 'eq_', 'eq_', 'ge_', 'ge_', 'gt_',
            'gt_', 'in_', 'inc', 'jmp', 'jmp', 'jnz', 'jnz', 'jz_', 'jz_',
            'lbl', 'le_', 'le_', 'log', 'lt_', 'lt_', 'mod', 'mod', 'mov',
            'mov', 'mul', 'mul', 'ne_', 'ne_', 'neg', 'not', 'or_', 'or_',
            'org', 'out', 'out', 'pop', 'pow', 'pow', 'psh', 'psh', 'raw',
            'rcl', 'rcl', 'ret', 'seg', 'smp', 'smp', 'srv', 'stk', 'sto',
            'sto', 'sub', 'sub', 'swp', 'txt'
        ],
        "regs": ['r1', 'r2', 'r3', 'r4', 'r5', 'r6'],
        "comment": ';',
        "inline": '#',
        "labels": ['@', '&', '*', '%']
    },
    "lua": {
        "keywords": [
            'and', 'break', 'do', 'else', 'elseif', 'end', 'false', 'for',
            'function', 'goto', 'if', 'in', 'local', 'nil', 'not', 'or',
            'repeat', 'return', 'then', 'true', 'until', 'while'
        ],
        "comment": '--'
    }
}


def init_bfstudio():
    global screen_rows, screen_cols, cx, cy, \
        file_loaded, file_rows, row_offset, column_offset, \
        file_name, file_type, status_message, status_message_time, \
        ul, quit_times, last_match, direction
    cx, cy = 0, 0
    screen_rows, screen_cols = get_terminal_size()
    screen_rows -= 1
    file_loaded = False
    file_type = None
    file_rows = []
    row_offset = 0
    column_offset = 0
    file_name = None
    status_message = ""
    status_message_time = 0
    ul = False
    quit_times = 1
    last_match = (0, -1)
    direction = 1


def reset_ul():
    global ul, quit_times
    ul = False
    quit_times = 1


def raw_read():
    c = getch()
    if c == chr(127):
        return EditorKeys.BACKSPACE
    elif c == '\t':
        return EditorKeys.TAB_KEY
    elif c == '\x1b':
        c1 = getch()
        c2 = getch()
        if (not c1) or (not c2):
            return c
        if c1 == '[':
            if '0' <= c2 <= '9':
                c3 = getch()
                if not c3:
                    return c
                if c3 == '~':
                    k = {
                        '1': EditorKeys.HOME_KEY,
                        '3': EditorKeys.DEL_KEY,
                        '4': EditorKeys.END_KEY,
                        '5': EditorKeys.PAGE_UP,
                        '6': EditorKeys.PAGE_DOWN,
                        '7': EditorKeys.HOME_KEY,
                        '8': EditorKeys.END_KEY,
                    }
                    if c2 in k:
                        return k[c2]
            else:
                k = {
                    'A': EditorKeys.ARROW_UP,
                    'B': EditorKeys.ARROW_DOWN,
                    'C': EditorKeys.ARROW_RIGHT,
                    'D': EditorKeys.ARROW_LEFT,
                    'F': EditorKeys.END_KEY,
                    'H': EditorKeys.HOME_KEY,
                }

                if c2 in k:
                    return k[c2]

        elif c1 == 'O':
            if c2 == 'H':
                return EditorKeys.HOME_KEY
            elif c2 == 'F':
                return EditorKeys.END_KEY
    return c


def printf(s, color=0):
    pre = '\x1b[3%dm' % color
    suf = '\x1b[39m'
    if color:
        pprint("%s%s%s" % (pre, s, suf))
    else:
        pprint(s)


def read_key():
    global screen_rows, screen_cols, cx, cy, row_offset, file_rows, ul, quit_times
    c = raw_read()
    if c == ctrl_key('x'):
        if ul and quit_times > 0:
            c = '' if quit_times == 1 else 's'
            set_status_message(
                "Warning: File has unsaved changes. Press Ctrl-X again to quit."
            )
            quit_times -= 1
        else:
            bf_exit()
    elif c == ctrl_key('s'):
        save_file()
    elif c == ctrl_key('r'):
        run()
    elif c == ctrl_key('f'):
        search()
    elif c in ('\r', '\n'):
        insert_new_line()
    elif c in (EditorKeys.ARROW_UP, EditorKeys.ARROW_LEFT,
               EditorKeys.ARROW_RIGHT, EditorKeys.ARROW_DOWN):
        move_cursor(c)
    elif c in (EditorKeys.PAGE_UP, EditorKeys.PAGE_DOWN):
        if c == EditorKeys.PAGE_UP:
            cy = row_offset
        times = screen_rows
        while times > 0:
            move_cursor(EditorKeys.ARROW_UP if c ==
                        EditorKeys.PAGE_UP else EditorKeys.ARROW_DOWN)
            times -= 1
    elif c == EditorKeys.HOME_KEY:
        cx = 0
    elif c == EditorKeys.END_KEY:
        if cy < len(file_rows):
            cx = len(file_rows[cy])
    elif c in (EditorKeys.BACKSPACE, EditorKeys.DEL_KEY, ctrl_key('h')):
        if c == EditorKeys.DEL_KEY:
            move_cursor(EditorKeys.ARROW_RIGHT)
        delete_char()
    elif c in (ctrl_key('l'), '\x1b'):
        pass
    elif c == EditorKeys.TAB_KEY:
        reset_ul()
        for _ in range(4):
            insert_char(' ')
    else:
        reset_ul()
        printset = set(string.printable)
        if set(c).issubset(printset):
            insert_char(c)


def scroll_editor():
    global cy, row_offset, screen_rows, cx, column_offset, screen_cols
    if cy < row_offset:
        row_offset = cy
    if cy >= (row_offset + screen_rows):
        row_offset = cy - screen_rows + 1
    if cx < column_offset:
        column_offset = cx
    if cx >= (column_offset + screen_cols):
        column_offset = cx - screen_cols + 1


def refresh_screen():
    global cx, cy, row_offset, column_offset
    scroll_editor()
    pprint("\x1b[?25l\x1b[2J\x1b[H")
    draw_rows()
    draw_status_bar()
    pprint("\x1b[%d;%dH" % (cy - row_offset + 1, cx - column_offset + 1))
    pprint("\x1b[?25h")


def draw_rows():
    global screen_rows, screen_cols, file_loaded, file_rows, row_offset, column_offset, ul
    welcome_message = "BFASM STUDIO"
    help_message1 = "< Ctrl-X = Exit | Ctrl-S = Save >"
    help_message2 = "< Ctrl-F = Find | Ctrl-R = Run >"
    for row in range(screen_rows):
        file_row = row + row_offset
        if file_row < len(file_rows):
            render_row(file_rows[file_row][column_offset:column_offset +
                                           screen_cols])
        else:
            pprint("~")
        if row == screen_rows // 3 - 1 and not file_loaded and not ul:
            pad_string = " " * ((screen_cols - len(welcome_message) - 1) // 2)
            pprint(pad_string, welcome_message)
        elif row == screen_rows // 3 and not file_loaded and not ul:
            pad_string = " " * ((screen_cols - len(help_message1) - 1) // 2)
            pprint(pad_string, help_message1)
        elif row == screen_rows // 3 + 1 and not file_loaded and not ul:
            pad_string = " " * ((screen_cols - len(help_message2) - 1) // 2)
            pprint(pad_string, help_message2)
        pprint("\n")


def draw_status_bar():
    global status_message, status_message_time, screen_cols
    global file_name, screen_cols, file_rows, cy, ul, file_type
    pprint("\x1b[7m")
    s = file_name if file_name else "[No file name]"
    if (time.time() - status_message_time) < 5 and status_message:
        left = status_message
    else:
        left = "%s - %d lines %s" % (s[0:20], len(file_rows),
                                     "(modified)" if ul else "")
    right = "%d/%d" % (cy + 1, len(file_rows))
    pad = " " * (screen_cols - len(left) - len(right))
    display_string = left + pad + right
    pprint(display_string[0:screen_cols])
    pprint("\x1b[m")


def set_status_message(*args):
    global status_message, status_message_time
    if args == (None,):
        status_message = None
    else:
        status_message = " ".join(args)
    status_message_time = time.time()


def load_file(filename):
    global file_loaded, file_rows, file_name, file_type
    try:
        with open(filename, 'a+') as file:
            file.seek(0, 0)
            file_rows = convert_string_to_rows(file.read())
        file_loaded = True
        file_name = filename
        file_type = file_name.split(".")[-1]
    except IOError:
        bf_exit("Error opening %s: file doesn't exist or cannot be opened.\n" %
                filename)
    reset_ul()


def save_file():
    global file_loaded, file_rows, file_name, file_type
    if not file_name:
        file_name = prompt("Save as: %s (Ctrl-C to cancel)", None)
        if not file_name:
            set_status_message("Save aborted.")
            return
    try:
        with open(file_name, 'w+') as file:
            text = convert_rows_to_string(file_rows)
            file.write(text)
            file_loaded = True
            file_type = file_name.split(".")[-1]
            set_status_message("%d bytes written to disk." % len(text))
    except IOError as e:
        set_status_message("Error writing to %s\n - %s" % (file_name, str(e)))
    reset_ul()


def run():
    global file_name, file_rows

    pprint("\x1b[2J")
    pprint("\x1b[H")

    if file_name:
        with tempfile.NamedTemporaryFile() as f:
            subprocess.run(['bfmake', "-o", f.name, file_name])
            subprocess.run(['bfi', f.name])
    else:
        with tempfile.NamedTemporaryFile() as s:
            with tempfile.NamedTemporaryFile() as f:
                text = convert_rows_to_string(file_rows)
                s.write(text.encode())
                s.flush()
                subprocess.run(['bfmake', "-o", f.name, s.name])
                subprocess.run(['bfi', f.name])

    printf("\n ~ Press any button to return to the editor", color=4)
    getch()


def prompt(message, callback):
    buf = ""
    while True:
        set_status_message(message % buf)
        refresh_screen()
        c = raw_read()
        if c == EditorKeys.BACKSPACE:
            buf = buf[0:-1]
        elif c == ctrl_key('c'):
            set_status_message(None)
            if callback:
                callback(buf, c)
            return None
        elif c in ('\r', '\n'):
            if len(buf) != 0:
                set_status_message(None)
                if callback:
                    callback(buf, c)
                return buf
        elif type(c) != EditorKeys and not is_ctrl(c) and ord(c) < 128:
            buf += c

        if callback:
            callback(buf, c)


def render_row(row):
    global file_type
    tokens = re.split(r'([^@&%\*A-Za-z0-9_-]?)', row)
    inline = False
    escape = False
    string = False
    comment = False
    for token in tokens:
        if token == '' or token == None:
            continue
        if not inline:
            if token == syntax["asm"]["inline"]:
                printf(token, color=2)
                inline = True
            elif comment:
                printf(token, color=1)
            elif token == syntax["asm"]["comment"]:
                printf(token, color=1)
                comment = True
            elif token in syntax["asm"]["keywords"]:
                printf(token, color=5)
            elif token in syntax["asm"]["regs"]:
                printf(token, color=4)
            elif token.isdigit():
                printf(token, color=4)
            elif token[0] in syntax["asm"]["labels"]:
                printf(token, color=6)
            else:
                printf(token)
        elif file_type == "lua" or inline:
            if comment:
                printf(token, color=1)
            elif token == syntax["lua"]["comment"]:
                printf(token, color=1)
                comment = True
            elif string:
                printf(token, color=2)
                if token == '\\':
                    escape = True
                elif token == '"':
                    if escape:
                        escape = False
                    else:
                        string = False
            elif token == '"':
                printf(token, color=2)
                string = True
            elif token in syntax["lua"]["keywords"]:
                printf(token, color=5)
            elif token.isdigit():
                printf(token, color=4)
            else:
                printf(token)


def search():
    global cx, cy, row_offset, column_offset

    tcx = cx
    tcy = cy
    t_row_offset = row_offset
    t_column_offset = column_offset
    query = prompt("Search: %s (Ctrl-C to cancel)", search_callback)
    if not query:

        cx = tcx
        cy = tcy
        row_offset = t_row_offset
        column_offset = t_column_offset


def move_cursor(c):
    global cx, cy, screen_rows, screen_cols, row_offset, column_offset, file_rows

    row = None if cy >= len(file_rows) else file_rows[cy]

    if c == EditorKeys.ARROW_UP:
        if cy != 0:
            cy -= 1
    elif c == EditorKeys.ARROW_DOWN:
        if cy < len(file_rows) - 1:
            cy += 1
    elif c == EditorKeys.ARROW_LEFT:
        if cx != 0:
            cx -= 1
        elif cy > 0:
            cy -= 1
            cx = len(file_rows[cy])
    elif c == EditorKeys.ARROW_RIGHT:
        if row and cx < len(file_rows[cy]):
            cx += 1
        elif row and cy < len(file_rows):
            cy += 1
            cx = 0

    row = "" if cy >= len(file_rows) else file_rows[cy]

    if cx > len(row):
        cx = len(row)


def search_callback(query, char):
    global cx, cy, column_offset, row_offset, screen_cols, file_rows, last_match, direction
    if char in ('\r', '\n', ctrl_key('c')):
        last_match = (0, -1)
        direction = 1
        return
    elif char in (EditorKeys.ARROW_RIGHT, EditorKeys.ARROW_DOWN):
        direction = 1
    elif char in (EditorKeys.ARROW_LEFT, EditorKeys.ARROW_UP):
        direction = -1
    else:
        last_match = (0, -1)
        direction = 1

    if last_match == (0, -1):
        direction = 1
    curr = last_match[0]
    counter = 0
    while True:
        if counter == len(file_rows) - 1:
            break
        if curr == -1:
            curr = len(file_rows) - 1
        elif curr == len(file_rows):
            curr = 0

        row = file_rows[curr]
        off = 0
        if direction == 1:
            s = row[last_match[1] + 1:]
            idx = s.lower().find(query.lower())
            off = last_match[1] + 1
        else:
            s = row[0:last_match[1]]
            idx = s.lower().rfind(query.lower())
        if idx > 0:
            last_match = (curr, idx + off)
            cy = curr
            cx = last_match[1]

            if (cx - column_offset) > (screen_cols - 5):
                column_offset = cx
            row_offset = cy
            break
        else:
            curr += direction
            counter += 1
            last_match = (last_match[0], -1)


def insert_char_at_row(row, at, c):
    global file_rows, ul
    if at < 0 or at > len(file_rows[row]):
        at = len(file_rows[row])
    file_rows[row] = file_rows[row][0:at] + c + file_rows[row][at:]
    ul = True


def insert_char(c):
    global cx, cy, file_rows
    if cy == len(file_rows):
        file_rows.append("")
    insert_char_at_row(cy, cx, c)
    cx += 1


def delete_char_at_row(row, at):
    global file_rows, ul
    if at < 0 or at >= len(file_rows[row]):
        return
    file_rows[row] = file_rows[row][0:at] + file_rows[row][at + 1:]
    ul = True


def delete_char():
    global cx, cy, file_rows
    if cy == len(file_rows):
        return
    if cy == 0 and cx == 0:
        return
    if cx > 0:
        delete_char_at_row(cy, cx - 1)
        cx -= 1
    else:
        cx = len(file_rows[cy - 1])
        file_rows[cy - 1] += file_rows[cy]
        delete_row(cy)
        cy -= 1


def delete_row(at):
    global file_rows, ul
    if at < 0 or at >= len(file_rows):
        return
    file_rows = file_rows[0:at] + file_rows[at + 1:]
    ul = True


def insert_row(at, s):
    global file_rows, ul
    if at < 0 or at > len(file_rows):
        return
    file_rows = file_rows[0:at] + [s] + file_rows[at:]
    ul = True


def insert_new_line():
    global cx, cy, file_rows
    if cx == 0:
        insert_row(cy, "")
    else:
        insert_row(cy + 1, file_rows[cy][cx:])
        file_rows[cy] = file_rows[cy][0:cx]
    cx = 0
    cy += 1


if __name__ == "__main__":
    init_bfstudio()
    if len(sys.argv) >= 2:
        load_file(sys.argv[1])
    set_status_message(None)
    while True:
        refresh_screen()
        read_key()
