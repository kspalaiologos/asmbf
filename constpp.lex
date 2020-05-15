
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

struct def_t {
	char * find, * replace;
	struct def_t * next;
};

struct def_t * ctx, * ctx_ptr;

struct def_t * new_def(void) {
	struct def_t * ret = malloc(sizeof(struct def_t));
	assert(ret);
	return ret;
}

void pop_def(char * text) {
	struct def_t * p = ctx;
	while(p->next && p->find && p->replace) {
		if(!strcmp(text, p->find)) {
			printf("%s", p->replace);
			return;
		}
		
		p = p->next;
	}
	printf("%s", text);
}

void push_def(char * text) {
	/* Skip the whitespace. */
	while(isspace((unsigned char)*text))
		text++;
	
	/* Skip the questionmark */
	text++;
	
	char * find = text, * replace = strchr(text, '=') + 1;
	unsigned find_len = replace - find - 1, replace_len = strlen(replace);
	
	ctx_ptr->find = malloc(find_len);
	memcpy(ctx_ptr->find, find, find_len);
	
	ctx_ptr->replace = malloc(replace_len);
	memcpy(ctx_ptr->replace, replace, replace_len);
	
	ctx_ptr->next = new_def();
	ctx_ptr->next->find = ctx_ptr->next->replace = NULL;
	ctx_ptr = ctx_ptr->next;
}

int yywrap(void) { return 1; }

int main(void) {
	ctx = ctx_ptr = new_def();
	
	yylex();
}

%}

%%
^[ \t]*\?([A-Za-z_][A-Za-z0-9_]*)\=([A-Za-z_][A-Za-z0-9_]*) { push_def(yytext); }
(([A-Za-z_][A-Za-z0-9_]*)|"[^"\n]*([A-Za-z_][A-Za-z0-9_]*)) { pop_def(yytext); }
. { putchar(yytext[0]); }
%%
