#include "tokens.h"
#include <stdio.h>

int lookahead;

// The BNF grammar for the language
void program();
void declarationList();
void declaration();
void varDeclaration();
void typeSpecifier();
void params();
void paramList();
void param();
void compoundStmt();
void statementList();
// expressions 11- 20
void relop();
void additiveExpression();
void additiveExpressionDash();
void addop();
void Term();
void Termdash();
void mulop();
void factor();

void program() {
    // 1. program → Program ID {declaration-list statement-list}
    match(PROGRAM);
    match(ID);
    match('{');
    declarationList();
    statementList();
    match('}');
}

void declarationList() {
    // 2. declaration-list → declaration-list declaration | ε
    // d -> declaration d | ε

    while (lookahead == INT || lookahead == FLOAT) {
        declaration();
    }
    // ε is represented by doing nothing
}

void declaration() {
    // 3. declaration → var-declaration
    varDeclaration();
}

void varDeclaration() {
    // 4. var-declaration → type-specifier ID ; | type-specifier ID [ NUM ] ;
    typeSpecifier();
    match(ID);
    if (lookahead == '[') {
        match('[');
        match(NUM);
        match(']');
    }
    match(';');
}

void typeSpecifier() {
    // 5. type-specifier → int | float
    if (lookahead == INT) {
        match(INT);
    } else if (lookahead == FLOAT) {
        match(FLOAT);
    } else {
        syntaxError("Type specifier expected");
    }
}

void params() {
    // 7. params → param-list | void
    if (lookahead == VOID) {
        match(VOID);
    } else {
        paramList();
    }
}

void paramList() {
    // 8. param-list → param-list , param | param
    param();
    while (lookahead == ',') {
        match(',');
        param();
    }
    // ε represented by doing nothing
}

void param() {
    // 9. param → type-specifier ID | type-specifier ID [ ]
    typeSpecifier();
    match(ID);
    if (lookahead == '[') {
        match('[');
        if (lookahead == ']') {
            match(']');
        } else {
            syntaxError("Expected right bracket");
        }
    }
}

void compoundStmt() {
    // 10. compound-stmt → { statement-list }
    match('{');
    statementList();
    match('}');
}

void relop() {
    match(RELOP);
}
void additiveExpression() {
    // additive-expression-> term additive-expressiondash
    Term();
    additiveExpressionDash();
}

void additiveExpressionDash() {
    // additiveExpressionDash -> addop additiveExpressionDash term | void
    if (lookahead == '+' || lookahead == '-') {
        addop();
        additiveExpressionDash();
        Term();
    } else {
        match(VOID);
    }
}
void addop() {
    if (lookahead == '+') {
        match('+');
    }
    if (lookahead == '-') {
        match('-');
    }
}
void Term() {
    // term -> factor termdash
    factor();
    Termdash();
}
void Termdash() {
    // termdash-> mulop termdash factor | void
    if ()
}
void mulop() {
    if (lookahead == '*') {
        match('*');
    }
    if (lookahead == '/') {
        match('/');
    }
}
void factor() {
}

int main() {
    lookahead = yylex(); // Initialize the first token
    program();
    if (lookahead != END_OF_FILE) {
        syntaxError("End of file expected");
    }
    printf("Parsing completed successfully.\n");
}

void match(int expected) {
    if (lookahead == expected) {
        lookahead = yylex();
    } else {
        syntaxError("Unexpected token");
    }
}

void syntaxError(char *message) {
    printf("Syntax error: %s at line %d, column %d\n", message, yylineno, yycolumn);
}
