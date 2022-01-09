%{
#define YYSTYPE void*
#include <iostream>
#include <string>
#include <cstring>
#include "xcw_parser.tab.h"
using namespace std;
%}
%option yylineno


%%
[\040\t]    {}
[\n\r]  {}
if      { return IF;}
return  { return RETURN;}
var     { return VAR;}
goto    { return GOTO;}
call    { return CALL;}
param   { return PARAM;}
end     { return END;}

l[0-9]+   {
    yylval = new string(yytext);
    return LABEL;
}
[a-zA-Z_][a-zA-Z0-9_]*   {
    yylval = new string(yytext);
    return IDENT;
}
[0-9]+    {
    yylval = new int(strtol(yytext, nullptr, 10));
    return NUM;
}
"-"[0-9]+ {
    yylval = new int(strtol(yytext, nullptr, 10));
    return NUM;
}

":"     {return COLON;}
"+"     {yylval = new string("+");return ADD;}
"-"     {yylval = new string("-");return SUB;}
"*"     { yylval = new string("*");return MUL;}
"/"     { yylval = new string("/");return DIV;}
"<"     { yylval = new string("<");return LE;}
"<="    { yylval = new string("<=");return LEQ;}
">"     { yylval = new string(">");return GE;}
">="    { yylval = new string(">=");return GEQ;}
"=="    { yylval = new string("==");return EQ;}
"!="    { yylval = new string("!=");return NEQ;}
"="     { yylval = new string("=");return ASSIGN;}
"["     { return LBRAC;}
"]"     { return RBRAC;}
"&&"    { yylval = new string("&&");return AND;}
"||"    { yylval = new string("||");return OR;}
"!"     { yylval = new string("!"); return NOT;}
"%"     { yylval = new string("%");return MOD;}

"//".*$ {/* Single line comment */}
%%

int yywrap() {
    return 1;
}