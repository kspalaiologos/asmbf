
/**
 *  This program will double cell range (it's size in bits).
 *  In case of asm2bf, by standard, this program will extend cells
 *  to the 32-bit (4 bytes) size.
 */

#include <stdio.h>

int main(void) {
	int c;
	
	while((c=getchar()) != EOF) {
		switch(c) {
			case '>':
				#ifdef DOUBLE_NOCOPY
					puts(">>>>");
				#else
					puts(">>>");
				#endif
				break;
			case '<':
				#ifdef DOUBLE_NOCOPY
					puts("<<<<");
				#else
					puts("<<<");
				#endif
				break;
			case '+':
				#ifdef DOUBLE_NOCOPY
					puts(">+<+[>-]>[->>+<]<<");
				#else
					puts("+>+[<->[->>+<<]]>>[-<<+>>]<<<[->>+<<]");
				#endif
				break;
			case '-':
				#ifdef DOUBLE_NOCOPY
					puts(">+<[>-]>[->>-<]<<-");
				#else
					puts("+>[<->[->>+<<]]>>[-<<+>>]<<<[->>-<<]>-<");
				#endif
				break;
			case '[':
				#ifdef DOUBLE_NOCOPY
					puts(">+<[>-]>[->+>[<-]<[<]>[-<+>]]<-[+<");
				#else
					puts(">[<+>[->>+<<]]>>[-<<+>>]<[<<+>>[->+<]]>[-<+>]<<<[[-]");
				#endif
				break;
			case ']':
				#ifdef DOUBLE_NOCOPY
					puts(">+<[>-]>[->+>[<-]<[<]>[-<+>]]<-]<");
				#else
					puts(">[<+>[->>+<<]]>>[-<<+>>]<[<<+>>[->+<]]>[-<+>]<<<]");
				#endif
				break;
			default:
				putchar(c);
		}
	}
}
