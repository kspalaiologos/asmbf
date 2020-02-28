
/* bfasmapi.c
 *
 * Copyright (C) Krzysztof Palaiologos Szewczyk, Maciej Maviek Stanka, 2020.
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

#include "bfasmapi.h"

unsigned int inchar();
void outbf();
void outrep();

static unsigned int m[2000], off, freecell;
static char *ac, *bc;
static long bcl, bsz, ip;
static unsigned short int* mem = NULL;

static int (*infptr)() = &getchar;
static int (*outfptr)(int) = &putchar;

int best_base(int n);
void translate(int n, int base);
int bf_set_input_func(int (*ptr)()) {
  infptr = ptr;
}
int bf_set_output_func(int (*ptr)(int)) {
  outfptr = ptr;
}

int bf_get_reg(int reg) {
  switch (reg) {
    case 1:
    case 2:
    case 3:
    case 4:
      return mem[4 + reg];
    case 5:
    // return mem[13];
    case 6:
    // return mem[21];
    default:
      printf("bfasm: bf_get_reg: invalid register: `r%d`", reg);
      exit(EXIT_FAILURE);
  }
}

int bf_set_reg(int reg, int val) {
  switch (reg) {
    case 1:
    case 2:
    case 3:
    case 4:
      return mem[4 + reg] = val;
    case 5:
    // return mem[13] = val;
    case 6:
    // return mem[21] = val;
    default:
      printf("bfasm: bf_set_reg: invalid register: `r%d`", reg);
      exit(EXIT_FAILURE);
  }
}

void bfmatch(int dir) {
  int level = 1;
  char ipvalue;
  do {
    ip += dir;
    if (ip < 0 || ip >= bcl) {
      puts("Mismatched brackets");
      exit(EXIT_FAILURE);
    }
    ipvalue = bc[ip];
    if (ipvalue == 91)
      level += dir;
    else if (ipvalue == 93)
      level -= dir;
  } while (level > 0);
}

int bfgen(int c) {
  bc[bcl++] = c;
  if (bcl >= bsz) {
    bc = realloc(bc, bsz + 64);
    bsz += 64;
  }
}

int __bfasm(char* code) {
  unsigned int n;
  long mp = 0, maxmp = 1023;
  ac = code;
  bc = malloc(64);
  bsz = 64;
  bcl = 0;
  char* s =
      "addanddecdiveq_ge_gt_in_incjmpjnzjz_lblle_lt_modmovmulne_negnotor_"
      "outpoppshrclstosubswpclrretendlogaslasrpowsrvampsmpnavstkorgdb_txtrawseg"
      "a+b+[\0"                                   /* 0 first */
      "b]\0"                                      /* 1 last (end, post, last) */
      "a[c+d+a-]c[a+c-]d[[-]\0"                   /* 2 pre */
      "d]\0"                                      /* 3 post */
      "2\0"                                       /* 4 immed */
      "2[-]\0"                                    /* 5 immed clear */
      "2[1+e+2-]e[2+e-]\0"                        /* 6 add */
      "1[e+1-]e[e[-]2[e+d+2-]d[2+d-]e[1-e[-]]]\0" /* and */
      "2-\0"                                      /* dec */
      "1[c+1-]c[2[d+e+2-]e[2+e-]d[e+c-[e[-]k+c-]k[c+k-]e[d-[1-d[-]]+e-]d-]1+c]"
      "\0"
      "1[d+1-]+2[d-c+2-]c[2+c-]d[1-d[-]]\0" /* eq_ */
      "1[d+1-]+2[c+k+e+2-]e[2+e-]k[d[l+e+d-]e[d+e-]+l[c-d-e-l[-]]e[k[-]+e-]k-]"
      "c[1-c[-]]d[-]\0" /* ge_ */
      "1[d+1-]2[c+k+e+2-]e[2+e-]k[d[l+e+d-]e[d+e-]+l[c-d-e-l[-]]e[k[-]+e-]k-]d["
      "1+d[-]]c[-]\0"                                     /* gt_ */
      "2,\0"                                              /* in_ */
      "2+\0"                                              /* inc */
      "a[-]b[-]2[b+c+2-]c[2+c-]\0"                        /* jmp addr */
      "1[c+d+1-]c[1+c-]d[a[-]b[-]2[b+c+2-]c[2+c-]d[-]]\0" /* jnz val, addr */
      "d+1[d[-]c+1-]c[1+c-]d[a[-]b[-]2[b+c+2-]c[2+c-]d[-]]\0" /* jz_ val, addr
                                                                 */
      "c+a[c-d+a-]d[a+d-]c[-d+b[e-c+b-]c[b+c-]e[d-e[-]]d[a+d-]c]e[-]\0" /* lbl
                                                                           */
      "1[d+1-]+2[c+k+e+2-]e[2+e-]k[d[l+e+d-]e[d+e-]+l[c-d-e-l[-]]e[k[-]+e-]k-]"
      "d[1-d[-]]c[-]\0" /* le_ */
      "1[d+1-]2[c+k+e+2-]e[2+e-]k[d[l+e+d-]e[d+e-]+l[c-d-e-l[-]]e[k[-]+e-]k-]c["
      "1+c[-]]d[-]\0"                                            /* lt_ */
      "1[-n+1]2[-o+2]n[>->+<[>]>[<+>-]<<[<]>-]p[-1+p]o[-]p[-]\0" /* mod */
      "1[-]2[1+e+2-]e[2+e-]\0"                                   /* 22 mov */
      "1[d+1-]d[2[1+e+2-]e[2+e-]d-]\0"                           /* mul */
      "1[d+1-]2[d-e+2-]e[2+e-]d[1+d[-]]\0"                       /* ne_ */
      "2[e-2-]e[2+e-]\0"                                         /* neg */
      "2-[e-2-]e[2+e-]\0"                                        /* not */
      "1[e+1-]e[1-e[-]]2[e+d+2-]d[2+d-]e[1[-]-e[-]]\0"           /* or_ */
      "2.\0"                                                     /* out */
      "2[-]q[-]>[>>]<<->[<<<[<<]>+>[>>]>-]<<<[<<]>[2+q-]\0"      /* pop */
      "2[e+q+2-]e[2+e-]q>[>>]+<<[<<]>[>[>>]<+<[<<]>-]\0"         /* psh */
      "1[-]2[e+o+*>+<2-]e[2+e-]*>[[>>]+[<<]>>-]+[>>]<[<[<<]>+1+*>[>>]<-]<[<<]>["
      ">[>>]<+<[<<]>-]>[>>]<<[-<<]>\0" /* rcl */
      "1[e+*>+<1-]e[1+e-]2[e+*+2-]e[2+e-]*>[[>>]+[<<]>>-]+[>>]<[-]<[<<]>[>[>>]<"
      "+<[<<]>-]>[>>]<<[-<<]>\0"                                 /* sto */
      "2[1-e+2-]e[2+e-]\0"                                       /* sub */
      "1[e+1-]2[1+2-]e[2+e-]\0"                                  /* swp */
      "2[-]\0"                                                   /* clr */
      "a[-]b[-]q[-]>[>>]<<->[<<<[<<]>+>[>>]>-]<<<[<<]>[b+q-]\0"  /* ret */
      "a[-]b[-]\0"                                               /* end */
      "e[-]2[[-]e+2]e[-2+e]2\0"                                  /* log */
      "e[-]2[-e++2]e[-2+e]\0"                                    /* asl */
      "c[-]2[-c+2]c[-[->>+<]>[<]<]e[-2+e]\0"                     /* asr */
      "c[-]2[c+2-]2+1[d[-]e[-]2[e+2-]e[c[2+d+c-]d[c+d-]e-]1-]\0" /* pow */
      "q[-]>[>>]<<-<[>+<-]>>[<<+>>-]<[>+<-]+[<<]>\0"             /* srv */
      "1[e+*>+<1-]e[1+e-]2[e+*+2-]e[2+e-]*>[[>>]+[<<]>>-]+>[>[>>]<+<[<<]>-]>[>>"
      "]<<[-<<]>\0" /* amp */
      "1[e+*>+<1-]e[1+e-]2[e+*+2-]e[2+e-]*>[[>>]+[<<]>>-]+>[>[>>]<-<[<<]>-]>[>>"
      "]<<[-<<]>\0" /* smp */
      "2\0"         /* nav */
      ;
  for (n = 0; n < 1900; n++)
    m[n + 20] = s[n];
  m[6] = 0;
  m[8] = 0;
  m[9] = 18;
  m[10] = m[9] + 2;
  outbf();
  m[11] = 1;
  m[12] = 0;
  m[1] = 1;
