
/* asm2bf
 *
 * Copyright (C) K. Palaiologos Szewczyk, 2017-2020.
 * License: MIT
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * --------------------------------------------------------------------------------------------------------------------------
 * A VERY VERY IMPORTANT NOTE! THIS FILE MAY SEEM OBFUSCATED OR OVERALL UNREADABLE. HERE'S WHY.
 * Back in the day when I started the asm2bf project (early 2017), I thought that I'd write this reference implementation,
 * and then write an asm2bf version of it, so that my self compiler actually runs the project.
 * Later on, I thought that I'll rather write asm2bf in C, so I froze the asm2bf self compiler in v0.9 and started to tweak
 * this program, that was meant to be the reference implementation.
 * There's no need for running away in terror, as the most of the code is just parsing the input and outputting the premade
 * instruction microcode in the `s' string, containing instruction names and the microcode.
 */

#include <stdio.h>
#include <stdlib.h>

#define IC 118

/* db command location: instruction count - 4 */
#define C1 (IC-4)

/* total mnemonic length: instruction count * 3 */
#define C2 (IC*3)

/* txt command location: instruction count - 3 */
#define C3 (IC-3)

/* location of various instructions */
#define RSE (IC-7)
#define STK (IC-6)
#define ORG (IC-5)
#define DB (IC-4)
#define RAW (IC-2)
#define SEG (IC-1)

#define STACK 23

/* no idea what this is */
#define C4 ((IC+7)*3)

unsigned int inchar();
void outbf();
void outrep();
static unsigned int m[10000], off, freecell, rseg;

int best_base(int n);
void translate(int n, int base);

#ifdef BFVM
    #define RLE
#endif

