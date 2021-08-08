%skeleton "lalr1.cc"
%require "3.2"
%debug
%defines
%define api.namespace {piranha}
%define api.parser.class {Parser}

%code requires {
    namespace piranha {
        class IrCompilationUnit;
        class Scanner;
    }

    #include "../include/ir_compilation_unit.h"
    #include "../include/ir_node.h"
    #include "../include/ir_attribute_list.h"
    #include "../include/ir_attribute.h"
    #include "../include/ir_value.h"
    #include "../include/ir_value_constant.h"
    #include "../include/ir_binary_operator.h"
    #include "../include/ir_import_statement.h"
    #include "../include/ir_token_info.h"
    #include "../include/ir_node_definition.h"
    #include "../include/ir_attribute_definition.h"
    #include "../include/ir_attribute_definition_list.h"
    #include "../include/compilation_error.h"
    #include "../include/ir_structure_list.h"
    #include "../include/ir_visibility.h"
    #include "../include/ir_unary_operator.h"
    #include "../include/memory_tracker.h"

    #include <string>

    #ifndef YY_NULLPTR
    #if defined __cplusplus && 201103L <= __cplusplus
    #define YY_NULLPTR nullptr
    #else
    #define YY_NULLPTR 0
    #endif
    #endif

    # define YYLLOC_DEFAULT(Cur, Rhs, N)                    \
    do                                                      \
        if (N) {                                            \
            (Cur).combine(&YYRHSLOC(Rhs, 1));               \
            (Cur).combine(&YYRHSLOC(Rhs, N));               \
        }                                                   \
        else {                                              \
            (Cur).combine(&YYRHSLOC(Rhs, 0));               \
        }                                                   \
    while (0)

    /* Remove annoying compiler warnings */
    #ifdef _MSC_VER
    /* warning C4065: switch statement contains 'default' but no 'case' labels */
    #pragma warning (disable: 4065)
    #endif
}

%parse-param {Scanner &scanner}
%parse-param {IrCompilationUnit &driver}

%code {
  #include <iostream>
  #include <cstdlib>
  #include <fstream>

  #include "../include/ir_compilation_unit.h"
  #include "../include/scanner.h"

#undef yylex
#define yylex scanner.yylex
}

%define api.value.type variant
%define api.location.type {piranha::IrTokenInfo}
%define parse.assert

%token END 0
%token <piranha::IrTokenInfo_string>    CHAR
%token <piranha::IrTokenInfo_string>    IMPORT
%token <piranha::IrTokenInfo_string>    AS
%token <piranha::IrTokenInfo_string>    NODE
%token <piranha::IrTokenInfo_string>    INLINE
%token <piranha::IrTokenInfo_string>    ALIAS
%token <piranha::IrTokenInfo_string>    INPUT
%token <piranha::IrTokenInfo_string>    OUTPUT
%token <piranha::IrTokenInfo_string>    MODIFY
%token <piranha::IrTokenInfo_string>    TOGGLE
%token <piranha::IrTokenInfo_string>    LABEL
%token <piranha::IrTokenInfo_int>       INT
%token <piranha::IrTokenInfo_float>     FLOAT
%token <piranha::IrTokenInfo_bool>      BOOL
%token <piranha::IrTokenInfo_string>    STRING
%token <piranha::IrTokenInfo_string>    DECORATOR
%token <piranha::IrTokenInfo_string>    PUBLIC
%token <piranha::IrTokenInfo_string>    PRIVATE
%token <piranha::IrTokenInfo_string>    BUILTIN_POINTER
%token <piranha::IrTokenInfo_string>    NAMESPACE_POINTER
%token <piranha::IrTokenInfo_string>    UNRECOGNIZED
%token <piranha::IrTokenInfo_string>    OPERATOR
%token <piranha::IrTokenInfo_string>    MODULE
%token <piranha::IrTokenInfo_string>    AUTO

