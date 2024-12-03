%{
#include <stdio.h>
#include <stdlib.h>

extern int yylex(void);
void yyerror(const char *s);
extern int yylineno; 
extern char *yytext;
%}

%token ID NUM
%token PROGRAM
%token INT FLOAT IF ELSE WHILE 
%token LT LE GT GE EQ NE

%left '+' '-'
%left '*' '/'
%left LE GE LT GT EQ NE

%start program

%%
program : PROGRAM ID '{' declaration_list statement_list '}' '.' ;

declaration_list
    : declaration
    | declaration declaration_list
    /* {
        printf("Declaration list\n");
    } */
    ;

declaration
    : var_declaration
    /* {
        printf("Declaration\n");
    } */
    ;

var_declaration
    : type_specifier ID var_declaration_dash
    /* {
        printf("Variable declaration\n");
    } */
    ;

var_declaration_dash
    : ';'
    | '[' NUM ']' ';'
    /* {
        printf("Variable declaration dash\n");
    } */
    ;

type_specifier
    : INT
    | FLOAT
    /* {
        printf("Type specifier\n");
    } */
    ;

statement_list
    : statement
    | statement statement_list
    /* {
        printf("Statement list\n");
    } */
    ;

statement
    : assignment_stmt
    | compound_stmt
    | selection_stmt
    | iteration_stmt
    /* {
        printf("Statement\n");
    } */
    ;

compound_stmt
    : '{' statement_list '}'
    /* {
        printf("Compound statement\n");
    } */
    ;

selection_stmt
    : IF '(' expression ')' statement selection_stmt_dash
    /* {
        printf("Selection statement\n");
    } */
    ;

selection_stmt_dash
    : ELSE statement
    | /* empty */
    /* {
        printf("Selection statement dash\n");
    } */
    ;

iteration_stmt
    : WHILE '(' expression ')' statement
    /* {
        printf("Iteration statement\n");
    } */
    ;

assignment_stmt
    : var '=' expression
    /* {
        printf("Assignment statement\n");
    } */
    ;

var
    : ID var_dash
    /* {
        printf("Variable\n");
    } */
    ;

var_dash
    : '[' expression ']'
    | /* empty */
    /* {
        printf("Variable dash\n");
    } */
    ;

expression
    : additive_expression
    | additive_expression relop additive_expression
    /* {
        printf("Expression\n");
    } */
    ;

relop
    : LE
    | GE
    | LT
    | GT
    | EQ
    | NE
    /* {
        printf("Relational operator\n");
    } */
    ;

additive_expression
    : term
    | additive_expression addop term
    /* {
        printf("Additive expression\n");
    } */
    ;

addop
    : '+'
    | '-'
    /* {
        printf("Additive operator\n");
    } */
    ;

term
    : factor
    | term mulop factor
    /* {
        printf("Term\n");
    } */
    ;

mulop
    : '*'
    | '/'
    /* {
        printf("Multiplicative operator\n");
    } */
    ;

factor
    : '(' expression ')'
    | var
    | NUM
    /* {
        printf("Factor\n");
    } */
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error at line %d, near '%s': %s\n", yylineno, yytext, s);
    exit(1); // Terminate on first error
}


int main(void) {
    yyparse();

    printf("Parsing successful\n");
    return 0;
}

/* int yylex(void) {
    // Implement lexer or use lex specification
    return 1; // Dummy return
} */
