#ifndef TOKENS_H
#define TOKENS_H

enum TokenType {
    // Token values for reserved words and other tokens
    // that do not have a single character equivalent.
    PROGRAM = 256, // Start enumeration from a value after the ASCII range
    ID,
    NUM,
    RELOP, // For relational operators like ==, !=, <, <=, >, >=

    // Keywords
    INT,
    FLOAT,
    VOID,
    RETURN,
    IF,
    ELSE,
    WHILE,
    END_OF_FILE,
    ERROR
};

extern int yycolumn;
extern int yylineno;
extern char *yytext;
extern int yylex(void);

#endif
