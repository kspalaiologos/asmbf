
#include <stdio.h>
#include <stdlib.h>

unsigned int inchar();
void outbf();
void outrep();
unsigned int m[2000];

int main(void) {
    unsigned int n;
    char * s = "addanddecdiveq_ge_gt_in_incjmpjnzjz_lblle_lt_modmovmulne_negnotor_outpoppshrclstosubswpclrretendstkorgdb_txtraw"
               "a+b+[\0" /* 0 first */
               "b]\0" /* 1 last (end, post, last) */
               "a[c+d+a-]c[a+c-]d[[-]\0" /* 2 pre */
               "d]\0" /* 3 post */
               "2\0" /* 4 immed */
               "2[-]\0" /* 5 immed clear */
               "2[1+e+2-]e[2+e-]\0" /* 6 add */
               "1[e+1-]e[e[-]2[e+d+2-]d[2+d-]e[1-e[-]]]\0" /* and */
               "2-\0" /* dec */
               "1[c+1-]c[2[d+e+2-]e[2+e-]d[e+c-[e[-]k+c-]k[c+k-]e[d-[1-d[-]]+e-]d-]1+c]\0"
               "1[d+1-]+2[d-c+2-]c[2+c-]d[1-d[-]]\0" /* eq_ */
               "1[d+1-]+2[c+k+e+2-]e[2+e-]k[d[l+e+d-]e[d+e-]+l[c-d-e-l[-]]e[k[-]+e-]k-]c[1-c[-]]d[-]\0" /* ge_ */
               "1[d+1-]2[c+k+e+2-]e[2+e-]k[d[l+e+d-]e[d+e-]+l[c-d-e-l[-]]e[k[-]+e-]k-]d[1+d[-]]c[-]\0" /* gt_ */
               "2,\0" /* in_ */
               "2+\0" /* inc */
               "a[-]b[-]2[b+c+2-]c[2+c-]\0" /* jmp addr */
               "1[c+d+1-]c[1+c-]d[a[-]b[-]2[b+c+2-]c[2+c-]d[-]]\0" /* jnz val, addr */
               "d+1[d[-]c+1-]c[1+c-]d[a[-]b[-]2[b+c+2-]c[2+c-]d[-]]\0" /* jz_ val, addr */
               "c+a[c-d+a-]d[a+d-]c[-d+b[e-c+b-]c[b+c-]e[d-e[-]]d[a+d-]c]e[-]\0" /* lbl */
               "1[d+1-]+2[c+k+e+2-]e[2+e-]k[d[l+e+d-]e[d+e-]+l[c-d-e-l[-]]e[k[-]+e-]k-]d[1-d[-]]c[-]\0" /* le_ */
               "1[d+1-]2[c+k+e+2-]e[2+e-]k[d[l+e+d-]e[d+e-]+l[c-d-e-l[-]]e[k[-]+e-]k-]c[1+c[-]]d[-]\0" /* lt_ */
               "1[k+1-]2[l+e+2-]e[2+e-]k[->>+<-[>]>[[<+>-]>>>+<]<<<<]p[-]m[1+m-]l[-]\0" /* mod */
               "1[-]2[1+e+2-]e[2+e-]\0" /* 22 mov */
               "1[d+1-]d[2[1+e+2-]e[2+e-]d-]\0" /* mul */
               "1[d+1-]2[d-e+2-]e[2+e-]d[1+d[-]]\0" /* ne_ */
               "2[e-2-]e[2+e-]\0" /* neg */
               "2-[e-2-]e[2+e-]\0" /* not */
               "1[e+1-]e[1-e[-]]2[e+d+2-]d[2+d-]e[1[-]-e[-]]\0" /* or_ */
               "2.\0" /* out */
               "2[-]q[-]>[>>]<<->[<<<[<<]>+>[>>]>-]<<<[<<]>[2+q-]\0" /* pop */
               "2[e+q+2-]e[2+e-]q>[>>]+<<[<<]>[>[>>]<+<[<<]>-]\0" /*  psh */
               "1[-]2[e+o+*>+<2-]e[2+e-]*>[[>>]+[<<]>>-]+[>>]<[<[<<]>+1+*>[>>]<-]<[<<]>[>[>>]<+<[<<]>-]>[>>]<<[-<<]>\0"  /* rcl */
               "1[e+*>+<1-]e[1+e-]2[e+*+2-]e[2+e-]*>[[>>]+[<<]>>-]+[>>]<[-]<[<<]>[>[>>]<+<[<<]>-]>[>>]<<[-<<]>\0" /* sto */
               "2[1-e+2-]e[2+e-]\0" /* sub */
               "1[e+1-]2[1+2-]e[2+e-]\0" /* swp */
               "2[-]\0" /* clr */
               "a[-]b[-]q[-]>[>>]<<->[<<<[<<]>+>[>>]>-]<<<[<<]>[b+q-]\0" /* ret */
               "a[-]b[-]\0"; /* end */
    for (n = 0; n < 1900; n++)  m[n + 20] = s[n];
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
        m[2] = 34;
        m[3] = m[0];
        goto Lai;
    }
    if (m[0] == ' ' || m[0] == '\t' || m[1] == 0) goto Laa;
    if (m[1] != 3) goto Lay;
    if (m[0] != '\"') goto Laz;
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
    if (m[4] == 111) goto Laz; /* not found, quit */
    goto Laf;
Lah:;
    m[1] = 2;
    m[2] = m[4] / 3;
    m[3] = 0;
    m[4] = 0;
    m[5] = 0;
    goto Laa;
Lae:; /* find operands */
    if (m[2] != 35) goto Lax; /* txt command */
    m[1] = 3;
    goto Laj;
Lax:;
    if (m[0] != 'r') goto Lak; /* r_ operand */
    m[0] = inchar();
    m[0] = m[0] - '1';
    if (m[0] > 3) goto Laz; /* not a number, quit */
    m[4] = m[0] + 'f';
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
    if (m[0] > 9) goto Laz; /* not a number, quit */
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
            outrep();
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
        case 32: /* stk */
            m[9] = m[3] * 2 + 18;
            goto Lap;
        case 33: /* org */
            m[10] = m[3] * 2 + m[9] + 2;
            goto Lap;
        case 34: /* db_ */
            m[6] = 4;
            m[4] = '^';
            outbf();
            m[6] = '+';
            outrep();
            m[10] = m[10] + 2;
            if (m[1] == 4) goto Laa;
            goto Lap;
        case 36: /* raw */
            putchar(m[3]);
            goto Lap;
    }
Lao:;
    if (m[4] == 0) {
        m[6] = 4; /* immed */
        m[4] = 'j';
        outbf();
        m[6] = '+';
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
    putchar('#');
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
    return 0;
}

void outrep() {
    while (m[3]) {
        putchar(m[6]);
        m[3]--;
    }
}

void outbf() {
    unsigned int r1, r4;
    m[7] = 131;
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
    if (r1 != '*') goto o7;
    r1 = m[9];
    goto o11;
o7:;
    if (r1 != '^') goto o8;
    r1 = m[10];
    goto o11;
o8:;
    if (r1 < 'a') goto o9;
    if (r1 > 'z') goto o9;
    r1 = r1 - 'a';
o11:;
    if (r1 < m[8]) goto o12;
    r4 = 0;
    m[15] = r1 - m[8];
o14:;
    if (m[15] <= r4) goto o13;
    putchar('>');
    r4++;
    goto o14;
o12:;
    r4 = 0;
    m[15] = m[8] - r1;
o16:;
    if (m[15] <= r4) goto o13;
    putchar('<');
    r4++;
    goto o16;
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
