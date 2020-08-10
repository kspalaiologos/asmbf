
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct delta_match {
    int32_t coefficient;
    uint8_t type;
};

struct replacement {
    char * src;
    char * dest;
};

struct delta_match queue[16];
unsigned int qp = 0, pos = 0;

struct delta_match suck_delta(void) {
    int c;
    struct delta_match m;
    
    m.coefficient = 0;
    
    if(qp) {
        return queue[--qp];
    }
    
    while(isdigit(c = getchar())) {
        pos++;
        m.coefficient *= 10;
        m.coefficient += c - '0';
    }
    
    if(!m.coefficient)
        m.coefficient = 1;
    
    if(c == '<' || c == '-')
        m.coefficient = -m.coefficient;
    
    if(c != '>' && c != '+' && c != '<' && c != '-') {
        ungetc(c, stdin);
        
        // probs no value.
        m.type = 0; m.coefficient = 0;
        
        return m;
    } else
        pos++;
    
    m.type = c == '<' || c == '>';
    
    fprintf(stderr, "\033[32mDebug: sucked delta: %d, %d\033[37m\n", m.coefficient, m.type);
    
    return m;
}

void match_ignore(void) { suck_delta(); }
void match_const(void) { printf("%d", suck_delta().coefficient); }
void match_value(void) { int32_t x = suck_delta().coefficient; if(x) printf("tape[mp] += %d;\n", x); }
void match_mp(void) { int32_t x = suck_delta().coefficient; if(x) printf("mp += %d;\n", x); }
void match_flexy(void) {
    struct delta_match m = suck_delta();
    
    if(m.type == 2) {
        fprintf(stderr, "\033[31m*** Severe: Incorrect flexy match type.\033[37m\n");
        abort();
    }
    
    if(!m.coefficient) return;
    
    printf(!m.type ? "tape[mp] += %d;\n" : "mp += %d;\n", m.coefficient);
}

void emit(const char * template) {
    uint8_t match_swap = 0;

    while(*template) {
        if(match_swap) {
            match_swap = 0;
            
            switch(*template) {
                case 'V': match_value(); break;
                case 'M': match_mp(); break;
                case 'C': match_const(); break;
                case 'I': match_ignore(); break;
                case 'T': printf("tape[mp]"); break;
                case 'S': pos++; getchar(); break;
                default: putchar('$'); putchar(*template);
            }
            
            template++;

            continue;
        }

        if(*template == '$')
            match_swap = 1;
        else
            putchar(*template);
        
        template++;
    }
}

struct replacement vm_model[] = {
    {"AA", "G=1;IP=1;mp=OFF('b');while(tape[mp]){$I"},
    {"AB", "mp=OFF('b');if(tape[0])tape[1]=0;}$I"},
    {"AC", "mp=OFF('a');if($T){$I"},
    {"AD", "mp=OFF('c');}$I"},
    {"AE", "$M$S"},
    {"AF", "$M$S$T=0;"},
    {"AG", "$Mt0=$T;$S$M$T+=t0;$S"},
    {"AH", "$Mt0=$T;$S$M$T=$T&&t0;$S"},
    {"AI", "$M$T--;$S"},
    {"AJ", "$Mt0=$T;$S$M$T/=t0;$S"},
    {"AK", "$Mt0=$T;$S$M$T=$T==t0;$S"},
    {"AL", "$Mt0=$T;$S$M$T=$T>=t0;$S"},
    {"AM", "$Mt0=$T;$S$M$T=$T>t0;$S"},
    {"AN", "$M$T=inchar();$S"},
    {"BL", "$M$T++;$S"},
    {"AO", "$MIP=$T;G=0;$S"},
    {"AP", "$Mt0=$T;$S$Mif(t0){$SIP=$T;G=0;}"},
    {"AQ", "$Mt0=$T;$S$Mif(!t0){$SIP=$T;G=0;}"},
    {"AR", "$S$S$M$Sif(IP==$C)G=1;$S$S"},
    {"AS", "$Mt0=$T;$S$M$S$T=$T<=t0;"},
    {"AT", "$Mt0=$T;$S$M$S$T=$T<t0;"},
    {"AU", "$Mt0=$T;$S$M$S$T=$T%t0;"},
    {"AV", "$Mt0=$T;$S$M$S$T=t0;"},
    {"AW", "$Mt0=$T;$S$M$S$T=$T*t0;"},
    {"AX", "$Mt0=$T;$S$M$S$T=$T!=t0;"},
    {"AY", "$M$T=-$T;$S"},
    {"AZ", "$M$T=!$T;$S"},
    {"BA", "$Mt0=$T;$S$M$T=$T||t0;$S"},
    {"BB", "$Mputchar($T);$S"},
    {"BC", "$Mt1=mp;$S$M$St2=mp;t0=tape[mp+2*sp];sp--;tape[mp=t1]=t0;mp=t2;"},
    {"BD", "$Mt0=$T;$S$M$Stape[mp+2*t0]=t0;sp++;"},
    {"BE", "$Mt2=mp;$S$M$St0=$T;$Mt1=tape[mp+2+2*t0];tape[t2]=t1;"},
    {"BF", "$Mt0=$T;$S$M$St1=$T;$Mtape[mp+2+2*t0]=t1;"},
    {"BG", "$Mt0=$T;$S$M$S$T-=t0;"},
    {"BH", "$Mt2=mp;$S$M$St3=mp;mp=t2;t0=$T;mp=t3;t1=$t;mp=t2;$T=t0;mp=t3;$T=t1;"},
    {"BI", "$M$S$T=0;"},
    {"BJ", "$M$SIP=tape[mp+2*sp];sp--;G=0;"},
    {"BK", "return 0;"},
    {"BM", "$M$S$T=$T==1;"},
    {"BN", "$M$S$T=$T<<1;"},
    {"BO", "$M$S$T=$T>>1;"},
    {"BP", "$M$St0=$T;$M$T=bfpow($T,t0);"},
    {"BR", "$M$St0=tape[mp+2*sp];tape[mp+2*sp]=tape[mp+2*(sp-1)];tape[mp+2*(sp-1)]=t0;"},
    {"BS", "$Mt2=$T;$S$M$St1=mp;$M$Stape[mp+2+2*t1]+=t2;"},
    {"BT", "$Mt2=$T;$S$M$St1=mp;$M$Stape[mp+2+2*t1]-=t2;"},
    {"BU", "$M$Z"},
    {NULL, NULL}
};

