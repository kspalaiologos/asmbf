
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
unsigned int qp = 0;

struct delta_match suck_delta(void) {
    int c;
    struct delta_match m;
    
    m.coefficient = 0;
    
    if(qp) {
        return queue[--qp];
    }
    
    while(isdigit(c = getchar())) {
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
    }
    
    m.type = c == '<' || c == '>';
    
    fprintf(stderr, "Debug: sucked delta: %d, %d\n", m.coefficient, m.type);
    
    return m;
}

void match_ignore(void) { suck_delta(); }
void match_const(void) { printf("%d", suck_delta().coefficient); }
void match_value(void) { printf("tape[mp] += %d;\n", suck_delta().coefficient); }
void match_mp(void) { printf("mp += %d;\n", suck_delta().coefficient); }
void match_flexy(void) {
    struct delta_match m = suck_delta();
    
    if(m.type == 2) {
        fprintf(stderr, "Severe: Incorrect flexy match type.\n");
        abort();
    }
    
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
                case 'S': getchar(); break;
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
    {"AA", "mp=OFF('b');$I"},
    {"AB", "mp=OFF('b');$I"},
    {"AC", "mp=OFF('a');$I"},
    {"AD", "mp=OFF('c');$I"},
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
    {"BZ", "$M$T++;$S"},
    {"AO", "$Mip=$T;g=0;break;$S"},
    {"AP", "$Mt0=$T;$Sif(t0){$M$Sip=$T;g=0;break;}"},
    {"AQ", "$Mt0=$T;$Sif(!t0){$M$Sip=$T;g=0;break;}"},
    {"AR", "case $C:"},
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
    {"BC", "$Mt1=mp;$S$M$St0=tape[mp+2*sp];sp--;tape[mp=t1]=t0;"},
    {"BD", "$Mt0=$T;$S$M$Stape[mp+2*t0]=t0;sp++;"},
    {"BE", "$Mt2=mp;$S$M$St0=$T;$Mt1=tape[mp+2*t0];mp=t2;$T=t1;"},
    {"BF", "$Mt0=$T;$S$M$St1=$T;$Mtape[mp+2*t0]=t1;"},
    {"BG", "$Mt0=$T;$S$M$S$T-=t0;"},
    {"BH", "$Mt2=mp;$S$M$St3=mp;mp=t2;t0=$T;mp=t3;t1=$t;mp=t2;$T=t0;mp=t3;$T=t1;"},
    {"BI", "$M$S$T=0;"},
    {"BJ", "$M$Sip=tape[mp+2*sp];sp--;g=0;break;"},
    {"BK", "return 0;"},
    {NULL, NULL}
};

int try_match(char buf[]) {
    struct replacement current_row;
    int i = 0;
    
    while((current_row = vm_model[i]).src) {
        if(!strcmp(buf, current_row.src)) {
            fprintf(stderr, "Debug: try_match: matched.\n");
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
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n"
        "#include <stdint.h>\n"
        "#define OFF(x) ((x) - 'a')\n"
        "uint8_t inchar(void) {\n"
            "uint8_t v = getchar();\n"
            "return v < 0 ? 0 : v;\n"
        "}\n"
        "int main(void) {\n"
            "uint16_t*tape=malloc(sizeof(uint16_t)*65536),mp,t0,t1,t2,t3,sp,ip=0,g=1;\n"
            "while(1)\n"
                "switch(ip) {\n"
                "case 0:\n"
                "if(!g)\n"
                    "return 0;\n"
    );
    
    while((c = getchar()) != EOF) {
        switch(c) {
            case '0' ... '9':
                fprintf(stderr, "Debug: found a backfill delta.\n");
                ungetc(c, stdin);
                match_flexy();
                break;
            case '+': printf("tape[mp]++;"); break;
            case '>': printf("mp++;"); break;
            case '-': printf("tape[mp]--;"); break;
            case '<': printf("mp++;"); break;
            case 'Z': case '\n': case '\r': case ' ': break;
            default:
                match[mp++] = c;
                
                if(!try_match(match)) {
                    fprintf(stderr, "Debug: tried matching '%s'.\n", match);
                } else {
                    fprintf(stderr, "Debug: matched '%s'.\n", match);
                    memset(match, 0, 32);
                    mp = 0;
                }
        }
    }
    
    puts("return 0;}}");
}
