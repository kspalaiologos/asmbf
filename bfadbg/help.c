
#include <ncurses.h>

#include "help.h"
#include "bfadbg.h"
#include "status.h"
#include "tui.h"

void help_pager(int page) {
    int r = -1;

again:
    switch(page) {
        case 0:
            r=msgbox(
                "0. Welcome to bfadbg!\n"
                "This interactive tutorial will teach you basic\n"
                "operations and commands available within the\n"
                "debugger. To continue, press enter and type %:help 1%."
            , "\nup"); break;
        case 1:
            r=msgbox(
                "1. Issues\n"
                "Please report issues to %Palaiologos#5430% or mail\n"
                "them to kspalaiologos<AT>gmail.com. My website:\n"
                "%kspalaiologos.now.im%. You can go to the next page\n"
                "using %:help 2%."
            , "\nup"); break;
        case 2:
            r=msgbox(
                "2. Navigation\n"
                "You can view the next page by pressing %p%, and go\n"
                "back to previous by pressing %u%."
            , "\nup"); break;
        case 3:
            r=msgbox(
                "3. Loading programs\n"
                "To load a program, use %:load [name]%. bfadbg comes\n"
                "with a simple program to test the debugger. To load\n"
                "it, use %:load hello.b%. When you're done working,\n"
                "you can %:unload% the file and %:load% another one,\n"
                "or quit the debugger by pressing %q%."
            , "\nup"); break;
        case 4:
            r=msgbox(
                "4. Interface (I)\n"
                "The bar you see on the top is the %memory view%. You\n"
                "can scroll it using the following keys:\n"
                "               %h  j  k  l%\n"
                "              %-2 -1  1  2%\n"
                "Try it out!"
            , "\nup"); break;
        case 5:
            r=msgbox(
                "5. Interface (II)\n"
                "The area on the bottom is the %status bar%. It displays\n"
                "various messages coming from the debugger. You can also\n"
                "type commands in there, by pressing %:%.\n"
                "In the default state, it displays the %position of cursor%\n"
                "into the file in percent. On the left to it, there are\n"
                "two indicators representing %memory usage%."
            , "\nup"); break;
        case 6:
            r=msgbox(
                "6. Interface (III)\n"
                "To quit command entry prompt, press %Ctrl+Q (^Q)%. To go\n"
                "back to the default message bar contents, press %v%."
            , "\nup"); break;
        case 7:
            r=msgbox(
                "7. Interface (IV)\n"
                "The panel on the right displays %register status%, i.e.\n"
                "the value of every significant cell inside the %permagen%.\n"
                "When the value changes, the register name and the new\n"
                "value is %highlighted%. The box below shows the amount of\n"
                "%cycles (time) the program already took to execute%, where\n"
                "one cycle is one instruction. Below, there is the\n"
                "brainfuck %instruction pointer% (not to be confused with\n"
                "%IP% register) and the brainfuck %memory pointer%."
            , "\nup"); break;
        case 8:
            r=msgbox(
                "8. Interface (V)\n"
                "The box below shows the brainfuck program output. By\n"
                "default, it's displayed in hexadecimal padded to two\n"
                "numbers %(%%02d)%. To change that, press %c%.\n"
                "To clear the output box, use the command %:clc%."
            , "\nup"); break;
        case 9:
            r=msgbox(
                "9. Moving around\n"
                "You can move around the file using these keys:\n"
                "                You can also navigate quickly \n"
                "          %i%     (page a time) using %n and m%.\n"
                "        %u   p%\n"
                "          %o%\n"
                "bfadbg will also highlight matching braces."
            , "\nup"); break;
        case 10:
            r=msgbox(
                "10. Running the code (I)\n"
                "To step a single brainfuck instruction, use %s%.\n"
                "To animate the execution, use %a%. To animate it\n"
                "quicker, use %z%. To run the program, use %f%.\n"
                "You can toggle a %breakpoint% using %b%. The\n"
                "program will unconditionally stop before bumping\n"
                "into one. You can break out of %a, z and f%\n"
                "by striking any key."
            , "\nup"); break;
        case 11:
            r=msgbox(
                "11. Conditional breakpoints (I)\n"
                "A %conditional breakpoint has no location% and\n"
                "fires only if a %condition% is satisfied. To open\n"
                "the breakpoint adding window, press %d%.\n"
                "You can quit it with %q%, select options with\n"
                "%space% and write text into fields marked with a\n"
                "dot. To switch fields, use %tab% For example, to\n"
                "%break if R2=5%, select %[Reg]%, then type %r5% into\n"
                "the box, then select %Eq%, then select %[Imm]%, then\n"
                "write %5% in the box to the right and finally press\n"
                "%enter% twice."
            , "\nup"); break;
        case 12:
            r=msgbox(
                "12. Conditional breakpoints (II)\n"
                "To list all conditional breakpoints previously\n"
                "defined, press %y%. You can move around this\n"
                "window using %i% (up) and %o% (down). You can\n"
                "%delete% a breakpoint by striking enter."
            , "\nup"); break;
        case 13:
            r=msgbox(
                "13. MISC (I)\n"
                "The brainfuck interpreter allocates memory\n"
                "in %1K% blocks, with %65K% initially. When an\n"
                "%access violation% occurs, the interpreter\n"
                "has tried to read too much to the left. This\n"
                "can be a sign of a %corrupted permagen, or\n"
                "a stack underflow%."
            , "\nup"); break;
        case 14:
            r=msgbox(
                "14. MISC (II)\n"
                "bfadbg will mark the end of file with %#%.\n"
                "You can clear the memory using %:mcl%.\n"
                "You can use %:clb% to remove all breakpoints.\n"
                "To edit memory, you can navigate to the desired\n"
                "cell using the top bar, then press %r%. You\n"
                "should see %M(x)%. There you can type, for\n"
                "example, %+=5% or %=10% (decimal). These do\n"
                "what you'd expect them to."
            , "\nup"); break;
        case 15:
            r=msgbox(
                "15. MISC (III)\n"
                "Use %:rc% to clear the cycle counter.\n"
                "By default, the interpreter will break upon\n"
                "bumping on %#% in the source code, but you can\n"
                "toggle that using %:tgh%."
            , "\nup"); break;
        case 16:
            r=msgbox(
                "16. Editing (I)\n"
                "Press %g% and a brainfuck instruction key\n"
                "(%+-<>.,[]%) to replace the instruction under\n"
                "the cursor. You can use %w% and a brainfuck\n"
                "instruction to insert it at the cursor\n"
                "position."
            , "\nup"); break;
        case 17:
            r=msgbox(
                "17. Running the code (II)\n"
                "You can modify the interpreter state by\n"
                "pressing %e%. A window will open where\n"
                "you can modify the %ip%, %mp% (note: can't\n"
                "move the %mp% outside the %maxmp% area),\n"
                "and the input queue (fetched by program\n"
                "using the %,% instruction)."
            , "\nup"); break;
        case 18:
            r=msgbox(
                "18. Editing (II).\n"
                "Press %x% to remove an instruction under the \n"
                "cursor. Use %:cp [n]% to purge %[n]% characters from\n"
                "the code. Use %:cr [str]% to start overwriting the\n"
                "code under the cursor with %[str]%. %:ci [str]% will\n"
                "do the same, but insert the code instead of\n"
                "replacing it."
            , "\nup"); break;
            break;
        case 19:
            r=msgbox(
                "19. The End.\n"
                "Happy Debugging! The source code will be\n"
                "released eventually.\n\n"
                "%(C) Kamila Szewczyk, 2020.%"
            , "\nup"); break;
        default:
            if(r == -1) {
                broadcast_status("No such help page.");
                r=10;
            } else {
                if(page < 0) {
                    page = 0;
                    beep();
                    goto again;
                }

                r = 10;
            }
    }

    if(r == 'p') {
        page++;
        repaint_all(1, 1);
        goto again;
    } else if(r == 'u') {
        page--;
        repaint_all(1, 1);
        goto again;
    }
}
