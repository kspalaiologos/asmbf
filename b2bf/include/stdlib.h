#ifndef _STDLIB_H
#define _STDLIB_H

malloc(n) {
    asm("rcl r1, r4");
    asm("mov r2, $(PAGE_SIZE)");
    asm("lt_ r2, r1");
    asm("jnz r2, %malloc_fail");
    asm("\n#alloc(\"r1\", \"r2\")");
    asm("ret");
    asm("@malloc_fail");
    asm("clr r1");
}

free(p) {
    asm("rcl r1, r4");
    asm("#free(\"r1\")");
}

#define NULL 0

#endif
