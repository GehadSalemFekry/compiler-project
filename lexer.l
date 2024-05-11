%{
#include "tokens.h"
int is_comment = 0; 
int yycolumn = 1;
int update_col_return(char* text, int ret);

extern "C" int yywrap() {
    return 1;
}
%}

%x IN_COMMENT
letter      [a-zA-Z]
digit       [0-9]
notletterdigit [^a-zA-Z0-9 ]
comment     "/*"
special     [+|\-|*|/|=|;|,|.|(|)|\[|\]|{|}]

num (({digit}+)|({digit}+)"."({digit}*))(("E"|"e")(("+"|"-")?)({digit}+))?  
wrong_num (({digit}+)|({digit}+)"."({digit}*))(("E"|"e")(("+"|"-")?))
id {letter}({letter}|{digit})*(("."|"#"|"$"|"_")?({letter}|{digit})+)?
wrong_id {letter}({letter}|{digit})*("."|"#"|"$"|"_")({notletterdigit}+)


%%
"else"                              { return update_col_return(yytext, ELSE); }
"if"                                { return update_col_return(yytext, IF); }
"int"                               { return update_col_return(yytext, INT); }
"return"                            { return update_col_return(yytext, RETURN); }
"void"                              { return update_col_return(yytext, VOID); }
"float"                             { return update_col_return(yytext, FLOAT); }
"while"                             { return update_col_return(yytext, WHILE); }
"Program"                           { return update_col_return(yytext, PROGRAM); }
{special}                           { return update_col_return(yytext, *yytext); } // return the ASCII character itself

"<="                                { return update_col_return(yytext, RELOP); }
"<"                                 { return update_col_return(yytext, RELOP); }
">="                                { return update_col_return(yytext, RELOP); }
">"                                 { return update_col_return(yytext, RELOP); }
"=="                                { return update_col_return(yytext, RELOP); }
"!="                                { return update_col_return(yytext, RELOP); }

{num}                               { return update_col_return(yytext, NUM); } 
{id}                                { return update_col_return(yytext, ID); }
{comment}                           { yycolumn += strlen(yytext); is_comment = 1; BEGIN(IN_COMMENT); }
[ \t]                               { yycolumn += strlen(yytext); } // ignore whitespace
\n                                  { ++yylineno; yycolumn = 1; /* printf("\n"); */ }
{wrong_num}                         { printf("ERR: Invalid number format after exponential notation at line %d, column %d\n", yylineno, yycolumn); }
{wrong_id}                          { printf("ERR: Invalid identifier format at line %d, column %d\n", yylineno, yycolumn); }
.                                   { printf("ERR: Unknown character %c at line %d, column %d\n", *yytext, yylineno, yycolumn); 
                                    ++yycolumn; }
<<EOF>>                             { return END_OF_FILE; }
<IN_COMMENT>{
  "*/"                              { yycolumn += strlen(yytext); is_comment = 0; BEGIN(0); }
  "\n"                              { ++yylineno; yycolumn = 1; }
  [^*]+                             { yycolumn += strlen(yytext);}
  "*"                               { yycolumn += strlen(yytext);}
}
%%

// int main() {
//     yyin = fopen("testcase.txt", "r");
//     if (!yyin) {
//         printf("ERR: opening file failed\n");
//         return 1;
//     }

//     int token;
//     while ((token = yylex()) != END_OF_FILE) {
//     }

//     fclose(yyin);
//     if(is_comment) {
//         printf("ERR: EOF reached while comment is not closed\n");
//     }
//     return 0;
// }

int update_col_return(char* text, int ret) {
    yycolumn += strlen(text);
    // printf("%s ", text);
    return ret;
}
