%{
#include <string>
#include "AST.h"
#include "parser.tab.h"

extern "C" int yywrap() { return 1; }
int line_number = 1;
%}

%%

[ \t]+          ; /* 忽略空白字符 */
\n              { line_number++; }

"local"         { return LOCAL; }
"if"            { return IF; }
"then"          { return THEN; }
"else"          { return ELSE; }
"elseif"        { return ELSEIF; }
"while"         { return WHILE; }
"do"            { return DO; }
"repeat"        { return REPEAT; }
"until"         { return UNTIL; }
"function"      { return FUNCTION; }
"end"           { return END; }
"return"        { return RETURN; }
"nil"           { return NIL; }
"and"           { return AND; }
"or"            { return OR; }
"not"           { return NOT; }

"~="            { return NE; }
"<="            { return LE; }
">="            { return GE; }
".."            { return CONC; }

[0-9]+\.[0-9]+ { yylval.number = atof(yytext); return NUMBER; }
[0-9]+         { yylval.number = atof(yytext); return NUMBER; }

\"[^\"]*\"     { 
                yylval.string = strdup(yytext + 1);
                yylval.string[strlen(yylval.string) - 1] = 0;
                return STRING; 
              }
\'[^\']*\'     { 
                yylval.string = strdup(yytext + 1);
                yylval.string[strlen(yylval.string) - 1] = 0;
                return STRING; 
              }

[a-zA-Z_][a-zA-Z0-9_]* { yylval.string = strdup(yytext); return IDENTIFIER; }

[-+*/%^<>=,;(){}[\]] { return yytext[0]; }

.               { printf("Unknown character: %s\n", yytext); }

%% 