Laa:;
  m[0] = inchar();
  if (m[1] == 1)
    m[0] = (m[0] >= 'A' && m[0] <= 'Z') ? (m[0]) - 'A' + 'a' : m[0];
Laj:;
  if (m[0] != '\0')
    goto Lam;
  if (m[1] == 2)
    goto Lai;
  goto Lab;
Lam:;
  if (m[0] != '\n' && m[0] != '\v' && m[0] != '\f' && m[0] != '\r')
    goto Lad;
  if (m[1] == 2)
    goto Lai;
  m[1] = 1;
  goto Laa;
Lad:;
  if (m[1] == 4) {
    if (m[0] == '\"') {
      m[1] = 1;
      goto Laa;
    }
    m[2] = 42;
    m[3] = m[0];
    goto Lai;
  }
  if (m[0] == ' ' || m[0] == '\t' || m[1] == 0)
    goto Laa;
  if (m[1] != 3)
    goto Lay;
  if (m[0] != '\"')
    goto Laz;
  m[1] = 4;
  goto Laa;
Lay:;
  if (m[0] != ';')
    goto Lac; /* comment */
  if (m[1] == 2)
    goto Lai;
  m[1] = 0;
  goto Laa;
Lac:;
  if (m[1] != 1)
    goto Lae; /* get 3 letter instruction */
  m[2] = inchar();
  m[3] = inchar();
  m[2] = (m[2] >= 'A' && m[2] <= 'Z') ? (m[2]) - 'A' + 'a' : m[2];
  m[3] = (m[3] >= 'A' && m[3] <= 'Z') ? (m[3]) - 'A' + 'a' : m[3];
  m[4] = 0;
