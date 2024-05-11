#include "tokens.h"
#include <bits/stdc++.h>
using namespace std;

int lookahead;

// The BNF grammar for the language
void program();
void declarationList();
void declarationListDash();
void declaration();
void varDeclaration(string &var_type, int &var_value);
void varDeclarationDash(string &var_type, string &id);
void typeSpecifier(string &type);
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
void assignmentStmt(string &type, int &value);
void var(string &id, string &type, int &value);
void varDash(string &id, string &type, int &value);
void expression(string &type, int &value);
void expressionDash(string &type, int &value);
void relop();
void additiveExpression(string &type, int &val);
void additiveExpressionDash(string &type, int &val);
void addop();
void term(string &type, int &val);
void termDash(string &type, int &val);
void mulop();
void factor(string &type, int &val);

void match(int expected);
void syntaxErrorMessage(string message);
void syntaxError(string message, string lookAheadToken, string expectedTokens);
void syntaxErrorToken(int token, int expectedToken);
string getTokenName(int token, string buffer);
void semanticErrorMessage(string message);

struct symbolTableEntry {
    string name;
    string type;
    int value;
    int declaration_line_number;
};

map<string, symbolTableEntry> symbolTable;

void program() {    
    // 1. program → Program ID {declaration-list statement-list}
    match(PROGRAM);
    symbolTable[yytext] = {yytext, "Program", 0, yylineno};
    match(ID);
    match('{');
    declarationList();
    statementList();
    match('}');
    match('.');
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
    string var_type;
    int var_value;
    varDeclaration(var_type, var_value);
}

void varDeclaration(string &var_type, int &var_value) {
    // 4. var-declaration → type-specifier var-declaration'

    string type;
    typeSpecifier(type);
    var_type = type;
    var_value = 0;

    string id = yytext;
    match(ID);
    // Check if the variable is already declared
    if (symbolTable.count(id) > 0) {
        semanticErrorMessage("The variable " + id + " is already declared at line " +
                             to_string(symbolTable[id].declaration_line_number));
    }
    if (symbolTable.count(id + "[0]") > 0) {
        semanticErrorMessage("The variable " + id + " is already declared at line " +
                             to_string(symbolTable[id + "[0]"].declaration_line_number));
    }
    symbolTable[id] = {id, type, var_value, yylineno};
    // TODO: what if the variable is an array?
    varDeclarationDash(var_type, id);
}

void varDeclarationDash(string &var_type, string &id) {
    // 4'. var-declaration' → ; | [ NUM ] ;
    if (lookahead == '[') {
        match('[');
        int num = stoi(yytext);
        match(NUM);
        match(']');

        if (num <= 0) {
            semanticErrorMessage("Array size should be greater than 0");
        }
        symbolTable.erase(id);
        for (int i = 0; i < num; i++) {
            symbolTable[id + "[" + to_string(i) + "]"] = {id + "[" + to_string(i) + "]", var_type, 0, yylineno};
        }
    }
    match(';');
}

void typeSpecifier(string &type) {
    // 5. type-specifier → int | float
    if (lookahead == INT) {
        match(INT);
        type = "INT";
    } else if (lookahead == FLOAT) {
        match(FLOAT);
        type = "FLOAT";
    } else {
        syntaxErrorMessage("Unexpected Type Specifier");
    }
}

// TODO: not using in the grammar at all.
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

    string type;
    typeSpecifier(type);
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
    string type;
    int value;

    switch (lookahead) {
    case ID:
        assignmentStmt(type, value);
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
    string type;
    int value;

    match(IF);
    match('(');
    expression(type, value);
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
    string type;
    int value;

    match(WHILE);
    match('(');
    expression(type, value);
    match(')');
    statement();
}

void assignmentStmt(string &type, int &value) {
    // 18. assignment-stmt -> var = expression
    string left_type, right_type;
    int left_value, right_value;
    string id = yytext; // if the variable is not declared, we will get an error in the var function 
    // TODO: if we don't exit after the first error, we need to consider the case where the variable is not declared here also
    var(id, left_type, left_value);
    match('=');
    expression(right_type, right_value);

    if (left_type == right_type) {
        type = left_type;
        value = right_value;
        symbolTable[id].value = value;
        cout << "Value of " << id << " is " << value << endl;
    } else {
        semanticErrorMessage("Type mismatch in assignment statement");
    }
}

void var(string &id, string &type, int &value) {
    // 19. var -> ID var'
    id = yytext;
    match(ID); 
    
    varDash(id, type, value);

    if (symbolTable.count(id) == 0) {
        semanticErrorMessage("The variable " + id + " is not declared");
    } else {
        type = symbolTable[id].type;
        value = symbolTable[id].value;
    }
}

