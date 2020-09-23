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
%token KEYWORD_IN KEYWORD_STACK KEYWORD_ADDRESS

%token COLON SLASH DOLLAR AT HASH ATTRIBUTE_OPEN ATTRIBUTE_CLOSE
%token BLOCK_CLOSE BLOCK_OPEN PAREN_OPEN PAREN_CLOSE MINUS ASSIGN EQUALS
%token PLUSEQUALS PLUS

%type<ast> TopLevelScope ProgramDeclaration ModuleDeclaration Address
%type<ast> DataDeclaration ImperativeDeclarations RegisterReference
%type<ast> ImperativeDeclaration RValue Expression RegisterList

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

RegisterReference
: DOLLAR CONSTANT {
    $$ = node(RegisterReference, NULL, NULL, $2);
}
;

RValue
: CONSTANT {
    $$ = node(ConstantWrapper, NULL, NULL, $1);
}
| RegisterReference {
    $$ = $1;
}
;

Expression
: RValue MINUS Expression {
    $$ = node(MinusExpression, append_brother($1, $3), NULL, NULL);
}
| RValue PLUS Expression {
    $$ = node(PlusExpression, append_brother($1, $3), NULL, NULL);
}
| RValue {
    $$ = $1;
}
;

RegisterList
: RegisterList RegisterReference {
    $$ = append_brother($1, $2);
}
| RegisterReference {
    $$ = $1;
}
;

ImperativeDeclaration
: KEYWORD_RET {
    $$ = node(ImperativeReturn, NULL, NULL, NULL);
}
| KEYWORD_END {
    $$ = node(ImperativeExit, NULL, NULL, NULL);
}
| RegisterReference ASSIGN Expression {
    $$ = node(ImperativeAssign, append_brother($1, $3), NULL, NULL);
}
| RegisterReference PLUSEQUALS RValue {
    $$ = node(ImperativeAssignAdd, append_brother($1, $3), NULL, NULL);
}
| KEYWORD_PRESERVE BLOCK_OPEN RegisterList[regs] BLOCK_CLOSE KEYWORD_IN BLOCK_OPEN ImperativeDeclarations[code] BLOCK_CLOSE {
    $$ = node(Preserve, append_brother(node(RegisterList, $regs, NULL, NULL), $code), NULL, NULL);
}
| IDENTIFIER SLASH CONSTANT BLOCK_OPEN ImperativeDeclarations BLOCK_CLOSE {

}
| IDENTIFIER SLASH CONSTANT
;

ImperativeDeclarations
: ImperativeDeclarations ImperativeDeclaration {
    $$ = append_brother($1, $2);
}
| %empty {
    $$ = NULL;
}
;

ModuleDeclaration
: ModuleDeclaration KEYWORD_DATA BLOCK_OPEN DataDeclaration[decl] BLOCK_CLOSE {
    $$ = append_brother($1, $decl);
}
| ModuleDeclaration KEYWORD_FN IDENTIFIER[name] SLASH CONSTANT[arity] BLOCK_OPEN ImperativeDeclarations[decl] BLOCK_CLOSE {
    $$ = append_brother($1, node(InternalFunctionDeclaration,
        append_brother(
            node(FunctionArityNode, NULL, NULL, $arity),
            $decl
        ),
    NULL, $name));
}
| ModuleDeclaration KEYWORD_PUBLIC KEYWORD_FN IDENTIFIER[name] SLASH CONSTANT[arity] BLOCK_OPEN ImperativeDeclarations[decl] BLOCK_CLOSE {
    $$ = append_brother($1, node(ExternalFunctionDeclaration,
        append_brother(
            node(FunctionArityNode, NULL, NULL, $arity),
            $decl
        ),
    NULL, $name));
}
| %empty {
    $$ = NULL;
}
;

DataDeclaration
: DataDeclaration IDENTIFIER COLON CONSTANT {
    $$ = append_brother($1, node(DataDeclaration, node(DataName, NULL, NULL, $2), NULL, $4));
}
| DataDeclaration IDENTIFIER COLON STRING {
    $$ = append_brother($1, node(DataDeclaration, node(DataName, NULL, NULL, $2), NULL, $4));
}
| DataDeclaration KEYWORD_DISCARD COLON CONSTANT {
    $$ = append_brother($1, node(DataDeclaration, NULL, NULL, $4));
}
| DataDeclaration KEYWORD_DISCARD COLON STRING {
    $$ = append_brother($1, node(DataDeclaration, NULL, NULL, $4));
}
| DataDeclaration KEYWORD_PUBLIC IDENTIFIER COLON CONSTANT {
    $$ = append_brother($1, node(ExternalDataDeclaration, node(DataName, NULL, NULL, $3), NULL, $5));
}
| DataDeclaration KEYWORD_PUBLIC IDENTIFIER COLON STRING {
    $$ = append_brother($1, node(ExternalDataDeclaration, node(DataName, NULL, NULL, $3), NULL, $5));
}
| %empty {
    $$ = NULL;
}
;

ProgramDeclaration
: ProgramDeclaration KEYWORD_STACK COLON CONSTANT {
    $$ = append_brother($1, node(StackSize, NULL, NULL, $4));
}
| ProgramDeclaration KEYWORD_ADDRESS COLON Address {
    $$ = append_brother($1, node(ProgramOrigin, $4, NULL, NULL));
}
| %empty {
    $$ = NULL;
}
;

Address
: CONSTANT[seg] AT CONSTANT[off] {
    $$ = node(Address, append_brother(
        node(Segment, NULL, NULL, $seg),
        node(Offset, NULL, NULL, $off)
    ), NULL, NULL);
}
| AT CONSTANT[off] {
    $$ = node(Address,
        node(Offset, NULL, NULL, $off)
    , NULL, NULL);
}
| CONSTANT[seg] AT {
    $$ = node(Address,
        node(Segment, NULL, NULL, $seg)
    , NULL, NULL);
}
| AT {
    $$ = node(Address, NULL, NULL, NULL);
}
;

%%

void yyerror(YYLTYPE * yyllocp, yyscan_t unused, const char * msg) {
    (void) unused;

    fprintf(stderr, "[%d:%d] %s\n", yyllocp->first_line, yyllocp->first_column, msg);
}
