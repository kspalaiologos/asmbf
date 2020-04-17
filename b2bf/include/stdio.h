#ifndef _STDIO_H
#define _STDIO_H

#include <ctype.h>

puts(s) {
    asm("rcl r1, r4");
    asm("@puts");
    asm("rcl r2, r1");
    asm("jz_ r2, %puts_quit");
    asm("out r2");
    asm("inc r1");
    asm("jmp %puts");
    asm("@puts_quit");
    asm("out 10");
}

putchar(c) {
    asm("rcl r1, r4");
    asm("out r1");
}

getchar() {
    asm("in_ r1");
}

getnumb() {
    auto a, c, x, z 1, s;
    s = &s + 1;
    while (isdigit(c = getchar()) && a < 10) s[a++] = c;
    for (0; a--; z =* 10) x =+ z * (s[a] - 48);
    return x;
}

#endif