%token <piranha::IrTokenInfo_string> '='
%token <piranha::IrTokenInfo_string> '+'
%token <piranha::IrTokenInfo_string> '-'
%token <piranha::IrTokenInfo_string> '/'
%token <piranha::IrTokenInfo_string> '*'
%token <piranha::IrTokenInfo_string> '('
%token <piranha::IrTokenInfo_string> ')'
%token <piranha::IrTokenInfo_string> '{'
%token <piranha::IrTokenInfo_string> '}'
%token <piranha::IrTokenInfo_string> '['
%token <piranha::IrTokenInfo_string> ']'
%token <piranha::IrTokenInfo_string> ':'
%token <piranha::IrTokenInfo_string> ';'
%token <piranha::IrTokenInfo_string> ','
%token <piranha::IrTokenInfo_string> '.'
%token <piranha::IrTokenInfo_string> '^'

%type <piranha::IrTokenInfo_string> standard_operator;
%type <piranha::IrTokenInfo_string> type_name;
%type <piranha::IrTokenInfoSet<std::string, 2>> type_name_namespace;
%type <piranha::IrNode *> node;
%type <piranha::IrNode *> node_member;
%type <piranha::IrNodeList *> node_list;
%type <piranha::IrAttributeList *> attribute_list;
%type <piranha::IrAttributeList *> connection_block;
%type <piranha::IrAttribute *> attribute;
%type <piranha::IrValue *> value;
%type <piranha::IrValue *> atomic_value;
%type <piranha::IrValue *> label_value;
%type <piranha::IrNode *> inline_node;
%type <piranha::IrNode *> inline_node_member;
%type <piranha::IrValue *> constant;
%type <piranha::IrValue *> data_access;
%type <piranha::IrValue *> mul_exp;
%type <piranha::IrValue *> add_exp;
%type <piranha::IrValue *> primary_exp;
%type <piranha::IrValue *> unary_exp;
%type <piranha::IrImportStatement *> import_statement;
%type <piranha::IrImportStatement *> import_statement_visibility;
%type <piranha::IrImportStatement *> import_statement_short_name;
%type <piranha::IrTokenInfo_string> string;

%type <piranha::IrNodeDefinition *> node_name;
%type <piranha::IrNodeDefinition *> node_inline;
%type <piranha::IrNodeDefinition *> node_shadow;
%type <piranha::IrNodeDefinition *> node_decorator;
%type <piranha::IrNodeDefinition *> node_definition;
%type <piranha::IrNodeDefinition *> specific_node_definition;
%type <piranha::IrNodeDefinition *> immediate_node_definition;

%type <piranha::IrAttributeDefinition *> port_value;
%type <piranha::IrAttributeDefinition *> port_declaration;
%type <piranha::IrAttributeDefinition *> port_status;
%type <piranha::IrAttributeDefinition *> port_connection;
%type <piranha::IrAttributeDefinition *> documented_port_definition;

%type <piranha::IrAttributeDefinitionList *> port_definitions;

%locations

%%

sdl 
  : END 
  | statement_list END
  ;

decorator
  : DECORATOR LABEL ':' string          { /* void */ }
  ;

decorator_list
  : decorator                           { /* void */ }
  | decorator_list decorator            { /* void */ }
  ;

statement
  : node_member                         { driver.addNode($1); }
  | import_statement_short_name         { driver.addImportStatement($1); }
  | node_decorator                      { driver.addNodeDefinition($1); }
  | MODULE '{' decorator_list '}'       { /* void */ }
  ;

statement_list 
  : statement                           { /* void */ }
  | statement_list statement            { /* void */ }
  | statement_list error                { /* void */ }
  ;

import_statement
  : IMPORT string                       { $$ = TRACK(new IrImportStatement($2)); }
  | IMPORT LABEL                        { 
                                            $$ = TRACK(new IrImportStatement($2));

                                            /* The name is a valid label so it can be used as a short name */
                                            $$->setShortName($2); 
                                        }
  ;

import_statement_visibility
  : PUBLIC import_statement             { $$ = $2; $$->setVisibility(IrVisibility::Public); }
  | PRIVATE import_statement            { $$ = $2; $$->setVisibility(IrVisibility::Private); }
  | import_statement                    { $$ = $1; $$->setVisibility(IrVisibility::Default); }
  ;

