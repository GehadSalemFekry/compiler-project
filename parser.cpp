#include "tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>
using namespace std;

int lookahead;

// The BNF grammar for the language
void program();
void declarationList();
void declarationListDash();
void declaration();
void varDeclaration();
void varDeclarationDash();
void typeSpecifier();
void params();
void paramList();
void paramListDash();
void param();
void paramDash();
void compoundStmt();
void statementList();
void statementListDash();
void statement();
void selectionStmt();
void selectionStmtDash();
void iterationStmt();
void assignmentStmt();
void var();
void varDash();
void expression();
void expressionDash();
void relop();
void additiveExpression();
void additiveExpressionDash();
void addop();
void term();
void termDash();
void mulop();
void factor();

void match(int expected);
void syntaxErrorMessage(string message);
void syntaxError(string message, string lookAheadToken, string expectedTokens);
void syntaxErrorToken(int token, int expectedToken);

void program() {
    // 1. program → Program ID {declaration-list statement-list}
    match(PROGRAM);
    match(ID);
    match('{');
    declarationList();
    statementList();
    match('}');
    // match('.'); 
}

void declarationList() {
    // 2. declaration-list → declaration declaration-list'
    declaration();
    declarationListDash();
}

void declarationListDash() {
    // 2'. declaration-list' → declaration declaration-list' | ε
    while (lookahead == INT || lookahead == FLOAT) {
        declaration();
    }
}

void declaration() {
    // 3. declaration → var-declaration
    varDeclaration();
}

void varDeclaration() {
    // 4. var-declaration → type-specifier var-declaration'
    typeSpecifier();
    match(ID);
    varDeclarationDash();
}

void varDeclarationDash() {
    // 4'. var-declaration' → ; | [ NUM ] ;
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
        syntaxErrorMessage("Unexpected Type Specifier");
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
    // 8. param-list → param param-list'
    param();
    paramListDash();
}

void paramListDash() {
    // 8'. param-list' → , param param-list' | ε
    while (lookahead == ',') {
        match(',');
        param();
    }
    // ε represented by doing nothing
}

void param() {
    // 9. param → type-specifier ID param'
    typeSpecifier();
    match(ID);
    paramDash();
}

void paramDash() {
    // 9'. param' → ε | [ ]
    if (lookahead == '[') {
        match('[');
        if (lookahead == ']') {
            match(']');
        } else {
            syntaxErrorToken(lookahead, ']');
        }
    }
}

void compoundStmt() {
    // 10. compound-stmt → { statement-list }
    match('{');
    statementList();
    match('}');
}

void statementList() {
    // 12. statement-list -> statement-list'
    statementListDash();
}

void statementListDash() {
    // 12'. statement-list' -> statement statement-list' | ε
    while (lookahead == ID || lookahead == '{' || lookahead == IF || lookahead == WHILE) {
        statement();
    }
}

void statement() {
    // 13. statement -> assignment-stmt | compound-stmt | selection-stmt | iteration-stmt
    switch (lookahead) {
    case ID:
        assignmentStmt();
        break;
    case '{':
        compoundStmt();
        break;
    case IF:
        selectionStmt();
        break;
    case WHILE:
        iterationStmt();
        break;
    default:
        syntaxErrorMessage("Unexpected statement");
    }
}

void selectionStmt() {
    // 15. selection-stmt -> if ( expression ) statement selection-stmt'
    match(IF);
    match('(');
    expression();
    match(')');
    statement();
    selectionStmtDash();
}

void selectionStmtDash() {
    // 15'. selection-stmt -> else statement | ε
    if (lookahead == ELSE) {
        match(ELSE);
        statement();
    }
}

void iterationStmt() {
    // 16. iteration-stmt -> while ( expression ) statement
    match(WHILE);
    match('(');
    expression();
    match(')');
    statement();
}

void assignmentStmt() {
    // 18. assignment-stmt -> var = expression
    var();
    match('=');
    expression();
}

void var() {
    // 19. var -> ID var'
    match(ID);
    varDash();
}

void varDash() {
    // 19'. var' -> ε | [ expression ]
    if (lookahead == '[') {
        match('[');
        expression();
        match(']');
    }
}

