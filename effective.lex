
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * chomp(char * s) {
	while(isspace(*s))
		s++;
	
	return s;
}

char * chomp_num(char * s) {
	while(isdigit(*s))
		s++;
	
	return s;
}

void effective_addr(char * text) {
	char * orig, * addr;
	int disp, base, index, scale;
	
	orig = text;
	
	while(*text) {
		if(!isdigit(*text)) {
			text++;
			continue;
		}
		
		do text++; while(isdigit(*text));
		while(isspace(*text)) text++;
		
		if(*text == '(')
			break;
	}
	
	if(*text == 0) {
		fprintf(stderr, "error: malformed effective adress.\n");
		exit(1);
	}
	
	text--;
	
	/* now, we're at the last digit of a string. */
	
	while(isspace(*text)) text--;
	while(isdigit(*text)) text--;
	
	text++;
	
	addr = malloc(strlen(text));
	strcpy(addr, text);
	
	*text = 0;
	
	/* step 1: read DISP `2 ( r2 , r3 , 4 )` */
	disp = atoi(addr);
	
	/* step 2: remove numerics ` ( r2 , r3 , 4 )`*/
	addr = chomp_num(addr);
	
	/* step 3: remove whitespace `( r2 , r3 , 4 )`*/
	addr = chomp(addr);
	
	/* step 4: remove the paren ` r2 , r3 , 4 )`*/
	addr++;
	
	/* step 5: remove whitespace `r2 , r3 , 4 )`*/
	addr = chomp(addr);
	
	/* step 6: read register / number for base */
	addr++; /* skip r */
	base = atoi(addr);
	
	addr++;
	
	/* step 7: remove numerics and whitespace */
	
	addr = chomp(chomp_num(addr));
	
	/* step 8: remove comma ` r3 , 4 )` */
	
	addr++;
	
	/* step 9: remove whitespace */
	
	addr = chomp(addr);
	
	/* step 10: read register/number for index `r3 , 4 )` */
	
	addr++; /* skip r */
	index = atoi(addr);
	
	addr++;
	
	addr = chomp(chomp_num(addr));
	
	/* step 11: remove comma and whitespace `, 4 )` */
	
	addr++;
	
	addr = chomp(addr);
	
	/* step 12: read scale `4 )` */
	
	scale = atoi(addr);
	
	/* step 13: construct the operation */
	
	/*
	
	2(r2,r3,4) =>
	  mov f2, r3
	  mul f2, 4
	  add f2, r2
	  add f2, 2
	
	*/
	
	/* todo: optimize output, more test cases, what about some obscure edge cases? */
	
    if(index > 0)
        printf("mov f2, r%d\n", index);
    else
        printf("clr f2\n");
    
    if(scale != 1)
        printf("mul f2, %d\n", scale);
    
    if(base > 0)
        printf("add f2, r%d\n", base);
    
    if(disp > 0)
        printf("add f2, %d\n", disp);
    
	printf("%sf2\n", orig);
}

int yywrap(void) { return 1; }

int main(void) {
	yylex();
}

%}

%%


^[A-Za-z0-9 \t\,]+([0-9]+[ \t]*\([ \t]*r[0-9]+[ \t]*,[ \t]*r[0-9]+[ \t]*,[ \t]*[0-9]+[ \t]*\)) { effective_addr(yytext); }
. { putchar(yytext[0]); }
%%