import_statement_short_name
  : import_statement_visibility AS LABEL        { $$ = $1; $$->setShortName($3); }
  | import_statement_visibility                 { $$ = $1; }
  ;

type_name
  : LABEL                               { $$ = $1; }
  | OPERATOR standard_operator          {
                                            IrTokenInfo_string info = $1;
                                            info.combine(&$2);
                                            info.data = std::string("operator") + $2.data;
                                            $$ = info;                                        
                                        }
  ;

type_name_namespace
  : type_name                           { 
                                            IrTokenInfoSet<std::string, 2> set; 
                                            set.data[1] = $1; 
                                            $$ = set; 
                                        }
  | LABEL NAMESPACE_POINTER type_name   { 
                                            IrTokenInfoSet<std::string, 2> set; 
                                            set.data[0] = $1; 
                                            set.data[1] = $3; 
                                            $$ = set; 
                                        }
  | NAMESPACE_POINTER type_name         { 
                                            IrTokenInfoSet<std::string, 2> set; 
                                            set.data[0].specified = true; 
                                            set.data[1] = $2; 
                                            $$ = set; 
                                        }
  ;

node
  : type_name_namespace LABEL connection_block          { $$ = TRACK(new IrNode($1.data[1], $2, $3, $1.data[0])); }
  | type_name_namespace connection_block                {
                                                            IrTokenInfo_string name;
                                                            name.specified = false;
                                                            name.data = "";

                                                            $$ = TRACK(new IrNode($1.data[1], name, $2, $1.data[0]));
                                                        }
  ;

node_member
  : node                                                { $$ = $1; }
  | data_access '.' node                                {
                                                            $$ = $3;
                                                            $$->setThis($1);
                                                        }
  ;

node_list
  : node_member                                         {
                                                            $$ = TRACK(new IrNodeList());
                                                            $$->add($1);
                                                        }
  | node_list node_member                               { 
                                                            $$ = $1;
                                                            $1->add($2);  
                                                        }
  | node_list error                                     { $$ = $1; }
  ;

standard_operator
  : '-'                                                 { $$ = $1; }
  | '+'                                                 { $$ = $1; }
  | '/'                                                 { $$ = $1; }
  | '*'                                                 { $$ = $1; }
  ;

node_name
  : NODE LABEL                                          { $$ = TRACK(new IrNodeDefinition($2)); }
  | NODE OPERATOR standard_operator                     {
                                                            IrTokenInfo_string info = $2;
                                                            info.combine(&$3);
                                                            info.data = std::string("operator") + $3.data;
                                                            $$ = TRACK(new IrNodeDefinition(info));
                                                        }
  ;

node_inline
  : INLINE node_name                                    { $$ = $2; $$->setIsInline(true); }
  | node_name                                           { $$ = $1; $$->setIsInline(false); }
  ;

node_shadow
  : node_inline BUILTIN_POINTER LABEL                   { $$ = $1; $$->setBuiltinName($3); $$->setDefinesBuiltin(true); }
  | node_inline                                         { $$ = $1; $$->setDefinesBuiltin(false); }
  ;

node_definition
  : node_shadow port_definitions '}'                    { $$ = $1; $$->setAttributeDefinitionList($2); $$->setBody(nullptr); }
  | node_shadow port_definitions node_list '}'          { $$ = $1; $$->setAttributeDefinitionList($2); $$->setBody($3); }
  | node_shadow error port_definitions '}'              { $$ = $1; $$->setAttributeDefinitionList($3); $$->setBody(nullptr); }
  | node_shadow error port_definitions node_list '}'    { $$ = $1; $$->setAttributeDefinitionList($3); $$->setBody($4); }
  | error port_definitions '}'                          { $$ = nullptr; yyerrok; }
  | error port_definitions node_list '}'                { $$ = nullptr; yyerrok; }
  ;

