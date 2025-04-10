#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//START OF TOKEN DEFINITION
typedef enum {
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_PUNCTUATOR,
    TOKEN_CONSTANT,
} TType;
typedef enum {
    TOKEN_INT,
    TOKEN_VOID,
    TOKEN_RETURN,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_DO,
    TOKEN_WHILE,
    TOKEN_FOR,
    TOKEN_BREAK,
    TOKEN_CONTINUE
} TKeyword;
typedef char *TIdentifier;
typedef enum {
    //Non Parsed
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_CURLY,
    TOKEN_RIGHT_CURLY,
    TOKEN_SEMICOLON,
    //Unary Operators
    TOKEN_EXCLAMATION,
    TOKEN_TILDE,
    TOKEN_PLUS_PLUS,
    TOKEN_MINUS_MINUS,
    //Binary Operators
    TOKEN_PLUS,
    TOKEN_STAR,
    TOKEN_FOWARD_SLASH,
    TOKEN_PERCENT,
    TOKEN_SUBTRACT,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_IS_EQUAL_TO,
    TOKEN_ASSIGNMENT,
    TOKEN_NOT_EQUAL_TO,
    TOKEN_GREATER_THAN,
    TOKEN_LESS_THAN,
    TOKEN_GREATER_THAN_EQUAL,
    TOKEN_LESS_THAN_EQUAL,
    TOKEN_PLUS_ASSIGNMENT,
    TOKEN_MINUS_ASSIGNMENT,
    TOKEN_MULTIPLY_ASSIGNMENT,
    TOKEN_DIVIDE_ASSIGNMENT,
    TOKEN_PERCENT_ASSIGNMENT
} TPunctuator;
typedef int TConstant;
typedef struct {
    TType type;
    union {
        TKeyword keyword;
        TIdentifier id;
        TPunctuator punctuator;
        TConstant constant;
    };
} Token;
typedef struct {
    Token *tokens;
    int numTokens;
} Tokens;
//END OF TOKEN DEFINITON

//START OF AST DEFINITION
typedef struct AST_expression AST_expression;
typedef struct AST_factor AST_factor;
typedef struct AST_statement AST_statement;
typedef struct AST_block AST_block;
typedef struct AST_declaration AST_declaration;
typedef enum {
    AST_NEGATE=11,
    AST_NOT = 5,
    AST_TILDE,
    AST_PLUS_PLUS,
    AST_MINUS_MINUS,
} AST_unary_operator;
typedef enum {
    AST_PLUS = 9,
    AST_STAR,
    AST_FOWARD_SLASH,
    AST_PERCENT,
    AST_SUBTRACT,
    AST_AND,
    AST_OR,
    AST_IS_EQUAL_TO,
    AST_ASSIGNMENT,
    AST_NOT_EQUAL_TO,
    AST_GREATER_THAN,
    AST_LESS_THAN,
    AST_GREATER_THAN_EQUAL,
    AST_LESS_THAN_EQUAL,
    AST_PLUS_ASSIGNMENT,
    AST_MINUS_ASSIGNMENT,
    AST_MULTIPLY_ASSIGNMENT,
    AST_DIVIDE_ASSIGNMENT,
    AST_PERCENT_ASSIGNMENT,
} AST_binary_operator;
typedef enum {
    AST_FACTOR_INT,
    AST_FACTOR_ID,
    AST_FACTOR_UNOP,
    AST_FACTOR_EXPRESSION
} AST_factor_type;
typedef struct AST_factor{
    AST_factor_type type;
    union {
        int integer;
        char *id;
        struct {
            AST_unary_operator operator;
            AST_factor *factor;
        };
        AST_expression *expression;
    };
} AST_factor;
typedef enum {
    AST_EXPRESSION_FACTOR,
    AST_EXPRESSION_BINOP
} AST_expression_type;
typedef struct AST_expression{
    AST_expression_type type;
    union {
        AST_factor *factor;
        struct {
            AST_expression *left;
            AST_binary_operator binaryOperator;
            AST_expression *right;
        };
    };
} AST_expression;
typedef AST_expression AST_return_statement;
typedef enum {
    AST_IF_ELSE,
    AST_IF
} AST_if_type;

typedef enum {
    AST_STATEMENT_RETURN,
    AST_STATEMENT_EXPRESSION,
    AST_STATEMENT_IF,
    AST_STATEMENT_NULL,//No real Value
    AST_STATEMENT_BLOCK,
    AST_STATEMENT_BREAK,//No real Value
    AST_STATEMENT_CONTINUE, //No real Value
    AST_STATEMENT_WHILE,
    AST_STATEMENT_DO,
    AST_STATEMENT_FOR
} AST_statement_type;
typedef struct {
    AST_expression *condition;
    AST_statement *statement;
} AST_while_statement;
typedef AST_while_statement AST_do_statement;
typedef enum {
    AST_FOR_NONE,
    AST_FOR_BOTH,
    AST_FOR_UPDATER,
    AST_FOR_CONDITION
} AST_for_type;
typedef enum {
    AST_FOR_INIT_NULL,
    AST_FOR_INIT_EXPR,
    AST_FOR_INIT_DECLARATION
} AST_for_init_type;
typedef struct {
    AST_for_init_type type;
    union {
        AST_expression *expr;
        AST_declaration *declaration;
    };
} AST_for_init;
typedef struct {
    AST_for_type type;
    AST_statement *statement;
    AST_for_init init;
    AST_expression *condition;
    AST_expression *updater;
} AST_for_statement;
typedef struct AST_if_statement AST_if_statement;
typedef struct AST_statement {
    AST_statement_type type;
    union {
        AST_return_statement *ret_statement;
        AST_expression *expression;
        AST_if_statement *if_statement; // Forward declaration is fine here
        AST_while_statement *while_statement;
        AST_do_statement *do_statement;
        AST_for_statement *for_statement;
        AST_block *block;
    };
} AST_statement;

typedef struct AST_if_statement{
    AST_if_type type;
    AST_expression *condition;
    AST_statement if_statement; // Now AST_statement is fully defined
    AST_statement else_statement;
} AST_if_statement;
typedef struct AST_declaration{
    char *id;
    AST_expression *expression;
} AST_declaration;
typedef enum {
    AST_BLOCK_DECLARATION,
    AST_BLOCK_STATEMENT,
} AST_block_type;
typedef struct AST_block_item {
    AST_block_type type;
    union {
        AST_declaration declaration;
        AST_statement statement;
    };
} AST_block_item;
typedef struct AST_block {
    AST_block_item *block_item;
    int num_items;
} AST_block;
typedef struct {
    char *id;
    AST_block *block;
} AST_function;
typedef struct {
    AST_function *function;
} AST;
//END OF AST DEFINITION

//START OF FUNCTION DEFINITIONS
void error(char *fmt, ...);
void tokenPrinter(Tokens *token);
AST *parse(Tokens *tokens);
Tokens *getTokens(FILE *file);
void freeTokens(Tokens *tokens);
void astPrinter(AST *ast);
//END OF FUNCTION DEFINITIONS
