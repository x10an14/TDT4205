%{
#include "parser.h"
#ifdef DUMP_TOKENS
    #define RETURN(t) do {                                      \
        fprintf ( stderr, "TOKEN ( %d,\t'%s' )\n", t, yytext );    \
        return t;                                               \
    } while ( 0 )
#else
    #define RETURN(t) return t
#endif
%}

%option array
%option noyywrap
%option yylineno

WHITESPACE [\t\n\ ]
COMMENTS \/\/[^\n]*\n
DIGIT [0-9]
LETTER [a-zA-Z_]
STRING \"([^\"\n]|\\\")*\"

%%

FUNC		{ RETURN( FUNC );}
PRINT		{ RETURN( PRINT );}
RETURN		{ RETURN( RETURN );}
CONTINUE	{ RETURN( CONTINUE );}
IF 		{ RETURN( IF );}
THEN		{ RETURN( THEN );}
ELSE		{ RETURN( ELSE );}
FI		{ RETURN( FI );}
WHILE		{ RETURN( WHILE );}
DO		{ RETURN( DO );}
DONE		{ RETURN( DONE );}
FOR		{ RETURN( FOR );}
TO		{ RETURN( TO );}
VAR		{ RETURN( VAR );}
":="		{ RETURN( ASSIGN );}
"<="		{ RETURN( LEQUAL );}
">="		{ RETURN( GEQUAL );}
"=="		{ RETURN( EQUAL );}
"!="		{ RETURN( NEQUAL );}
"*"		{ RETURN( "*" );}
"/"		{ RETURN( "/" );}
"+"		{ RETURN( "+" );}
"-"		{ RETURN( "-" );}
{COMMENTS}	{ RETURN( CONTINUE );}
{DIGIT}+	{ RETURN( NUMBER );}
{LETTER}({LETTER}|{DIGIT})* { RETURN( IDENTIFIER );}
{STRING}	{ RETURN( STRING );}
.           { RETURN( yytext[0] ); }
%%