specific_node_definition
  : node_definition                                     { $$ = $1; }
  | PRIVATE node_definition                             { 
                                                            if ($2 != nullptr) {
                                                                $$ = $2; 
                                                                $$->setVisibility(IrVisibility::Private); 
                                                                $$->setScopeToken($1);
                                                            }
                                                            else $$ = nullptr;
                                                        }
  | PUBLIC node_definition                              { 
                                                            if ($2 != nullptr) {
                                                                $$ = $2;
                                                                $$->setVisibility(IrVisibility::Public);
                                                                $$->setScopeToken($1);
                                                            }
                                                            else $$ = nullptr;
                                                        }
  ;

immediate_node_definition
  : AUTO specific_node_definition                       {   
                                                            $$ = $2;
                                                            if ($$ != nullptr) {
                                                                IrNode *newNode = TRACK(new IrNode(*($2->getNameToken()), $2, TRACK(new IrAttributeList())));
                                                                driver.addNode(newNode);
                                                            }
                                                        }
  | specific_node_definition                            { $$ = $1; }
  ;

node_decorator
  : decorator_list immediate_node_definition            { $$ = $2; }
  | immediate_node_definition                           { $$ = $1; }
  ;

port_definitions
  : '{'                                                 { $$ = TRACK(new IrAttributeDefinitionList()); }
  | port_definitions documented_port_definition ';'     { $$ = $1; $$->addDefinition($2); }
  | port_definitions documented_port_definition error   { $$ = $1; $$->addDefinition($2); }
  | port_definitions error ';'                          { $$ = $1; }
  ;

port_declaration
  : INPUT LABEL                                         { $$ = TRACK(new IrAttributeDefinition($1, $2, IrAttributeDefinition::Direction::Input)); }
  | OUTPUT LABEL                                        { $$ = TRACK(new IrAttributeDefinition($1, $2, IrAttributeDefinition::Direction::Output)); }
  | MODIFY LABEL                                        { $$ = TRACK(new IrAttributeDefinition($1, $2, IrAttributeDefinition::Direction::Modify)); }
  | TOGGLE LABEL                                        { $$ = TRACK(new IrAttributeDefinition($1, $2, IrAttributeDefinition::Direction::Toggle)); }  
  ;

port_status
  : ALIAS port_declaration              { $$ = $2; $$->setAlias(true); $$->setAliasToken($1); }
  | port_declaration                    { $$ = $1; $$->setAlias(false); }
  ;

port_value
  : port_status '[' type_name_namespace ']'        { $$ = $1; $$->setTypeInfo($3); }
  | port_status                                    { $$ = $1; }
  ;

port_connection
  : port_value ':' value                    { $$ = $1; $$->setDefaultValue($3); }
  | port_value                              { $$ = $1; $$->setDefaultValue(nullptr); }
  ;

documented_port_definition
  : decorator_list port_connection          { $$ = $2; }
  | port_connection                         { $$ = $1; }
  ;

inline_node_member
  : inline_node                             { $$ = $1; }
  | data_access '.' inline_node             {
                                                $$ = $3;
                                                $$->setThis($1);
                                            }
  ;

inline_node
  : type_name_namespace connection_block    { $$ = TRACK(new IrNode($1.data[1], $2, $1.data[0])); }
  ;

connection_block
  : '(' ')'                             {
                                            $$ = TRACK(new IrAttributeList());
                                            $$->registerToken(&$1);
                                            $$->registerToken(&$2);
                                        }
  | '(' attribute_list ')'              {
                                            $$ = $2;
                                            $$->registerToken(&$1);
                                            $$->registerToken(&$3);
                                        }
  | '(' error ')'                       { yyerrok; }
  ;

attribute_list
  : attribute                           { 
                                            $$ = TRACK(new IrAttributeList());
                                            $$->addAttribute($1); 
                                        }
  | attribute_list ',' attribute        {
                                            $1->addAttribute($3); 
                                            $$ = $1; 
                                        }
  | error ',' attribute                 {
                                            $$ = TRACK(new IrAttributeList());
                                            $$->addAttribute($3);
                                        }
  ;