void varDash(string &id, string &type, int &value) {
    // 19'. var' -> ε | [ expression ]
    if (lookahead == '[') {
        match('[');
        string exp_type;
        int exp_val;
        expression(exp_type, exp_val);
        match(']');

        id += "[" + to_string(exp_val) + "]";
    }
}

void expression(string &type, int &value) {
    // 20. expression -> additive-expression expression'

    string add_exp_type, exp_dash_type;
    int add_exp_val, exp_dash_val;
    additiveExpression(add_exp_type, add_exp_val);

    if (lookahead == RELOP) {
        expressionDash(exp_dash_type, exp_dash_val);
        // TODO
    } else {
        type = add_exp_type;
        value = add_exp_val;
    }
}

void expressionDash(string &type, int &value) {
    // 20'. expression' -> relop additive-expression expression' | ε
    while (lookahead == RELOP) {
        relop();

        string add_exp_type;
        int add_exp_val;
        additiveExpression(add_exp_type, add_exp_val);
    }
}

void relop() {
    // 21. relop -> == | != | < | <= | > | >=
    match(RELOP);
}

void additiveExpression(string &type, int &val) {
    // 22. additive-expression-> term additive-expression'
    string term_type;
    int term_val;
    string add_exp_dash_type;
    int add_exp_dash_val;
    term(term_type, term_val);

    if (lookahead == '+' || lookahead == '-') {
        additiveExpressionDash(add_exp_dash_type, add_exp_dash_val);

        if (term_type == add_exp_dash_type) {
            type = term_type;
        } else {
            semanticErrorMessage("Type mismatch in additive expression");
        }
    } else {
        type = term_type;
        val = term_val;
    }
    // val = term_val + add_exp_dash_val;
}

void additiveExpressionDash(string &type, int &val) {
    // 22'. additiveExpressionDash -> addop term additiveExpression' | ε

    string res = "";
    while (lookahead == '+' || lookahead == '-') {
        char op = lookahead;
        addop();

        string term_type;
        int term_val;
        term(term_type, term_val);
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

void term(string &type, int &val) {
    // 24. term -> factor term'

    string factor_type, term_dash_type;
    int factor_val, term_dash_val;
    factor(factor_type, factor_val);

    if (lookahead == '*' || lookahead == '/') {
        termDash(term_dash_type, term_dash_val);

        if (factor_type == term_dash_type) {
            type = factor_type;
        } else {
            semanticErrorMessage("Type mismatch in term");
        }
    } else {
        type = factor_type;
        val = factor_val;
    }
}

void termDash(string &type, int &val) {
    // 24'. term' -> mulop factor term' | ε
    while (lookahead == '*' || lookahead == '/') {
        mulop();

        string factor_type, term_dash_type;
        int factor_val, term_dash_val;
        termDash(term_dash_type, term_dash_val);
        factor(factor_type, factor_val);

        if (factor_type == term_dash_type) {
            type = factor_type;
        } else {
            semanticErrorMessage("Type mismatch in term'");
        }
    }

    // TODO: Implement the logic for multiplication and division, evaluate and assign the value to val
}

void mulop() {
    // 25. mulop -> * | /
    if (lookahead == '*') {
        match('*');
    } else if (lookahead == '/') {
        match('/');
    }
}

void factor(string &type, int &val) {
    // 26. factor -> (expression) | var | NUM

    string id;
    string child_type;
    int child_val;
    switch (lookahead) {
    case '(':
        match('(');
        expression(child_type, child_val);
        match(')');
        break;
    case ID:
        var(id, child_type, child_val);
        break;  
    case NUM:
        val = atoi(yytext);
        // TODO: accomodate for float (in all functions)
        match(NUM);
        // TODO: Assign the value to val
        // TODO: Assign the type to type
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

    // Print the symbol table
    cout << "Symbol Table: " << endl;
    for (auto &[key, entry] : symbolTable) {
        cout << "Name: " << entry.name << ", Type: " << entry.type << ", Value: " << entry.value
             << ", Declaration Line: " << entry.declaration_line_number << endl;
    }

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
    exit(1);
}

void semanticErrorMessage(string message) {
    cout << "Semantic error at line " << yylineno << ", col " << yycolumn << ": " << message << "\n";
    // TODO: Should we exit after the first error or not?
    // exit(1);
}

void syntaxError(string message, string lookAheadToken, string expectedTokens) {
    cout << "Syntax error: " << message << " at line " << yylineno << ", column " << yycolumn << ". Saw '"
         << lookAheadToken << "' but expected: " << expectedTokens << "\n";
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