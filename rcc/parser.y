%define api.pure full
%define api.token.raw
%define parse.error verbose
%locations
%param { yyscan_t scanner }

%code top {
    #include <stdio.h>
    #include <stdlib.h>
    #include "node.h"

    #define node(a, b, c, d) new_node(a, b, c, d, yyloc.first_line, yyloc.first_column)
}

%code requires {
    typedef void * yyscan_t;
}

%union {
    char * string;
    struct node * ast;
}

%code {
    int yylex(YYSTYPE * yylvalp, YYLTYPE * yyllocp, yyscan_t scanner);
    void yyerror(YYLTYPE * yyllocp, yyscan_t unused, const char * msg);
    void dispatch(struct node *);
}

%token END 0 "end of file"

%token<string> CONSTANT IDENTIFIER STRING
%token KEYWORD_PROGRAM KEYWORD_MODULE KEYWORD_DATA KEYWORD_DISCARD
%token KEYWORD_FN KEYWORD_PUBLIC KEYWORD_PRESERVE KEYWORD_END KEYWORD_RET
%token KEYWORD_FUSE KEYWORD_WHILE KEYWORD_DO KEYWORD_WITH KEYWORD_OUT
%token KEYWORD_IN KEYWORD_STACK KEYWORD_ADDRESS KEYWORD_FOR KEYWORD_REF
%token KEYWORD_DEREF

%token COLON SLASH DOLLAR AT HASH ATTRIBUTE_OPEN ATTRIBUTE_CLOSE
%token BLOCK_CLOSE BLOCK_OPEN PAREN_OPEN PAREN_CLOSE MINUS ASSIGN EQUALS
%token PLUSEQUALS PLUS

%type<ast> TopLevelScope ProgramDeclaration ModuleDeclaration Address
%type<ast> DataDeclaration ImperativeDeclarations RegisterReference
%type<ast> ImperativeDeclaration RValue Expression RegisterList Function
%type<ast> FunctionCall RelOp

%start Start

%left MINUS

%%

Start
: Start TopLevelScope     { dispatch($2); }
| %empty
;

TopLevelScope
: KEYWORD_PROGRAM BLOCK_OPEN ProgramDeclaration[decl] BLOCK_CLOSE {
    $$ = node(ProgramDeclaration, $decl, NULL, NULL);
}
| KEYWORD_MODULE BLOCK_OPEN ModuleDeclaration[decl] BLOCK_CLOSE {
    $$ = node(ModuleDeclaration, $decl, NULL, NULL);
}
;

%%

void yyerror(YYLTYPE * yyllocp, yyscan_t unused, const char * msg) {
    (void) unused;

    fprintf(stderr, "[%d:%d] %s\n", yyllocp->first_line, yyllocp->first_column, msg);
}
