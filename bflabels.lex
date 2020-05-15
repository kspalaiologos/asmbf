
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct label_t {
	struct label_t * next;
	char * name;
	int id;
};

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
	
	text = strchr(text, '%') + 1;
	head = main_node;
	while(head->next != NULL)
		if(head->name && strcmp(text, head->name) == 0) {
			printf("%d", head->id);
			return;
		} else
			head = head->next;
	
	addlabel(text);
}

void addlabel(char * text) {
	static int curid = 1;
	struct label_t * head;
	char * cpy, surpress_label = 0;
	
	cpy = strchr(text, '@') + 1;
	if(cpy != (void *)1)
		text = cpy;
	else surpress_label = 1;
	
	if(main_node == NULL)
		main_node = alloc_node();
	
	head = main_node;
	while(head->next != NULL) {
		if(head->name && strcmp(text, head->name) == 0) {
			printf("lbl %d\n", head->id);
			return;
		} else head = head->next;
	}
	
	head->next = alloc_node();
	head->name = malloc(strlen(text) + 1);
	strcpy(head->name, text);
	!surpress_label && printf("lbl %d\n", curid);
	surpress_label && printf("%d", curid);
	head->id = curid++;
}

int yywrap(void) { return 1; }

int main(void) {
	yylex();
}

%}

%%
^[ \t]*\@([A-Za-z_][A-Za-z0-9_]*) { addlabel(yytext); }
(%([A-Za-z_][A-Za-z0-9_]*)|"[^"\n]*%([A-Za-z_][A-Za-z0-9_]*)) { getlabel(yytext); }
. { putchar(yytext[0]); }
%%