#ifndef BFASM_NO_EXPORT_MAIN
int main(void) {
#else
int bfasm(void) {
#endif
    unsigned int n;
    char * s = "addanddecdiveq_ge_gt_in_incjmpjnzjz_lblle_lt_modmovmulne_negnotor_outpoppshrclstosubswpclrretendlogaslasrpowsrvampsmpnavceqcneclecltcgecgtcjncjzcadcsucmucdicmdcslcsrcpwcpscpocswcrvcmocrccstcamcsmx00x01x02x03x04shrshlcoucincpapargcdcgccrefmufdifrefadfsudupcdp"
    "x05x06x07x08x09x0ax0bx0cx0dx0ex0fx10x11x12x13x14x15x16otscotdscsgtsptsletps"
    "rsestkorgdb_txtrawseg"
           #ifndef BFVM
           "a+b+[<[>-]>[>]<\0" /* 0 first */
           "b]\0" /* 1 last (end, post, last) */
           "a[c+a-]c[-a+\0" /* 2 pre */
           "c]\0" /* 3 post */
           "2\0" /* 4 immed */
           "2[-]\0" /* 5 immed clear */
           "2[1+e+2-]e[2+e-]\0" /* 6 add */
           "1[e+1[-]]e[-2[1+2[e+2-]]e[2+e-]]\0" /* and */
           "2-\0" /* dec */
           "2[n+2-]1[m+>-[>>]<[[>+<-]<+>>>]<<1-]l[1+l-]m[2+m-]n[2+n-]\0" /* div */
           "1[2-e+1-]+2[1-2[e+2-]]e[2+e-]\0" /* eq_, alternatively: 2[1-e+2-]e[2+e-]1[e+1[-]]+e[1-e-] */
           "1[n+1-]+2[m+2-]m[2+m>[-<]>]<<[1-m-[2+m-]<<]>>n[-]\0" /* ge_ */
           "1[m+1-]2[n+2-]m[2+m>[-<]>]<<[[-]1+2-m<<]>>n[2+n-]\0" /* gt_ */
           "2,\0" /* in_ */
           "2+\0" /* inc */
           "a[-]b[-]2[b+c+2-]c[2+c-]\0" /* jmp addr */
           "1[a[-]2[b+c+2-]c[2+c-]1[c+1-]]c[1+c-]\0" /* jnz val, addr */
           "1[d-1[c+1-]]c[1+c-]d+[-a[-]2[b+c+2-]c[2+c-]d]\0" /* jz_ val, addr */
           "a[e[-]c-a-]+c+[-b[c+e-b-]e[a-c[b+e+c-]e[-]]c[-]]\0" /* lbl */
           "1[m+1-]+2[n+2-]m[2+m>[-<]>]<<[[-]1-2-m<<]>>n[2+n-]\0" /* le_ */
           "1[n+1-]2[m+2-]m[2+m>[-<]>]<<[1-m-[2+m-]<<]>>n[-]\0" /* lt_ */
           "2[n+2-]1[m+>-[<<]<[[>+<-]<<]>>1-]m[1+2+m-]n[2+n-]\0" /* mod */
           "1[-]2[1+e+2-]e[2+e-]\0" /* 22 mov */
           "1[d+1-]d[2[1+e+2-]e[2+e-]d-]\0" /* mul */
           "1[2-c+1-]2[1+2[c+2-]]c[2+c-]\0" /* ne_, alternatively: 2[1-e+2-]e[2+e-]1[e+1[-]]e[1+e-] */
           "2[e-2-]e[2+e-]\0" /* neg */
           "2-[e-2-]e[2+e-]\0" /* not */
           "1[e-1[-]]+e+[-1-2[1+[e+2-]]e[2+e-]]\0" /* or_ */
           "2.\0" /* out */
           "2[-]#>[>>]<<->[<<<[<<]>2+#>[>>]>-]<<<[<<]>\0" /* pop */
           "2[p+#>[>>]>+<<<[<<]>2-]p[2+p-]#>[>>]+[<<]>\0" /* psh */
           "1[-]2[p+*>[>>]+[<<]>2-]p[2+p-]*>[>>]>[<<<[<<]>1+*>[>>]>>+<-]>[<+>-]<<<<[-<<]>\0"  /* rcl */
           "1[p+*>[>>]+[<<]>1-]p[1+p-]*>[>>]>[-]<<<[<<]>2[p+*>[>>]>+<<<[<<]>2-]p[2+p-]*>[>>]<<[-<<]>\0" /* sto */
           "2[1-e+2-]e[2+e-]\0" /* sub */
           "1[e+1-]2[1+2-]e[2+e-]\0" /* swp */
           "2[-]\0" /* clr */
           "a[-]b[-]#>[>>]<<->[<<<[<<]>b+#>[>>]>-]<<<[<<]>\0" /* ret */
           "a[-]b[-]\0" /* end */
           "2[e+2[-]]e[2+e-]\0" /* log */
           "2[e+2-]e[2++e-]\0" /* asl */
           "2[m+2-]k+m[-[-2+m>]<]<[>]>k-\0" /* asr */
           "1[k+1-]+2[c+1[k[l+m+k-]l[k+l-]1-]m[1+m-]2-]c[2+c-]k[-]\0" /* pow */
           "#>[>>]<[<+>-]<<[>>+<<-]>-[<+>-]+[<<]>\0" /* srv */
           "1[e+*>[>>]+[<<]>1-]e[1+e-]2[e+*>[>>]>+<<<[<<]>2-]e[2+e-]*>[>>]<<[-<<]>\0" /* amp */
           "1[e+*>[>>]+[<<]>1-]e[1+e-]2[e+*>[>>]>-<<<[<<]>2-]e[2+e-]*>[>>]<<[-<<]>\0" /* smp */
           "2\0" /* nav */ 
           "q[-]+2[1-e+2-]1[q-1[2-e+1-]]e[2+1+e-]\0" /* ceq v */
           "q[-]1[2-e+1-]2[q+2[1-e+2-]]e[1+2+e-]\0" /* cne v */
           "q[-]+1[m+1-]2[n+2-]m[1+2+m>[-<]>]<<[q-2-m-[1+m-]<<]>>n[2+n-]\0" /* cle v */
           "q[-]1[n+1-]2[m+2-]m[1+2+m>[-<]>]<<[q+1-m-[2+m-]<<]>>n[1+n-]\0" /* clt */
           "q[-]+1[n+1-]2[m+2-]m[1+2+m>[-<]>]<<[q-1-m-[2+m-]<<]>>n[1+n-]\0" /* cge v */
           "q[-]1[m+1-]2[n+2-]m[1+2+m>[-<]>]<<[q+2-m-[1+m-]<<]>>n[2+n-]\0" /* cgt v */
	   #else
           "AAAb\0" /* 0 first */
           "ABb\0" /* 1 last (end, post, last) */
           "ACa\0" /* 2 pre */
           "ADc\0" /* 3 post */
           "AE2Z\0" /* 4 immed */
           "AF2Z\0" /* 5 immed clear */
           "AG2Z1Z\0" /* 6 add */
           "AH2Z1Z\0" /* and */
           "AI2Z\0" /* dec */
           "AJ2Z1Z\0" /* div */
           "AK2Z1Z\0" /* eq_ */
           "AL2Z1Z\0" /* ge_ */
           "AM2Z1Z\0" /* gt_ */
           "AN2Z\0" /* in_ */
           "BL2Z\0" /* inc */
           "AO2Z\0" /* jmp addr */
           "AP1Z2Z\0" /* jnz val, addr */
           "AQ1Z2Z\0" /* jz_ val, addr */
           "AR\0" /* lbl */
           "AS2Z1Z\0" /* le_ */
           "AT2Z1Z\0" /* lt_ */
           "AU2Z1Z\0" /* mod */
           "AV2Z1Z\0" /* 22 mov */
           "AW2Z1Z\0" /* mul */
           "AX2Z1Z\0" /* ne_ */
           "AY2Z\0" /* neg */
           "AZ2Z\0" /* not */
           "BA2Z1Z\0" /* or_ */
           "BB2Z\0" /* out */
           "BC2Z#Z\0" /* pop */
           "BD2Z#Z\0" /* psh */
           "BE1Z2Z*Z\0" /* rcl */
           "BF1Z2Z*Z\0" /* sto */
           "BG2Z1Z\0" /* sub */
           "BH1Z2Z\0" /* swp */
           "BI2Z\0" /* clr */
           "BJ#Z\0" /* ret */
           "BKb\0" /* end */
           "BM2Z\0" /* log */
           "BN2Z\0" /* asl */
           "BO2Z\0" /* asr */
           "BP2Z1Z\0" /* pow */
           "BR#Z\0" /* srv */
           "BS1Z2Z*Z\0" /* amp */
           "BT1Z2Z*Z\0" /* smp */
           "BU2Z\0" /* nav */
	   "BV1Z2ZqZ\0" /* ceq */
	   "BW1Z2ZqZ\0" /* cne */
           "BX1Z2ZqZ\0" /* cle */
	   "BY1Z2ZqZ\0" /* clt */
	   "BZ1Z2ZqZ\0" /* cge */
	   "CA1Z2ZqZ\0" /* cgt */
	   #endif
           /* c?? instructions go here. don't even try to understand that. */
           "q[c+d+q-]c[q+c-]d[a[-]b[-]2[b+c+2-]c[2+c-]d[-]]\0" /* cjn v */
           "p+>[<-]<[-a[-]b[-]2[b+c+2-]c[2+c-]p<]>\0" /* cjz v */
           "q[2[1+e+2-]e[2+e-]p<+>]<[->]\0" /* cad v */
           "q[2[1-e+2-]e[2+e-]p<+>]<[->]\0" /* csu v */
           "q[1[d+1-]d[2[1+e+2-]e[2+e-]d-]p<+>]<[->]\0" /* cmu */
           "q[2[n+2-]1[m+>-[>>]<[[>+<-]<+>>>]<<1-]l[1+l-]m[2+m-]n[2+n-]p<+>]<[->]\0" /* cdi */
           "q[2[n+2-]1[m+>-[<<]<[[>+<-]<<]>>1-]m[1+2+m-]n[2+n-]p<+>]<[->]\0" /* cmd */
           "q[2[e+2-]e[2++e-]p<+>]<[->]\0" /* csl v */
           "q[2[m+2-]k+m[-[-2+m>]<]<[>]>k-p<+>]<[->]\0" /* csr v */
           "q[1[k+1-]+2[c+1[k[l+m+k-]l[k+l-]1-]m[1+m-]2-]c[2+c-]k[-]p<+>]<[->]\0" /* cpw v */
           "q[2[p+#>[>>]>+<<<[<<]>2-]p[2+p-]#>[>>]+[<<]>p<+>]<[->]\0" /* cps v */
           "q[2[-]#>[>>]<<->[<<<[<<]>2+#>[>>]>-]<<<[<<]>p<+>]<[->]\0" /* cpo v */
           "q[1[e+1-]2[1+2-]e[2+e-]p<+>]<[->]\0" /* csw v */
           "q[#>[>>]<[<+>-]<<[>>+<<-]>-[<+>-]+[<<]>p<+>]<[->]\0" /* crv v */
           "q[1[-]2[1+e+2-]e[2+e-]p<+>]<[->]\0" /* cmo v */
           "q[1[-]2[p+*>[>>]+[<<]>2-]p[2+p-]*>[>>]>[<<<[<<]>1+*>[>>]>>+<-]>[<+>-]<<<<[-<<]>p<+>]<[->]\0" /* crc v */
           "q[1[p+*>[>>]+[<<]>1-]p[1+p-]*>[>>]>[-]<<<[<<]>2[p+*>[>>]>+<<<[<<]>2-]p[2+p-]*>[>>]<<[-<<]>p<+>]<[->]\0" /* cst v */
           "q[1[e+*>[>>]+[<<]>1-]e[1+e-]2[e+*>[>>]>+<<<[<<]>2-]e[2+e-]*>[>>]<<[-<<]>p<+>]<[->]\0" /* cam */
           "q[1[e+*>[>>]+[<<]>1-]e[1+e-]2[e+*>[>>]>-<<<[<<]>2-]e[2+e-]*>[>>]<<[-<<]>p<+>]<[->]\0" /* csm */
           "1[l+1-]2[1+n+2-]1[2+1-]e+l[>>[-[->>+<]<]<[>>>+<<]<[-[->>+<]<]>>[>-<<]>>[+[--<<<e[d+1+e-]o]]p[l+p-]e[<+>-]<[>++<-]>l]n[-]e[-]\0" /* x00 */
           "1[l+1-]2[1+n+2-]1[2+1-]e+l[>>[-[->>+<]<]<[>>>+<<]<[-[->>+<]<]>>[>-<<]>>+[<<<e[d+1+e-]o[-]]p[l+p-]e[<+>-]<[>++<-]>l]n[e[d+1+e-]d[>+<-]n-]e[-]\0" /* x01 */
           "1[l+1-]2[1+n+2-]1[2+1-]e+l[>>[-[->>+<]<]<[>>>-<<]<[-[->>+<]<]>>[>-<<]>>+[<<<e[d+1+e-]o+[-]]p[l+p-]e[<+>-]<[>++<-]>l]n[e[d+1+e-]d[>+<-]n-]e[-]\0" /* x02 */
           "2[d+2-]-d[2-d-]\0" /* x03 */
           "p+>-[++<-]<[-<]>\0" /* x04 */
           "k+2[e+1[m+1-]m[-[-1+m>]<]<[>]>2-]e[2+e-]k-\0" /* shr */
           "2[d+2-]d[2+1[e+1-]e[1++e-]d-]\0" /* shl */
           "q[2.p<+>]<[->]\0" /* cou */
           "q[2,p<+>]<[->]\0" /* cin */
           "2[m+2-]k+q[-]m[-[-2++m>]<]<[>>2+q+m<]>k-\0" /* cpa */
           "2[m+2-]k+1[-]m[-[-2++m>]<]<[>>2+1+m<]>k-\0" /* par */
           "1[k+1-]2[l+m+2-]m[2+m-]l[<[>>+<-[>>]>[[<+>-]>>]<<<<-]>[<+>-]>[<<+>+>-]<]k[1+k-]\0" /* gcd */
           "q[1[k+1-]2[l+m+2-]m[2+m-]l[<[>>+<-[>>]>[[<+>-]>>]<<<<-]>[<+>-]>[<<+>+>-]<]k[1+k-]p<+>]<[->]\0" /* cgc */
           "q[a[-]b[-]#>[>>]<<->[<<<[<<]>b+#>[>>]>-]<<<[<<]>p<+>]<[->]\0" /* cre */
           "#>[>>]<<[-<<<[<+>-]<-[>>>>>[<<<<+>>>+>-]<[>+<-]<<<<-]>>>>>[-]<<<]+<<+[<<]>\0" /* fmul */
           "#>[>>]<<-<<-<<-<[>+<-]>>[>>[<<<<+>>>+>-]<[>+<-]<-]>>[-]>>[<<<<<[>+>+<<-]>>[<<+>>-]>>>-]<<<<<[-]+[<<]>\0" /* fdiv */
           "#>[>>]<<-<[<+>>+<-]>>[<<+>>>+<-]>[<<[>+>-[>>]<[[>+<-]>>]<<<-]>>[<<+>>-]<[<+>>+<-]>]<<<[>[<->>>+<<-]>+>[<<+>>-]<<<]<-[>+>[<<->>>>+<<-]>>[<<+>>-]<<<<]+>>[-]+[<<]>\0" /* freduce */
           "#>[>>]<<-<[<<<<->+>>>-]<-<<-<[>[<+>>>>+<<<-]>[<+>-]<[>>>[<<+>+>-]<[>+<-]<<-]<[>+<-]>>>>>>[<<<<<[<+>>+<-]<[>+<-]>>>>>>-]<<[<<<[<+>>>+<<-]>>[<<+>>-]>-]]<[>>[<+>-]>>>>>[<<<<+>>>>-]<<<]<<[-]+[<<]>\0" /* fadd */
           "#>[>>]<<-<[<<<<->+>>>-]<-<<-<[>[<+>>>>+<<<-]>[<+>-]<[>>>[<<->+>-]<[>+<-]<<-]<[>+<-]>>>>>>[<<<<<[<+>>+<-]<[>+<-]>>>>>>-]<<[<<<[<+>>>+<<-]>>[<<+>>-]>-]]<[>>[<+>-]>[>>>>-<<<<-]>>>>[<<<<+>>>>-]<<<]<<[-]+[<<]>\0" /* fsub */
           "#>[>>]<[->+>+<<]>[-<+>]+[<<]>\0" /* dup */
           "q[#>[>>]<[->+>+<<]>[-<+>]+[<<]>p<+>]<[->]\0" /* cdup (cdp) */
           "q[2[1-k+2-]1[q-k[1+2+k-]1[l+1-]]l[1+l-]k[1+2+k-]p<+>]<[->]\0" /* candeq x05 */
           "q[-2[1-k+2-]1[q+k[1+2+k-]1[l+1-]]l[1+l-]k[1+2+k-]p<+>]<[->]\0" /* candne x06 */
           "q[1[m+1-]2[n+2-]m[1+2+m>[-<]>]<<[q-2-m-[1+m-]<<]>>n[2+n-]p<+>]<[->]\0" /* candle x07 */
           "q[1[n+1-]2[m+2-]m[1+2+m>[-<]>]<<[q-1-m-[2+m-]<<]>>n[1+n-]p<+>]<[->]\0" /* candge x08 */
           "q[-1[n+1-]2[m+2-]m[1+2+m>[-<]>]<<[q+1-m-[2+m-]<<]>>n[1+n-]p<+>]<[->]\0" /* candlt x09 */
           "q[-1[m+1-]2[n+2-]m[1+2+m>[-<]>]<<[q+2-m-[1+m-]<<]>>n[2+n-]p<+>]<[->]\0" /* candgt x0A */
           "p+>[<-]<[-q+2[1-k+2-]1[q-k[1+2+k-]1[l+1-]]l[1+l-]k[1+2+k-]p<]>\0" /* coreq x0B */
           "p+>[<-]<[-2[1-k+2-]1[q+k[1+2+k-]1[l+1-]]l[1+l-]k[1+2+k-]p<]>\0" /* corne x0C */
           "p+>[<-]<[-q+1[m+1-]2[n+2-]m[1+2+m>[-<]>]<<[q-2-m-[1+m-]<<]>>n[2+n-]p<]>\0" /* corle x0D */
           "p+>[<-]<[-q+1[n+1-]2[m+2-]m[1+2+m>[-<]>]<<[q-1-m-[2+m-]<<]>>n[1+n-]p<]>\0" /* corge x0E */
           "p+>[<-]<[-1[n+1-]2[m+2-]m[1+2+m>[-<]>]<<[q+1-m-[2+m-]<<]>>n[1+n-]p<]>\0" /* corlt x0F */
           "p+>[<-]<[-1[m+1-]2[n+2-]m[1+2+m>[-<]>]<<[q+2-m-[1+m-]<<]>>n[2+n-]p<]>\0" /* corgt x10 */
           "2[1-k+2-]1[q+k[1+2+k-]1[l+1-]]l[1+l-]k[1+2+k-]q-[+[-]+<<+>]<[->]>\0" /* cxoreq x11 */
           "2[1-k+2-]1[q-k[1+2+k-]1[l+1-]]l[1+l-]k[1+2+k-]q[+[-]+<<+>]<[->]>\0" /* cxorne x12 */
           "1[m+1-]2[n+2-]m[1+2+m>[-<]>]<<[q+2-m-[1+m-]<<]>>n[2+n-]q-[+[-]+<<+>]<[->]>\0" /* cxorle x13 */
           "1[n+1-]2[m+2-]m[1+2+m>[-<]>]<<[q+1-m-[2+m-]<<]>>n[1+n-]q-[+[-]+<<+>]<[->]>\0" /* cxorge x14 */
           "1[n+1-]2[m+2-]m[1+2+m>[-<]>]<<[q-1-m-[2+m-]<<]>>n[1+n-]q[+[-]+<<+>]<[->]>\0" /* cxorlt x15 */
           "1[m+1-]2[n+2-]m[1+2+m>[-<]>]<<[q-2-m-[1+m-]<<]>>n[2+n-]q[+[-]+<<+>]<[->]>\0" /* cxorgt x16 */
           "2[p+*>[>>]+[<<]>2-]p[2+p-]*>[>>]>[-]<<<[<<]>1[p+*>[>>]>+<<<[<<]>1-]p[1+p-]*>[>>]<<[-<<]>\0" /* ots: sto with reversed parameters */
           "q[2[p+*>[>>]+[<<]>2-]p[2+p-]*>[>>]>[-]<<<[<<]>1[p+*>[>>]>+<<<[<<]>1-]p[1+p-]*>[>>]<<[-<<]>p<+>]<[->]\0" /* cots => cot */
           "#>[>>]<[-]<[-]<<[<<]>\0" /* dsc */
           "1[-]2-[p+#>[>>]+<<-<<[<<]>2-]+p[2+p-]#>[>>]<[<+>-]<-[[<<]>1+#>[>>]<+<-]>>+[-<<+>>>>]<<<<[<<]>\0" /* sgt */
           "2-[p+#>[>>]+<<-<<[<<]>2-]+p[2+p-]#>[>>]<[-]<[<<]>1[p+#>[>>]<+<[<<]>1-]p[1+p-]#>[>>]>>[-<<+>>>>]<<<<[<<]>\0" /* spt */
           "2[-]#>[>>]<<<<[>>[<<+>>-]+<<<<]>>[<2+#>-]+<\0" /* sle */
           "1-[p+#>[>>]+<<-<<[<<]>1-]+p[1+p-]#>[>>]<[-]<[<<]>2[p+#>[>>]<+<[<<]>2-]p[2+p-]#>[>>]>>[-<<+>>>>]<<<<[<<]>\0" /* tps */
           ;
    for (n = 0; n < 8000; n++)  m[n + 20] = s[n];
    m[6] = 0;
    m[8] = 0;
    m[9] = STACK;
    m[10] = m[9] + 2;
    outbf();
    m[11] = 1;
    m[12] = 0;
    m[1] = 1;
Laa:;
    m[0] = inchar();
    if (m[1] == 1) m[0] = (m[0] >= 'A' && m[0] <= 'Z') ? (m[0]) - 'A' + 'a' : m[0];
Laj:;
    if (m[0] != '\0') goto Lam;
    if (m[1] == 2) goto Lai;
    goto Lab;
Lam:;
    if (m[0] != '\n' && m[0] != '\v' && m[0] != '\f' && m[0] != '\r') goto Lad;
    if (m[1] == 2) goto Lai;
    m[1] = 1;
    goto Laa;
Lad:;
    if (m[1] == 4) {
        if (m[0] == '\"') {
            m[1] = 1;
            goto Laa;
        }
        m[2] = C1;
        m[3] = m[0];
        goto Lai;
    }
    if (m[0] == ' ' || m[0] == '\t' || m[1] == 0) goto Laa;
    if (m[1] != 3) goto Lay;
    
    if (m[0] != '\"') {
#ifndef BFASM_NO_ERROR_CODES
        printf("\n** ERROR: Closing quote expected.\n");
#endif
        goto Laz;
    }
    
    m[1] = 4;
    goto Laa;
Lay:;
    if (m[0] != ';') goto Lac; /* comment */
    if (m[1] == 2) goto Lai;
    m[1] = 0;
    goto Laa;
Lac:;
    if (m[1] != 1) goto Lae; /* get 3 letter instruction */
    m[2] = inchar();
    m[3] = inchar();
    m[2] = (m[2] >= 'A' && m[2] <= 'Z') ? (m[2]) - 'A' + 'a' : m[2];
    m[3] = (m[3] >= 'A' && m[3] <= 'Z') ? (m[3]) - 'A' + 'a' : m[3];
    if(m[3] == ' ') m[3] = '_';
    m[4] = 0;
Laf:;
    m[5] = m[4] + 20;
    m[6] = m[m[5]];
    if (m[0] != m[6]) goto Lag;
    m[5]++;
    m[6] = m[m[5]];
    if (m[2] != m[6]) goto Lag;
    m[5]++;
    m[6] = m[m[5]];
    if (m[3] == m[6]) goto Lah;
Lag:;
    m[4]++;
    m[4]++;
    m[4]++;
#ifndef BFASM_NO_ERROR_CODES
    if (m[4] == C2) { fprintf(stderr, "\n** ERROR: No such instruction: %c%c%c\n", m[0], m[2], m[3]); goto Laz; } /* not found, quit */
#else
    if (m[4] == C2) goto Laz; /* not found, quit */
#endif
    goto Laf;
Lah:;
    m[1] = 2;
    m[2] = m[4] / 3;
    m[3] = 0;
    m[4] = 0;
    m[5] = 0;
    goto Laa;
Lae:; /* find operands */
    if (m[2] != C3) goto Lax; /* txt command */
    m[1] = 3;
    goto Laj;
Lax:;
    if(m[0] == 'f' || m[0] == 'F') {
        m[0] = inchar();
        m[0] = m[0] - '1';
#ifndef BFASM_NO_ERROR_CODES
        if (m[0] > 2) { printf("\n** ERROR: Register unavailable: f%d\n", m[0] + 1); goto Laz; }
#else
        if (m[0] > 2) goto Laz;
#endif
        m[4] = m[0] == 0 ? 'q' : (m[0] == 1 ? 'v' : 't');
        goto Laa;
    }
    
    if (m[0] != 'r' && m[0] != 'R') goto Lak; /* r_ operand */
    m[0] = inchar();
    m[0] = m[0] - '1';
#ifndef BFASM_NO_ERROR_CODES
    if (m[0] > 5) { printf("\n** ERROR: Register unavailable: r%d\n", m[0] + 1); goto Laz; }
#else
    if (m[0] > 5) goto Laz;
#endif
    m[4] = m[0] >= 4 ? (m[0] - 4 + 'r') : (m[0] + 'f');
    goto Laa;
Lak:;
    if (m[0] != ',') goto Lan; /* , */
    m[5] = m[4];
    m[4] = 0;
    goto Laa;
Lan:;
    if (m[0] != '.') goto Lal; /* .char */
    m[3] = inchar();
    goto Laa;
Lal:; /* number */
    m[0] = m[0] - '0';
#ifndef BFASM_NO_ERROR_CODES
    if (m[0] > 9) { printf("\n** ERROR: Expected digit, got `%c'\n", m[0] + '0'); goto Laz; }
#else
    if (m[0] > 9) goto Laz;
#endif
    m[3] = m[3] * 10;
    m[3] = m[3] + m[0];
    goto Laa;
Lai:;
    if (m[4] && m[4] == m[5]) {
        m[6] = 22;
        m[5] = 'j';
        outbf();
        m[5] = m[4];
        m[4] = 'j';
    }
    if (m[11] == 1 && m[2] != 12) {
        m[6] = 2;
        outbf(); /* pre */
        m[11] = 0;
    }
    switch (m[2]) {
        case 0: /* add */
            if (m[4] == 0) {
            m[6] = 4;
            m[4] = m[5];
            outbf();
            m[6] = '+';
            outrep();
            goto Lap;
            }
            goto Lao;
        case 9: /* jmp */
            m[11] = 1;
            m[12] = 1;
            goto Lao;
        case 10: case 46: /* jnz */
            m[11] = 1;
            m[12] = 1;
            goto Lao;
        case 11: case 47: /* jz_ */
            m[11] = 1;
            m[12] = 1;
            goto Lao;
        case 12: /* lbl */
            if (m[11] == 0) {
                m[6] = 3;
                outbf(); /* post */
            }
            
            if(m[3] == 0) {
#ifndef BFASM_NO_ERROR_CODES
                fprintf(stderr, "\n** WARNING: Everytime you use `lbl 0', a neko girl dies. "
                                "You shouldn't be using `lbl' anyway, asm2bf is bundled with "
                                "a label preprocessor for a reason\n");
#endif
            }
#ifndef BFVM
            m[11] = 1;
            m[6] = 4;
            m[4] = 'e';
            outbf();
            m[6] = '+';
            outrep(); // note: potential optimalization, << & < free
            m[6] = 18; // ???
            outbf();
#else
            putchar('A');
            putchar('R');
            m[11] = 1;
            m[6] = 4;
            m[4] = 'e';
            outbf();
            m[6] = '+';
            outrep(); // note: potential optimalization, << & < free
            m[6] = 18; // ???
            outbf();
#endif
            goto Lap;
        case 16: /* mov */
            if (m[4] == 0) {
            m[6] = 5;
            m[4] = m[5];
            outbf();
            m[6] = '+';
            outrep();
            goto Lap;
            }
            goto Lao;
        case 27: /* sub */
            if (m[4] == 0) {
                m[6] = 4;
                m[4] = m[5];
                outbf();
                m[6] = '-';
                outrep();
                goto Lap;
            }
            goto Lao;
        case 30: /* ret */
            m[11] = 1;
            m[12] = 1;
            goto Lao;
        case 31: /* end */
            m[11] = 1;
            m[12] = 1;
            goto Lao;
        case STK: /* stk */
            m[9] = m[3] * 2 + STACK; // ???
            goto Lap;
        case ORG: /* org */
            m[10] = m[3] * 2 + m[9] + 2;
            goto Lap;
        case DB: /* db_ */
            if(m[3] == '\\' && m[1] == 4) {
                m[3] = getchar();
                
                switch(m[3]) {
                    case 'n': m[3] = '\n'; break;
                    case 'f': m[3] = '\f'; break;
                    case 'r': m[3] = '\r'; break;
                    case '0': m[3] = '\0'; break;
                    case '\\': m[3] = '\\'; break;
                    default:
#ifndef BFASM_NO_ERROR_CODES
                        fprintf(stderr, "\n** ERROR: No such escape sequence: \\%c\n", m[3]);
#endif
                        goto Laz;
                }
            }
            
            m[6] = 4;
            m[4] = '^';
            outbf();
            m[6] = '+';
            freecell = 1;
            outrep();
            m[10] = m[10] + 2;
            if (m[1] == 4) goto Laa;
            goto Lap;
        case RAW: /* raw */
            putchar(m[3]);
            goto Lap;
        case SEG: /* seg */
            off=m[3];
            m[10]=m[9] + 2;
            goto Lap;
        case RSE:
            #ifdef BFVM
                rseg = !rseg;
            #else
                fprintf(stderr, " *** WARNING: Brainfuck target; `rse' ignored.\n");
            #endif
            goto Lap;
    }
Lao:;
    if (m[4] == 0) {
        m[6] = 4; /* immed */
        m[4] = 'j';
        outbf();
        m[6] = '+';
        freecell = 1;
        outrep();
        m[3]++;
    }
    m[6] = m[2] + 6;
    outbf();
    if (m[3] == 0) goto Lap;
    m[6] = 5; /* immed clear */
    outbf();
Lap:;
    m[1] = 1;
    if (m[12] == 1) {
        m[6] = 3;
        outbf(); /* post */
        m[12] = 0;
    }
    goto Laj;
Laz:;
#ifdef BFVM
    putchar('?');
#else
    putchar('#');
#endif
    return 0;
Lab:;
    if (m[11] == 0 || m[12] == 1) {
        m[6] = 3;
        outbf(); /* post */
    }
    m[6] = 2;
    outbf(); /* pre */
    m[6] = 3;
    outbf(); /* post */
    m[6] = 1;
    outbf(); /* last */
    return 0;
}

void outrep() {
    #ifndef RLE
        #ifndef DISABLE_OPT
            if(m[3] < 15 || !freecell) {
        #endif
                while (m[3]) {
                    putchar(m[6]);
                    m[3]--;
                }
        #ifndef DISABLE_OPT
            } else {
                translate(m[3], best_base(m[3]));
                m[3] = 0;
            }
        #endif
    #else
        if(m[3] > 1) {
            #ifndef RLE_POSTFIX
                printf("%d%c", m[3], m[6]);
            #else
                printf("%c%d", m[6], m[3]);
            #endif
            m[3] = 0;
        } else if(m[3]) {
            putchar(m[6]);
            m[3] = 0;
        }
    #endif
    
    freecell = 0;
}

void outbf() {
    unsigned int r1, r4;
    m[7] = C4;
    r4 = 0;
o1:;
    if (r4 >= m[6]) goto o2;
o3:;
    r1 = m[m[7]];
    m[7]++;
    if (r1 != '\0') goto o3;
    r4++;
    goto o1;
o2:;
    r1 = m[m[7]];
    if (r1 == '\0') goto o4;
    if (r1 != '1') goto o5;
    r1 = m[5];
o5:;
    if (r1 != '2') goto o6;
    r1 = m[4];
o6:;
    if (r1 == '#') { r1 = STACK-2; goto o11; } /* when stack changes, set it */
    if (r1 != '*') goto o7;
    if(rseg == 0) r1 = m[9] + off * 2; // ???
    else r1 = m[9] + off;
    goto o11;
o7:;
    if (r1 != '^') goto o8;
    if(rseg == 0) r1 = m[10] + off * 2; // ???
    else r1 = m[10] + off;
    goto o11;
o8:;
    if (r1 < 'a') goto o9;
    if (r1 > 'z') goto o9;
    r1 = r1 - 'a';
o11:;
    if (r1 < m[8]) goto o12;
    r4 = 0;
    m[15] = r1 - m[8];
#ifdef RLE
o14:;
    if (m[15] <= r4) goto o13;
    if (m[15] > 1) {
        #ifndef RLE_POSTFIX
            printf("%d>", m[15]);
        #else
            printf(">%d", m[15]);
        #endif
        r4 = m[15];
    } else if(m[15]) {
        putchar('>');
        
        r4 = m[15];
    }
    goto o14;
#else
o14:;
    if (m[15] <= r4) goto o13;
    putchar('>');
    r4++;
    goto o14;
#endif
o12:;
    r4 = 0;
    m[15] = m[8] - r1;
#ifdef RLE
o16:;
    if (m[15] <= r4) goto o13;
    if (m[15] > 1) {
        #ifndef RLE_POSTFIX
            printf("%d<", m[15]);
        #else
            printf("<%d", m[15]);
        #endif
        r4 = m[15];
    } else if(m[15]) {
        putchar('<');
        
        r4 = m[15];
    }
    goto o16;
#else
o16:;
    if (m[15] <= r4) goto o13;
    putchar('<');
    r4++;
    goto o16;
#endif
o13:;
    m[8] = r1;
    goto o10;
o9:;
    putchar(r1);
o10:;
    m[7]++;
    goto o2;
o4:;
}

unsigned int inchar() {
    int c = getchar();
    if (c <= 0)
        c = 0;
    return c;
}

int stack_usage(int n, int base) {
    int sp = 0;
    
    while(n != 0) {
        sp++;
        n /= base;
    }
    
    return sp;
}

int grade(int n, int base) {
    int sp = 0, norm = 0;
    
    while(n != 0) {
        sp++;
        norm += n % base;
        n /= base;
    }
    
    return norm + (6 + base) * sp + (sp % 2 == 1 ? 4 : 0);
}

int best_base(int n) {
    int v = 0, b = 0, i = 2;
    
    for(; i < 60; i++) {
        int cv = grade(n, i);
        
        if(v == 0 || v > cv) {
            v = cv;
            b = i;
        }
    }
    
    return b;
}

void translate(int n, int base) {
    int stack[stack_usage(n, base) + 1], sp = 0, flip = 1;
    
    while(n != 0) {
        stack[sp++] = n % base;
        n /= base;
    }
    
    putchar('>');
    
    while(sp != 0) {
        sp--;
        
        int bc = base;
        
        while(stack[sp]--)
            putchar('+');
        
        if(sp != 0) {
            if(!flip) {
            putchar('[');
            putchar('>');
            
            while(bc--)
                putchar('+');
            
            putchar('<');
            putchar('-');
            putchar(']');
            putchar('>');
            } else {
            putchar('[');
            putchar('<');
            
            while(bc--)
                putchar('+');
            
            putchar('>');
            putchar('-');
            putchar(']');
            putchar('<');
            }
        }
        
        flip = !flip;
    }
    
    if(!flip) {
        putchar('[');
        putchar('-');
        putchar('<');
        putchar('+');
        putchar('>');
        putchar(']');
        putchar('<');
    }
}