attribute
  : LABEL ':' value                     { $$ = TRACK(new IrAttribute($1, $3)); }
  | value                               { $$ = TRACK(new IrAttribute($1)); }
  ;

label_value
  : LABEL                               { $$ = static_cast<IrValue *>(TRACK(new IrValueLabel($1))); }
  ;

value
  : add_exp                             { $$ = $1; }
  ;

string
  : STRING                              { $$ = $1; @$ = $1; }
  | string STRING                       { 
                                            $$ = IrTokenInfo_string();
                                            $$.data = $1.data + $2.data;
                                            $$.combine(&$1);
                                            $$.combine(&$2);
                                        }
  ;

constant
  : INT                                 { $$ = static_cast<IrValue *>(TRACK(new IrValueInt($1))); @$ = $1; }
  | string                              { $$ = static_cast<IrValue *>(TRACK(new IrValueString($1))); }
  | FLOAT                               { $$ = static_cast<IrValue *>(TRACK(new IrValueFloat($1))); @$ = $1; }
  | BOOL                                { $$ = static_cast<IrValue *>(TRACK(new IrValueBool($1))); @$ = $1; }
  ;

atomic_value
  : label_value                         { $$ = $1; }
  | inline_node_member                  { $$ = static_cast<IrValue *>(TRACK(new IrValueNodeRef($1))); }
  | constant                            { $$ = $1; }
  ;

primary_exp
  : atomic_value                        { $$ = $1; }
  | '(' value ')'                       { $$ = $2; $$->registerToken(&$1); $$->registerToken(&$3); }
  | '(' error ')'                       { $$ = nullptr; yyerrok; }
  ;

data_access
  : primary_exp                         { $$ = $1; }
  | data_access '.' label_value         { 
                                            $$ = static_cast<IrValue *>(
                                                TRACK(new IrBinaryOperator(IrBinaryOperator::Operator::Dot, $1, $3)));
                                        }
  ;

unary_exp
  : data_access                         { $$ = $1; }
  | '-' data_access                     {
                                            $$ = static_cast<IrValue *>(
                                                TRACK(new IrUnaryOperator(IrUnaryOperator::Operator::NumericNegate, $2)));
                                        }
  | '+' data_access                     {
                                            $$ = static_cast<IrValue *>(
                                                TRACK(new IrUnaryOperator(IrUnaryOperator::Operator::Positive, $2)));
                                        }
  | '!' data_access                     {
                                            $$ = static_cast<IrValue *>(
                                                TRACK(new IrUnaryOperator(IrUnaryOperator::Operator::BoolNegate, $2)));
                                        }
  ;

mul_exp
  : unary_exp                           { $$ = $1; }
  | mul_exp '*' unary_exp               {
                                            $$ = static_cast<IrValue *>(
                                                TRACK(new IrBinaryOperator(IrBinaryOperator::Operator::Mul, $1, $3)));
                                        }
  | mul_exp '/' unary_exp               {
                                            $$ = static_cast<IrValue *>(
                                                TRACK(new IrBinaryOperator(IrBinaryOperator::Operator::Div, $1, $3)));
                                        }
  ;

add_exp
  : mul_exp                             { $$ = $1; }
  | add_exp '+' mul_exp                 {
                                            $$ = static_cast<IrValue *>(
                                                TRACK(new IrBinaryOperator(IrBinaryOperator::Operator::Add, $1, $3)));
                                        }
  | add_exp '-' mul_exp                 {
                                            $$ = static_cast<IrValue *>(
                                                TRACK(new IrBinaryOperator(IrBinaryOperator::Operator::Sub, $1, $3)));
                                        }
  ;
%%

void piranha::Parser::error(const IrTokenInfo &l, const std::string &err_message) {
    CompilationError *err;
    
    if (l.valid) {
        err = TRACK(new CompilationError(l, ErrorCode::UnexpectedToken));
    }
    else {
        err = TRACK(new CompilationError(l, ErrorCode::UnidentifiedToken));
    }

    driver.addCompilationError(err);
}