Laf:;
  m[5] = m[4] + 20;
  m[6] = m[m[5]];
  if (m[0] != m[6])
    goto Lag;
  m[5]++;
  m[6] = m[m[5]];
  if (m[2] != m[6])
    goto Lag;
  m[5]++;
  m[6] = m[m[5]];
  if (m[3] == m[6])
    goto Lah;
Lag:;
  m[4]++;
  m[4]++;
  m[4]++;
  if (m[4] == 138)
    goto Laz; /* not found, quit */
  goto Laf;
Lah:;
  m[1] = 2;
  m[2] = m[4] / 3;
  m[3] = 0;
  m[4] = 0;
  m[5] = 0;
  goto Laa;
Lae:; /* find operands */
  if (m[2] != 43)
    goto Lax; /* txt command */
  m[1] = 3;
  goto Laj;
Lax:;
  if (m[0] != 'r' && m[0] != 'R')
    goto Lak; /* r_ operand */
  m[0] = inchar();
  m[0] = m[0] - '1';
  if (m[0] > 6)
    goto Laz; /* not a number, quit */
  m[4] = m[0] != 4 ? (m[0] < 4 ? (m[0] + 'f') : (m[0] + 'q')) : 'm';
  goto Laa;
Lak:;
  if (m[0] != ',')
    goto Lan; /* , */
  m[5] = m[4];
  m[4] = 0;
  goto Laa;
Lan:;
  if (m[0] != '.')
    goto Lal; /* .char */
  m[3] = inchar();
  goto Laa;
Lal:; /* number */
  m[0] = m[0] - '0';
  if (m[0] > 9)
    goto Laz; /* not a number, quit */
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
    case 10: /* jnz */
      m[11] = 1;
      m[12] = 1;
      goto Lao;
    case 11: /* jz_ */
      m[11] = 1;
      m[12] = 1;
      goto Lao;
    case 12: /* lbl */
      if (m[11] == 0) {
        m[6] = 3;
        outbf(); /* post */
      }
      m[11] = 1;
      m[6] = 4;
      m[4] = 'e';
      outbf();
      m[6] = '+';
      outrep();  // note: potential optimalization, << & < free
      m[6] = 18;
      outbf();
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
    case 40: /* stk */
      m[9] = m[3] * 2 + 18;
      goto Lap;
    case 41: /* org */
      m[10] = m[3] * 2 + m[9] + 2;
      goto Lap;
    case 42: /* db_ */
      m[6] = 4;
      m[4] = '^';
      outbf();
      m[6] = '+';
      freecell = 1;
      outrep();
      m[10] = m[10] + 2;
      if (m[1] == 4)
        goto Laa;
      goto Lap;
    case 44: /* raw */
      bfgen(m[3]);
      goto Lap;
    case 45: /* seg */
      off = m[3];
      m[10] = m[9] + 2;
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
  if (m[3] == 0)
    goto Lap;
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
  bfgen('#');
  return 0;
