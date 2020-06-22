
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct label_t {
	struct label_t * next;
	char * name;
	int id;
};

static int segment, origin;

struct label_t * main_node;

struct label_t * alloc_node(void) {
	struct label_t * x = malloc(sizeof(struct label_t));
	x->next = x->name = NULL;
	x->id = 0;
	return x;
}

void addlabel(char * text);

void getlabel(char * text) {
	struct label_t * head;
	
	if(*text == '"') {
		printf("%s", text);
		return;
	}
	
	if(main_node == NULL)
		main_node = alloc_node();
	
	text = strchr(text, '*') + 1;
	head = main_node;
	while(head->next != NULL)
		if(head->name && strcmp(text, head->name) == 0) {
			printf("%d", head->id);
			return;
		} else
			head = head->next;
	
	fprintf(stderr, "asm2bf: label `%s` not found.", text);
	exit(1);
}

void addlabel(char * text) {
	struct label_t * head;
	
	text = strchr(text, '&') + 1;
	
	if(main_node == NULL)
		main_node = alloc_node();
	
	head = main_node;
	while(head->next != NULL) {
		if(head->name && strcmp(text, head->name) == 0) {
			fprintf(stderr, "asm2bf: label `%s` already exists.\n", head->name);
			exit(1);
		} else head = head->next;
	}
	
	head->next = alloc_node();
	head->name = malloc(strlen(text) + 1);
	strcpy(head->name, text);
	
	#ifndef IGNORE_SEGMENTS
		head->id = segment + origin;
	#else
		head->id = origin;
	#endif
}

int yywrap(void) { return 1; }

int main(void) {
	yylex();
}

%}

%%
^[ \t]*\&([A-Za-z_][A-Za-z0-9_]*) { addlabel(yytext); }
(\*([A-Za-z_][A-Za-z0-9_]*)|\"[^\"\n]*\*([A-Za-z_][A-Za-z0-9_]*)) { getlabel(yytext); }
^[ \t]*db_ { origin++; printf("%s", yytext); }
^[ \t]*txt[ \t]*\".*\" { origin += strlen(strchr(yytext, '"') + 1) - 1; printf("%s", yytext); }
^[ \t]*seg[ \t]*([0-9]+) { segment = atoi(strpbrk(yytext, "0123456789")); printf("%s", yytext); }
^[ \t]*org[ \t]*([0-9]+) { origin = atoi(strpbrk(yytext, "0123456789")); printf("%s", yytext); }
. { putchar(yytext[0]); }
%%
