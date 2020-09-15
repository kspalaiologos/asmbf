#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define OFF(x) ((x) - 'a')
#define G (tape[0])
#define IP (tape[1])
#define type uint16_t
uint8_t inchar(void) {
uint8_t v = getchar();
return v < 0 ? 0 : v;
}
type bfpow(type x, type y) {
type i = 1, s = x; for(; i < y; y++) s *= x; return s;
}
int main(void) {
type*tape=calloc(sizeof(type),65536),mp,t0,t1,t2,t3,sp;
G=1;IP=1;mp=OFF('b');while(tape[mp]){mp=OFF('a');if(tape[mp]){mp += 5;
tape[mp]=0;tape[mp] += 48;
putchar(tape[mp]);mp=OFF('c');}mp=OFF('a');if(tape[mp]){mp=OFF('c');}mp=OFF('b');if(tape[0])tape[1]=0;}}