Lab:;
  if (m[11] == 0 || m[12] == 1) {
    m[6] = 3;
    outbf(); /* post */
  }
  m[6] = 2;
  outbf(); /* pre */
  m[6] = 37;
  outbf(); /* end */
  m[6] = 3;
  outbf(); /* post */
  m[6] = 1;
  outbf(); /* last */
  bfgen('\0');

  if (mem == NULL)
    mem = calloc(1024, sizeof(unsigned short int));

  mem[0] = 0;
  mem[1] = 0;
  ip = -1;

  while (++ip < bcl) {
    switch (bc[ip]) {
      case '>':
        if (mp >= maxmp) {
          mem = realloc(mem, (maxmp + 1024) * sizeof(unsigned short int));
          if (mem == NULL) {
            puts("Out of memory");
            return EXIT_FAILURE;
          }
          for (n = 1; n <= 1024; n++)
            mem[maxmp + n] = 0;
          maxmp += 1024;
        }
        mp++;
        break;
      case '<':
        if (mp <= 0) {
          puts("Access Violation");
          return EXIT_FAILURE;
        }
        mp--;
        break;
      case '+':
        mem[mp]++;
        break;
      case '-':
        mem[mp]--;
        break;
      case '.':
        outfptr(mem[mp]);
        break;
      case ',':
        n = infptr();
        if (n < 0)
          n = 0;
        mem[mp] = n;
        break;
      case '[':
        if (bc[ip + 1] == '-' && bc[ip + 2] == ']') {
          mem[mp] = 0;
          ip += 2;
        } else if (mem[mp] == 0)
          bfmatch(1);
        break;
      case ']':
        bfmatch(-1);
        ip--;
        break;
    }
  }

  free(bc);
  return EXIT_SUCCESS;
}

void outrep() {
#ifndef DISABLE_OPT
  if (m[3] < 15 || !freecell) {
#endif
    while (m[3]) {
      bfgen(m[6]);
      m[3]--;
    }
#ifndef DISABLE_OPT
  } else {
    translate(m[3], best_base(m[3]));
    m[3] = 0;
  }
#endif

  freecell = 0;
}

void outbf() {
  unsigned int r1, r4;
  m[7] = 158;
  r4 = 0;
o1:;
  if (r4 >= m[6])
    goto o2;
o3:;
  r1 = m[m[7]];
  m[7]++;
  if (r1 != '\0')
    goto o3;
  r4++;
  goto o1;
o2:;
  r1 = m[m[7]];
  if (r1 == '\0')
    goto o4;
  if (r1 != '1')
    goto o5;
  r1 = m[5];
o5:;
  if (r1 != '2')
    goto o6;
  r1 = m[4];
o6:;
  if (r1 != '*')
    goto o7;
  r1 = m[9] + off * 2;
  goto o11;
o7:;
  if (r1 != '^')
    goto o8;
  r1 = m[10] + off * 2;
  goto o11;
o8:;
  if (r1 < 'a')
    goto o9;
  if (r1 > 'z')
    goto o9;
  r1 = r1 - 'a';
o11:;
  if (r1 < m[8])
    goto o12;
  r4 = 0;
  m[15] = r1 - m[8];
o14:;
  if (m[15] <= r4)
    goto o13;
  bfgen('>');
  r4++;
  goto o14;
o12:;
  r4 = 0;
  m[15] = m[8] - r1;
o16:;
  if (m[15] <= r4)
    goto o13;
  bfgen('<');
  r4++;
  goto o16;
o13:;
  m[8] = r1;
  goto o10;
o9:;
  bfgen(r1);
o10:;
  m[7]++;
  goto o2;
o4:;
}

unsigned int inchar() {
  int c = *ac++;
  if (c <= 0)
    c = 0;
  return c;
}

int stack_usage(int n, int base) {
  int sp = 0;

  while (n != 0) {
    sp++;
    n /= base;
  }

  return sp;
}

int grade(int n, int base) {
  int sp = 0, norm = 0;

  while (n != 0) {
    sp++;
    norm += n % base;
    n /= base;
  }

  return norm + (6 + base) * sp + (sp % 2 == 1 ? 4 : 0);
}

int best_base(int n) {
  int v = 0, b = 0;

  for (int i = 2; i < 60; i++) {
    int cv = grade(n, i);

    if (v == 0 || v > cv) {
      v = cv;
      b = i;
    }
  }

  return b;
}

void translate(int n, int base) {
  int stack[stack_usage(n, base) + 1], sp = 0, flip = 1;

  while (n != 0) {
    stack[sp++] = n % base;
    n /= base;
  }

  bfgen('[');
  bfgen('-');
  bfgen(']');
  bfgen('>');

  while (sp != 0) {
    sp--;

    int bc = base;

    while (stack[sp]--)
      bfgen('+');

    if (sp != 0) {
      if (!flip) {
        bfgen('[');
        bfgen('>');

        while (bc--)
          bfgen('+');

        bfgen('<');
        bfgen('-');
        bfgen(']');
        bfgen('>');
      } else {
        bfgen('[');
        bfgen('<');

        while (bc--)
          bfgen('+');

        bfgen('>');
        bfgen('-');
        bfgen(']');
        bfgen('<');
      }
    }

    flip = !flip;
  }

  if (!flip) {
    bfgen('[');
    bfgen('-');
    bfgen('<');
    bfgen('+');
    bfgen('>');
    bfgen(']');
    bfgen('<');
  }
}
