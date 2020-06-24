
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * chomp(char * s) {
	while(isspace(*s))
		s++;
	
	return s;
}

char * allowed_instructions[] = {
    "sto", "amp", "smp", "cst", "cam",
    "csm", "cot", "ots", "spt", NULL
};

void virtual_call(char * text) {
	text = chomp(chomp(text)+6);
    
    /* text now contains the entire linetext. */
    /* now, parse the instruction. */
    
    for(int i = 0; allowed_instructions[i]; i++) {
        if(!strncmp(text, allowed_instructions[i], strlen(allowed_instructions[i]))) {
            text += strlen(allowed_instructions[i]);
            text = chomp(text);
            
            if(!isdigit(*text) && *text != '.') {
                fprintf(stderr, "Error: vxcall not required for `%s'.", text);
                exit(1);
            }
            
            printf("\nmov f3, ");
            
            if(*text == '.') {
                text++;
                printf("%d", *text);
            } else {
                while(isdigit(*text)) {
                    putchar(*text);
                    text++;
                }
            }
            
            /* we are at the comma now. */
            
            printf("\n%s f3%s\n", allowed_instructions[i], text);
            return;
        }
    }
    
    fprintf(stderr, "Error: this vxcall is not supported: `%s'\n", text);
    exit(1);
}

int yywrap(void) { return 1; }

int main(void) {
	yylex();
}

%}

%%
^[\t ]*vxcall[\t ]+.*$ { virtual_call(yytext); }
. { putchar(yytext[0]); }
%%
