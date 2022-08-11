%{
/* C++ string header, for string ops below */
#include <string>
#include <sstream>
#include <iostream>

/* Implementation of yyFlexScanner */
#include "../include/scanner.h"
#undef  YY_DECL
#define YY_DECL int piranha::Scanner::yylex( piranha::Parser::semantic_type *const lval, piranha::Parser::location_type *location)

/* using "token" to make the returns for the tokens shorter to type */
using token = piranha::Parser::token;

/* define yyterminate as this instead of NULL */
#define yyterminate() return( token::END )

/* msvc2010 requires that we exclude this header file. */
#define YY_NO_UNISTD_H

/* update location on matching */
#define YY_USER_ACTION count();

#define DEFAULT_BUILD(location) buildValue<std::string>(yytext, (location), true)
#define BUILD_INVALID(location) buildValue<std::string>(yytext, (location), false)

%}

/* This is to avoid macro redefinition warnings */
%top{
#include <stdint.h>
}

%option debug
%option nodefault
%option yyclass="piranha::Scanner"
%option noyywrap
%option c++

D                   [0-9]
L                   [a-zA-Z_]
H                   [a-fA-F0-9]

%%
%{                  /** Code executed at the beginning of yylex **/
                    m_yylval = lval;
%}

"import"            { DEFAULT_BUILD(location); return (token::IMPORT); }
"as"                { DEFAULT_BUILD(location); return (token::AS); }
"input"             { DEFAULT_BUILD(location); return (token::INPUT); }
"output"            { DEFAULT_BUILD(location); return (token::OUTPUT); }
"modify"            { DEFAULT_BUILD(location); return (token::MODIFY); }
"toggle"            { DEFAULT_BUILD(location); return (token::TOGGLE); }
"alias"             { DEFAULT_BUILD(location); return (token::ALIAS); }
"inline"            { DEFAULT_BUILD(location); return (token::INLINE); }
"auto"              { DEFAULT_BUILD(location); return (token::AUTO); }
"node"              { DEFAULT_BUILD(location); return (token::NODE); }
"public"            { DEFAULT_BUILD(location); return (token::PUBLIC); }
"private"           { DEFAULT_BUILD(location); return (token::PRIVATE); }
"operator"          { DEFAULT_BUILD(location); return (token::OPERATOR); }
"module"            { DEFAULT_BUILD(location); return (token::MODULE); }
"@"                 { DEFAULT_BUILD(location); return (token::DECORATOR); }
"true"              {
                        buildValue<bool>(true, location);
                        return (token::BOOL);
                    }
"false"             {
                        buildValue<bool>(false, location);
                        return (token::BOOL);
                    }
{L}({L}|{D})*       {
                        DEFAULT_BUILD(location);
                        return (token::LABEL);
                    }
0[xX]{H}+           {
                        unsigned int value;
                        value = std::stoul(yytext, nullptr, 16);

                        buildValue<int>((int)value, location);
                        return (token::INT);
                    }
{D}+                {
                        unsigned int value;
                        value = std::stoul(yytext, nullptr, 10);

                        buildValue<int>((int)value, location);
                        return (token::INT);
                    }
{D}+"."{D}+         {
                        double value;
                        value = std::stod(yytext);

                        buildValue<double>(value, location);
                        return (token::FLOAT);
                    }
\"(\\.|[^\\"\n])*\" {
                        // String literal
                        size_t length = strlen(yytext);
                        std::string str(yytext + 1, length - 2);

                        buildValue<std::string>(str, location);

                        return (token::STRING);
                    }

"("                 { DEFAULT_BUILD(location); return ('('); }
")"                 { DEFAULT_BUILD(location); return (')'); }
"["                 { DEFAULT_BUILD(location); return ('['); }
"]"                 { DEFAULT_BUILD(location); return (']'); }
"{"                 { DEFAULT_BUILD(location); return ('{'); }
"}"                 { DEFAULT_BUILD(location); return ('}'); }
":"                 { DEFAULT_BUILD(location); return (':'); }
";"                 { DEFAULT_BUILD(location); return (';'); }
","                 { DEFAULT_BUILD(location); return (','); }
"^"                 { DEFAULT_BUILD(location); return ('^'); }

"."                 { DEFAULT_BUILD(location); return ('.'); }
"*"                 { DEFAULT_BUILD(location); return ('*'); }
"/"                 { DEFAULT_BUILD(location); return ('/'); }
"+"                 { DEFAULT_BUILD(location); return ('+'); }
"-"                 { DEFAULT_BUILD(location); return ('-'); }

"=>"                { DEFAULT_BUILD(location); return (token::BUILTIN_POINTER); }
"::"                { DEFAULT_BUILD(location); return (token::NAMESPACE_POINTER); }
"="                 { DEFAULT_BUILD(location); return ('='); }

[ \t\v\n\f]         { }
"//"[^\n]*          { }
"/*"                { comment(); }
.                   { BUILD_INVALID(location); return (token::UNRECOGNIZED); }
%%

void piranha::Scanner::comment(void) {
    char c, prev = 0;
    while ((c = yyinput()) != 0) { /* (EOF maps to 0) */
        countChar(c);
        if (c == '/' && prev == '*') return;
        prev = c;
    }

    LexerError("Unterminated comment");
}

void piranha::Scanner::count() {
    step();

    for (int i = 0; yytext[i] != '\0'; i++) {
        char c = yytext[i];
        countChar(c);
    }
}

void piranha::Scanner::countChar(char c) {
    if (c == '\n') {
        m_loc->lineEnd++;
        m_loc->colEnd = 1;
    }
    else if (c == '\t') {
        m_loc->colEnd += 4;
    }
    else {
        m_loc->colEnd++;
    }
}

void piranha::Scanner::step() {
    m_loc->lineStart = m_loc->lineEnd;
    m_loc->colStart = m_loc->colEnd;
}
