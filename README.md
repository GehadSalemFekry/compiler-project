# Running the Recursive-Descent Parser

This document outlines the steps to compile and run the recursive-descent parser which uses a Lex-generated scanner for the C- programming language.

## Prerequisites

Before you start, make sure you have the following installed:
- GCC (GNU Compiler Collection)
- Flex (Fast Lexical Analyzer generator)

## Files Description

- `lexer.l`: Contains the Lex rules for tokenizing the input.
- `parser.c`: Contains the C code for the recursive-descent parser.
- `tokens.h`: Header file with token definitions and extern declarations for shared variables.

## Compilation

To compile the scanner and parser, execute the following commands in your terminal:

```bash
flex -o scanner.cpp lexer.l         # Generate the scanner C source file from the Lex file
g++ -o parser scanner.cpp parser.cpp  # Compile the scanner and parser together into an executable
```

## Running the Parser

To run the parser with an input file `testcase.txt`, use the following command:

```bash
./parser < testcase.txt
```