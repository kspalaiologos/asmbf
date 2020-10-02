%{
#define _GNU_SOURCE
#include <stdio.h>

#include "error.h"
#include "lexer.h"

#include "parser.tab.h"

// Supply the prototype ourselves.
// Flex doesn't put this code at the top, and probably
// already includes <stdio.h> without GNU extensions.

#ifndef asprintf
    int asprintf(char ** strp, const char * fmt, ...); 
#endif

#define YY_USER_ACTION                          \
  yylloc->first_line = yylloc->last_line;       \
  yylloc->first_column = yylloc->last_column;   \
  if (yylloc->last_line == yylineno)            \
    yylloc->last_column += yyleng;              \
  else                                          \
    yylloc->last_line = yylineno,               \
    yylloc->last_column = yytext                \
      + yyleng - strrchr(yytext, '\n');

void lexer_error(char c, int lineno);

%}

%option noinput nounput noyywrap 8bit nodefault
%option yylineno reentrant bison-bridge
%option bison-locations

%%

program            return KEYWORD_PROGRAM;
module             return KEYWORD_MODULE;
data               return KEYWORD_DATA;
_                  return KEYWORD_DISCARD;
fn                 return KEYWORD_FN;
pub                return KEYWORD_PUBLIC;
preserve           return KEYWORD_PRESERVE;
end                return KEYWORD_END;
ret                return KEYWORD_RET;
fuse               return KEYWORD_FUSE;
while              return KEYWORD_WHILE;
do                 return KEYWORD_DO;
with               return KEYWORD_WITH;
out                return KEYWORD_OUT;
in                 return KEYWORD_IN;
for                return KEYWORD_FOR;
stack              return KEYWORD_STACK;
address            return KEYWORD_ADDRESS;
ref                return KEYWORD_REF;
deref              return KEYWORD_DEREF;

":"                return COLON;
"/"                return SLASH;
"$"                return DOLLAR;
"@"                return AT;
"#"                return HASH;
"[["               return ATTRIBUTE_OPEN;
"]]"               return ATTRIBUTE_CLOSE;
"{"                return BLOCK_OPEN;
"}"                return BLOCK_CLOSE;
"("                return PAREN_OPEN;
")"                return PAREN_CLOSE;
"-"                return MINUS;
"+"                return PLUS;
"="                return ASSIGN;
"=="               return EQUALS;
"+="               return PLUSEQUALS;

"//".*\n           ;

\"(\\.|[^\"])*\" {
	yylval->string = strdup(yytext);
	return STRING;
}

[A-Za-z_][0-9A-Za-z_]* {
	yylval->string = strdup(yytext);
	return IDENTIFIER;
}

[0-9]+ {
	yylval->string = strdup(yytext);
	return CONSTANT;
}

\.. {
    asprintf(&(yylval->string), "%d", yytext[1]);
    return CONSTANT;
}

[\n\r\f\t ]    ;
.|\n           { lexer_error(*yytext, yylineno); exit(1); }

%%

void process_file(const char * name) {
	process_stream(fopen(name, "rb"));
}

void process_stream(FILE * f) {
    yyscan_t yyscanner;

	yylex_init(&yyscanner);
	yyset_in(f, yyscanner);
	yyparse(yyscanner);
	yylex_destroy(yyscanner);
}

void lexer_error(char c, int line) {
	fprintf(stderr, "[%d] Lexer error, unexpected `%c'.", line, c);
}