void expression() {
    // 20. expression -> additive-expression expression'
    additiveExpression();
    expressionDash();
}

void expressionDash() {
    // 20'. expression' -> relop additive-expression expression' | ε
    while (lookahead == RELOP) {
        relop();
        additiveExpression();
    }
}

void relop() {
    // 21. relop -> == | != | < | <= | > | >=
    match(RELOP);
}

void additiveExpression() {
    // 22. additive-expression-> term additive-expression'
    term();
    additiveExpressionDash();
}

void additiveExpressionDash() {
    // 22'. additiveExpressionDash -> addop term additiveExpression' | ε
    while (lookahead == '+' || lookahead == '-') {
        addop();
        term();
    }
}

void addop() {
    // 23. addop -> + | -
    if (lookahead == '+') {
        match('+');
    } else if (lookahead == '-') {
        match('-');
    }
}

void term() {
    // 24. term -> factor term'
    factor();
    termDash();
}

void termDash() {
    // 24'. term' -> mulop factor term' | ε
    while (lookahead == '*' || lookahead == '/') {
        mulop();
        termDash();
        factor();
    }
}

void mulop() {
    // 25. mulop -> * | /
    if (lookahead == '*') {
        match('*');
    } else if (lookahead == '/') {
        match('/');
    }
}

void factor() {
    // 26. factor -> (expression) | var | NUM
    switch (lookahead) {
    case '(':
        match('(');
        expression();
        match(')');
        break;
    case ID:
        var();
        break;
    case NUM:
        match(NUM);
        break;
    default:
        syntaxErrorMessage("Unexpected factor");
    }
}

int main() {
    lookahead = yylex(); // Initialize the first token
    program();
    if (lookahead != END_OF_FILE) {
        syntaxErrorToken(lookahead, END_OF_FILE);
    }
    cout << "Parsing completed successfully.\n" << endl;
    // printf("Parsing completed successfully.\n");
}

void match(int expected) {
    if (lookahead == expected) {
        lookahead = yylex();
    } else {
        syntaxErrorToken(lookahead, expected);
    }
}

void syntaxErrorMessage(string message) {
    cout << "Syntax error: " << message << " at line " << yylineno << ", column " << yycolumn << "\n";
    // printf("Syntax error: %s at line %d, column %d\n", message, yylineno, yycolumn);
    exit(1);
}

void syntaxError(string message, string lookAheadToken, string expectedTokens) {
    cout << "Syntax error: " << message << " at line " << yylineno << ", column " << yycolumn << ". Saw '" << lookAheadToken
         << "' but expected: " << expectedTokens << "\n";
    // printf("Syntax error: %s at line %d, column %d. Saw '%s' but expected: %s\n", message, yylineno, yycolumn,
    //        lookAheadToken, expectedTokens);
    exit(1);
}

string getTokenName(int token, string buffer) {
    if (token < 256) {
        // For ASCII tokens, return the character
        buffer = string(1, char(token)) + " [ASCII " + to_string(token) + "]";
        // sprintf(buffer, "%c [ASCII %d]", token, token);
        return buffer;
    } else {
        // For named tokens, return a descriptive name
        switch (token) {
        case PROGRAM:
            return "PROGRAM";
        case ID:
            return "identifier";
        case NUM:
            return "number";
        case RELOP:
            return "relational operator";
        case INT:
            return "INT";
        case FLOAT:
            return "FLOAT";
        case VOID:
            return "VOID";
        case RETURN:
            return "RETURN";
        case IF:
            return "IF";
        case ELSE:
            return "ELSE";
        case WHILE:
            return "WHILE";
        case END_OF_FILE:
            return "end of file";
        case ERROR:
            return "error";
        default:
            buffer = "Unknown Token [" + to_string(token) + "]";
            // sprintf(buffer, "Unknown Token [%d]", token);
            return buffer;
        }
    }
}

void syntaxErrorToken(int token, int expectedToken) {
    string buffer1, buffer2;
    string lookAheadToken = getTokenName(token, buffer1);
    string expectedTokenName = getTokenName(expectedToken, buffer2);
    string buffer = "Unexpected token";
    // sprintf(buffer, "Unexpected token");
    syntaxError(buffer, lookAheadToken, expectedTokenName);
}