int try_match(char buf[]) {
    struct replacement current_row;
    int i = 0;
    
    while((current_row = vm_model[i]).src) {
        if(!strcmp(buf, current_row.src)) {
            emit(current_row.dest);
            return 1;
        }
        
        i++;
    }
    
    return 0;
}

int main(void) {
    char match[32] = {0};
    int c, mp = 0;
    
    printf(
        #ifndef FREESTANDING
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n"
        "#include <stdint.h>\n"
        #endif
        "#define OFF(x) ((x) - 'a')\n"
        "#define G (tape[0])\n"
        "#define IP (tape[1])\n"
        #ifdef BFVM_32
        "#define type uint32_t\n"
        #else
        "#define type uint16_t\n"
        #endif
        #ifndef FREESTANDING
        "uint8_t inchar(void) {\n"
            "uint8_t v = getchar();\n"
            "return v < 0 ? 0 : v;\n"
        "}\n"
        #endif
        "type bfpow(type x, type y) {\n"
            "type i = 1, s = x; for(; i < y; y++) s *= x; return s;\n"
        "}\n"
        "int main(void) {\n"
        #ifndef FREESTANDING
            #ifdef BFVM_HEAP
                "type*tape=calloc(sizeof(type)," BFVM_HEAP "),mp,t0,t1,t2,t3,sp;\n"
            #else
                "type*tape=calloc(sizeof(type),65536),mp,t0,t1,t2,t3,sp;\n"
            #endif
        #else
            "type*tape=0x7000,mp,t0,t1,t2,t3,sp;\n"
        #endif
    );
    
    while((c = getchar()) != EOF) {
        pos++;
        switch(c) {
            case '0' ... '9':
                fprintf(stderr, "\033[36mDebug: found a backfill delta.\033[37m\n");
                ungetc(c, stdin);
                match_flexy();
                break;
            case '+': fprintf(stderr, "\033[31mDebug: Stray BF @%d\033[37m\n", pos); printf("tape[mp]++;"); break;
            case '>': fprintf(stderr, "\033[31mDebug: Stray BF @%d\033[37m\n", pos); printf("mp++;"); break;
            case '-': fprintf(stderr, "\033[31mDebug: Stray BF @%d\033[37m\n", pos); printf("tape[mp]--;"); break;
            case '<': fprintf(stderr, "\033[31mDebug: Stray BF @%d\033[37m\n", pos); printf("mp++;"); break;
            case 'Z': fprintf(stderr, "\033[31mDebug: !! Z not matched @%d\033[37m\n", pos); case '\n': case '\r': case ' ': break;
            default:
                match[mp++] = c;
                
                if(!try_match(match)) {
                    fprintf(stderr, "\033[33mDebug: tried matching '%s'.\033[37m\n", match);
                } else {
                    fprintf(stderr, "\033[33mDebug: matched '%s'.\033[37m\n", match);
                    memset(match, 0, 32);
                    mp = 0;
                }
        }
    }
    
    puts("}");
}
