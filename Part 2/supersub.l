/******************************************************************* 
purpose: Flex file to generate lexical analysis for supersub
 to comple run "flex supersub.l; mv lex.yy.c lexer.cpp"
 *******************************************************************/
%option noyywrap
%{
#include "lexer.h"

int yyLine = 1;
%}

%%
 
 /* Keywords */ 
combinator     { return TOK_COMBINATOR; }
evaluate       { return TOK_EVALUATE; }
dictionary     { return TOK_DICTIONARY; }
set            { return TOK_SET; }

 /* Punctuation */

;         { return TOK_SEMICOLON; }
\(        { return TOK_OPENPAREN; }
\)        { return TOK_CLOSEPAREN; }
\{        { return TOK_OPENBRACE; }
\}        { return TOK_CLOSEBRACE; }


 /* Operators */

\\        { return TOK_SLASH; }
\.        { return TOK_DOT; }
\+        { return TOK_PLUS; }
-         { return TOK_MINUS; }
\*        { return TOK_MULTIPLY; }
\/        { return TOK_DIVIDE; }
:=        { return TOK_ASSIGN; }
==        { return TOK_EQUALTO; }
\<        { return TOK_LESSTHAN; }
>         { return TOK_GREATERTHAN; }

 /* Abstractions */

[A-Za-z][0-9A-Za-z_]*   { return TOK_IDENTIFIER; }
\$[A-Za-z][0-9A-Za-z_]* { return TOK_CIDENTIFIER; }
[0-9]+                  { return TOK_INTLIT ; }
[0-9]+\.?[0-9]*         { return TOK_FLOATLIT; }
\".+\"                  { return TOK_STRINGLIT; }

 /* Eat any whitespace */
[ \t\r]+               /* nop */
 /* Eat Comments and increment line counter */
\#.*\n                  { yyLine++; }
 /* Eat newline characters, AND increment line counter */

[\n]      { yyLine++; }

 /* Found an unknown character */

.         { return TOK_UNKNOWN; }

 /* Recognize end of file */

<<EOF>>   { return TOK_EOF; }


