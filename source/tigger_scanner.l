%{
#define YYSTYPE void*
#include <iostream>
#include <string>
#include <cstring>
#include "xcw_parser.tab.h"
using namespace std;
int charNum = 1;
%}
%option yylineno

digit [0-9]
letter_ [A-Za-z_]
ident {letter_}({letter_}|{digit})*
fname f_{ident}
label l{digit}+

%%
[\040\t]    {charNum += yyleng;}
[\n\r]  {charNum = 1;}
if      {charNum += yyleng; return IF;}
return  {charNum += yyleng; return RETURN;}
var     {charNum += yyleng; return VAR;}
goto    {charNum += yyleng; return GOTO;}
call    {charNum += yyleng; return CALL;}
param   {charNum += yyleng; return PARAM;}
end     {charNum += yyleng; return END;}
{label}   {
    charNum += yyleng;
    yylval = new string(yytext);
    return LABEL;
}
[Ttp]{digit}+   {
    charNum += yyleng;
    yylval = new string(yytext);
    return IDENT;
}
{fname}     {
    charNum += yyleng;
    yylval = new string(yytext);
    return FUNC;
}
{digit}+    {
    charNum += yyleng;
    yylval = new int(strtol(yytext, nullptr, 10));
    return NUM;
}
"-"{digit}+ {
    charNum += yyleng;
    yylval = new int(strtol(yytext, nullptr, 10));
    return NUM;
}
":"     {++charNum; return COLON;}
"+"     {++charNum; yylval = new string("+");return ADD;}
"-"     {++charNum; yylval = new string("-");return SUB;}
"*"     {++charNum; yylval = new string("*");return MUL;}
"/"     {++charNum; yylval = new string("/");return DIV;}
"<"     {++charNum; yylval = new string("<");return LE;}
"<="    {charNum += 2; yylval = new string("<=");return LEQ;}
">"     {++charNum; yylval = new string(">");return GE;}
">="    {charNum += 2; yylval = new string(">=");return GEQ;}
"=="    {charNum += 2; yylval = new string("==");return EQ;}
"!="    {charNum += 2; yylval = new string("!=");return NEQ;}
"="     {++charNum; yylval = new string("=");return ASSIGN;}
"["     {++charNum; return LBRAC;}
"]"     {++charNum; return RBRAC;}
"&&"    {charNum += 2; yylval = new string("&&");return AND;}
"||"    {charNum += 2; yylval = new string("||");return OR;}
"!"     {++charNum; yylval = new string("!"); return NOT;}
"%"     {++charNum; yylval = new string("%");return MOD;}

"//".*$ {/* Single line comment */}
%%

int yywrap() {
    return 1;
}