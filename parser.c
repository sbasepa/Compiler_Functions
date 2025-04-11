#include "compiler.h"
#define BLOCK_STEP_SIZE 50
#define VAR_STEP_SIZE 10
AST_expression *makeExpression(int *i, Tokens *tokens, int minPrecedence);
int maxTokens;
void increaseIndex(int *i) {
    if (*i == maxTokens) {
        error("Parsing Error When Increasing Index");
    }
    (*i)++;
}
int precedence(Token token) {
    if (token.type != TOKEN_PUNCTUATOR) {
        error("UNKNOWN ERROR (While Parsing Expression)");
    }
    switch (token.punctuator) {
        case AST_STAR:
        case AST_FOWARD_SLASH:
        case AST_PERCENT:
            return 50;
        case AST_PLUS:
        case AST_SUBTRACT:
            return 45;
        case AST_GREATER_THAN:
        case AST_LESS_THAN:
        case AST_GREATER_THAN_EQUAL:
        case AST_LESS_THAN_EQUAL:
            return 35;
        case AST_NOT_EQUAL_TO:
        case AST_IS_EQUAL_TO:
            return 30;
        case AST_AND:
            return 10;
        case AST_OR:
            return 5;
        case AST_ASSIGNMENT:
            return 1;
        default:
            return 0;
    }
}
int isBinOp(Token token){
    if(token.type != TOKEN_PUNCTUATOR) {
        return 0;
    }
    return (token.punctuator == TOKEN_PERCENT_ASSIGNMENT) ||(token.punctuator == TOKEN_DIVIDE_ASSIGNMENT) ||(token.punctuator == TOKEN_MULTIPLY_ASSIGNMENT) ||(token.punctuator == TOKEN_MINUS_ASSIGNMENT) ||(token.punctuator == TOKEN_PLUS_ASSIGNMENT) ||(token.punctuator == TOKEN_SUBTRACT) || (token.punctuator == TOKEN_PLUS) || (token.punctuator == TOKEN_STAR) ||(token.punctuator == TOKEN_FOWARD_SLASH) || (token.punctuator == TOKEN_PERCENT) ||(token.punctuator == TOKEN_AND) || (token.punctuator == TOKEN_OR) || (token.punctuator == TOKEN_IS_EQUAL_TO) || (token.punctuator == TOKEN_NOT_EQUAL_TO) ||(token.punctuator == TOKEN_GREATER_THAN) || (token.punctuator == TOKEN_GREATER_THAN_EQUAL) || (token.punctuator == TOKEN_LESS_THAN) ||(token.punctuator == TOKEN_LESS_THAN_EQUAL) ||(token.punctuator == TOKEN_ASSIGNMENT);
}
int isUnOp(Token token) {
    if(token.type != TOKEN_PUNCTUATOR) {
        return 0;
    }
    return (token.punctuator == TOKEN_MINUS_MINUS) ||(token.punctuator == TOKEN_PLUS_PLUS) ||(token.punctuator == TOKEN_TILDE) || (token.punctuator == TOKEN_SUBTRACT) || (token.punctuator == TOKEN_EXCLAMATION);
}
AST_block_item makeBlockItem(int *i, Tokens *tokens);
AST_declaration makeDeclaration(int *i, Tokens *tokens) {
    AST_declaration output;
    Token curToken = tokens->tokens[*i];
    increaseIndex(i);
    curToken = tokens->tokens[*i];
    if (curToken.type != TOKEN_IDENTIFIER) {
        error("Expected Identifier In Declaration");
    }
    output.id = malloc(strlen(curToken.id) + 1); 
    strcpy(output.id, curToken.id);
    increaseIndex(i);
    curToken = tokens->tokens[*i];
    if (curToken.type == TOKEN_PUNCTUATOR && curToken.punctuator == TOKEN_ASSIGNMENT){
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        output.expression = makeExpression(i, tokens, 0);
        increaseIndex(i);
        curToken = tokens->tokens[*i];
    }
    if (curToken.type == TOKEN_PUNCTUATOR && curToken.punctuator == TOKEN_SEMICOLON) {
        return output;
    }
    error("Expected Semicolon or = Sign After Declaration");
}
AST_block *makeBlocks(int *i, Tokens *tokens) {
    AST_block *output = malloc(sizeof(AST_block));
    output->num_items = 0;
    int estimatedBlocks = BLOCK_STEP_SIZE;
    output->block_item = (AST_block_item *) malloc(sizeof(AST_block_item) * estimatedBlocks);
    Token curToken = tokens->tokens[*i];
    while (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_RIGHT_CURLY) {
        if (output->num_items == estimatedBlocks) {
            estimatedBlocks += BLOCK_STEP_SIZE;
            output->block_item = realloc(output->block_item, sizeof(AST_block_item) * estimatedBlocks);
        }
        output->block_item[output->num_items] = makeBlockItem(i, tokens);
        output->num_items++;
        increaseIndex(i);
        curToken = tokens->tokens[*i];
    }
    if (output->num_items != estimatedBlocks) {
        output->block_item = realloc(output->block_item, output->num_items * sizeof(AST_block_item));
    }
    return output;
}
AST_factor *makeFactor(int *i, Tokens *tokens) {
    Token curToken = tokens->tokens[*i];
    AST_factor *output = (AST_factor *) malloc(sizeof(AST_factor));
    if (curToken.type == TOKEN_CONSTANT) {
        output->type = AST_FACTOR_INT;
        output->integer = curToken.constant;
        return output;
    }
    if (curToken.type == TOKEN_IDENTIFIER) {
        if(tokens->tokens[*i+1].type == TOKEN_PUNCTUATOR && (tokens->tokens[*i+1].punctuator == TOKEN_PLUS_PLUS || tokens->tokens[*i+1].punctuator == TOKEN_MINUS_MINUS)) {
            output->factor = (AST_factor *) malloc(sizeof(AST_factor));
            output->factor->id = malloc(strlen(curToken.id) + 1); 
            strcpy(output->factor->id, curToken.id);
            increaseIndex(i);
            curToken = tokens->tokens[*i];
            output->type = AST_FACTOR_UNOP;
            output->operator = curToken.punctuator;
            output->factor->type = AST_FACTOR_ID;
            return output;
        }
        output->type = AST_FACTOR_ID;
        output->id = malloc(strlen(curToken.id) + 1); 
        strcpy(output->id, curToken.id);
        return output;
    }
    if (isUnOp(curToken)) {
        output->type = AST_FACTOR_UNOP;
        output->operator = curToken.punctuator;
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        output->factor = makeFactor(i, tokens);
        return output;
    }
    if (curToken.type == TOKEN_PUNCTUATOR && curToken.punctuator == TOKEN_LEFT_PAREN) {
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        output->type = AST_FACTOR_EXPRESSION;
        output->expression = makeExpression(i, tokens, 0);
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        if (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_RIGHT_PAREN) {
            error("Expected ) After Expression");
        }
        return output;
    }
    error("Unable To Create A Factor, %d", curToken.punctuator);
}
AST_expression *makeExpression(int *i, Tokens *tokens, int minPrecedence) {
    Token curToken = tokens->tokens[*i];
    AST_expression *output = (AST_expression *) malloc(sizeof(AST_expression));
    if (!output) {
        error("Memory allocation failed for output");
    }
    output->factor = makeFactor(i, tokens);
    output->type = AST_EXPRESSION_FACTOR;

    while (isBinOp(tokens->tokens[*i + 1]) && precedence(tokens->tokens[*i + 1]) >= minPrecedence) {
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        Token operator = curToken;

        // Allocate memory for newExpr
        AST_expression *newExpr = (AST_expression *) malloc(sizeof(AST_expression));
        if (!newExpr) {
            error("Memory allocation failed for newExpr");
        }
        newExpr->type = AST_EXPRESSION_BINOP;
        newExpr->left = output;
        newExpr->binaryOperator = operator.punctuator;
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        if (operator.punctuator == TOKEN_ASSIGNMENT) {
            newExpr->right = makeExpression(i, tokens, precedence(operator));
        } else {
            newExpr->right = makeExpression(i, tokens, precedence(operator) + 1);
        }

        output = newExpr;
    }
    return output;
}
AST_for_init makeForInit(int *i, Tokens *tokens) {
    AST_for_init output = {0};
    Token curToken = tokens->tokens[*i];
    if (curToken.type == TOKEN_KEYWORD && curToken.keyword == TOKEN_INT) {
        output.type = AST_FOR_INIT_DECLARATION;
        output.declaration = malloc(sizeof(AST_declaration)); // Allocate memory
        *output.declaration = makeDeclaration(i, tokens);
        return output;
    }
    if (curToken.type == TOKEN_PUNCTUATOR && curToken.punctuator == TOKEN_SEMICOLON) {
        output.type = AST_FOR_INIT_NULL;
        return output;
    }
    output.type = AST_FOR_INIT_EXPR;
    output.expr = makeExpression(i, tokens, 0);
    return output;
}
AST_statement makeStatement(int *i, Tokens *tokens) {
    AST_statement output = {0};
    Token curToken = tokens->tokens[*i];
    if (curToken.type == TOKEN_PUNCTUATOR && curToken.punctuator == TOKEN_SEMICOLON) {
        output.type = AST_STATEMENT_NULL;
        return output;
    }
    if (curToken.type == TOKEN_KEYWORD && curToken.keyword == TOKEN_RETURN) {
        output.type = AST_STATEMENT_RETURN;
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        output.ret_statement = makeExpression(i, tokens, 0);
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        if (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_SEMICOLON) {
            error("Expected Semicolon At End Of Return Statement");
        }
        return output;
    }
    if (curToken.type == TOKEN_KEYWORD && curToken.keyword == TOKEN_IF) {
        output.type = AST_STATEMENT_IF;
        output.if_statement = malloc(sizeof(AST_if_statement));
        output.if_statement->type = AST_IF;
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        if (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_LEFT_PAREN) {
            error("Expected Left Parenthesis For If Statement's Condition");
        }
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        output.if_statement->condition = makeExpression(i, tokens, 0);
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        if (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_RIGHT_PAREN) {
            error("Expected Right Parenthesis For If Statement's Condition");
        }
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        output.if_statement->if_statement = makeStatement(i, tokens);
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        
        if (curToken.type == TOKEN_KEYWORD && curToken.keyword == TOKEN_ELSE) {
            output.if_statement->type = AST_IF_ELSE;
            increaseIndex(i);
            curToken = tokens->tokens[*i];
            output.if_statement->else_statement =  makeStatement(i, tokens);
        } else {
            (*i)--;
        }
        return output;
    }
    if (curToken.type == TOKEN_PUNCTUATOR && curToken.punctuator == TOKEN_LEFT_CURLY) {
        output.type = AST_STATEMENT_BLOCK;
        increaseIndex(i);
        output.block = makeBlocks(i, tokens);
        return output;
    }
    if (curToken.type == TOKEN_KEYWORD && curToken.keyword == TOKEN_BREAK) {
        output.type = AST_STATEMENT_BREAK;
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        if (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_SEMICOLON) {
            error("Expected Semicolon After Keyword Break");
        }
        return output;
    }
    if (curToken.type == TOKEN_KEYWORD && curToken.keyword == TOKEN_CONTINUE) {
        output.type = AST_STATEMENT_CONTINUE;
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        if (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_SEMICOLON) {
            error("Expected Semicolon After Keyword Continue");
        }
        return output;
    }
    if (curToken.type == TOKEN_KEYWORD && curToken.keyword == TOKEN_DO) {
        output.type = AST_STATEMENT_DO;
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        output.do_statement = malloc(sizeof(AST_do_statement));
        output.do_statement->statement = malloc(sizeof(AST_statement));
        *output.do_statement->statement = makeStatement(i, tokens);
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        if (curToken.type != TOKEN_KEYWORD || curToken.keyword != TOKEN_WHILE) {
            error("Expected Keyword While In Do Statement");
        }
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        if (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_LEFT_PAREN) {
            error("Expected Left Parenthesis In Do Statement");
        }
        output.do_statement->condition = makeExpression(i,tokens,0);
        curToken = tokens->tokens[*i];
        if (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_RIGHT_PAREN) {
            error("Expected Right Parenthesis In Do Statement");
        }  
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        if (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_SEMICOLON) {
            error("Expected Semicolon In Do Statement");
        }
        return output;

    }
    if (curToken.type == TOKEN_KEYWORD && curToken.keyword == TOKEN_WHILE) {
        output.type = AST_STATEMENT_WHILE;
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        if (curToken.type == TOKEN_KEYWORD || curToken.keyword == TOKEN_WHILE) {
            error("Expected Keyword While In Do Statement");
        }
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        if (curToken.type == TOKEN_PUNCTUATOR || curToken.punctuator == TOKEN_LEFT_PAREN) {
            error("Expected Left Parenthesis In While Statement");
        }
        output.while_statement = malloc(sizeof(AST_while_statement));
        output.while_statement->condition = malloc(sizeof(AST_expression));
        output.while_statement->condition = makeExpression(i,tokens,0);
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        if (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_RIGHT_PAREN) {
            error("Expected Right Parenthesis In While Statement");
        }  
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        output.while_statement->statement = malloc(sizeof(AST_statement));
        *output.while_statement->statement = makeStatement(i,tokens);
        return output;

    }
    if (curToken.type == TOKEN_KEYWORD && curToken.keyword == TOKEN_FOR) {
        output.type = AST_STATEMENT_FOR;
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        if (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_LEFT_PAREN) {
            error("Expected Left Parenthesis In For Statement");
        }  
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        output.for_statement = malloc(sizeof(AST_for_statement));
        output.for_statement->init = makeForInit(i, tokens);
        output.for_statement->type = AST_FOR_NONE;
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        if (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_SEMICOLON) {
            output.for_statement->type = AST_FOR_CONDITION;
            output.for_statement->condition = makeExpression(i,tokens,0);
            increaseIndex(i);
            curToken = tokens->tokens[*i];
            if (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_SEMICOLON) {
                error("Exprected Semicolon After Expression In For Loop Condition");
            }
        }
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        if (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_RIGHT_PAREN) {
            if (output.for_statement->type == AST_FOR_CONDITION) {
                output.for_statement->type = AST_FOR_BOTH;
            } else {
                output.for_statement->type = AST_FOR_UPDATER;
            }
            output.for_statement->updater = malloc(sizeof(AST_expression));
            output.for_statement->updater = makeExpression(i,tokens,0);
            increaseIndex(i);
            curToken = tokens->tokens[*i];
            if (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_RIGHT_PAREN) {
                error("Exprected Right Parenthesis After Expressions In For Loop");
            }
        }
        increaseIndex(i);
        curToken = tokens->tokens[*i];
        output.for_statement->statement = malloc(sizeof(AST_statement));
        *output.for_statement->statement = makeStatement(i, tokens);
        return output;
    }
    output.type = AST_STATEMENT_EXPRESSION;
    output.expression = makeExpression(i, tokens, 0);
    increaseIndex(i);
    curToken = tokens->tokens[*i];
    if (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_SEMICOLON) {
        error("Expected Semicolon At End Of Return Statement");
    }
    return output;
}

AST_block_item makeBlockItem(int *i, Tokens *tokens) {
    AST_block_item output;
    Token curToken = tokens->tokens[*i];
    if (curToken.type == TOKEN_KEYWORD && curToken.keyword == TOKEN_INT) {
        output.type = AST_BLOCK_DECLARATION;
        output.declaration = makeDeclaration(i,tokens);
        return output;
    }
    output.type = AST_BLOCK_STATEMENT;
    output.statement = makeStatement(i, tokens);
    return output;
}

AST_function *makeFunction(Tokens *tokens) {
    int i = 0;
    Token curToken = tokens->tokens[i];
    AST_function *output = (AST_function *) malloc(sizeof(AST_function));
    if (curToken.type != TOKEN_KEYWORD || curToken.keyword != TOKEN_INT) {
        error("Expected Keyword Int");
    }
    increaseIndex(&i);
    curToken = tokens->tokens[i];
    if (curToken.type != TOKEN_IDENTIFIER) {
        error("Expected Function Name");
    }
    output->id = malloc(strlen(curToken.id) + 1); 
    strcpy(output->id, curToken.id);                          // Correct string copy              // Correct string copy              // Correct string copy              // Correct string copy
    increaseIndex(&i);
    curToken = tokens->tokens[i];
    if (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_LEFT_PAREN) {
        error("Expected Left Parenthesis");
    }
    increaseIndex(&i);
    curToken = tokens->tokens[i];
    if (curToken.type != TOKEN_KEYWORD || curToken.keyword != TOKEN_VOID) {
        error("Expected Keyword Void");
    }
    increaseIndex(&i);
    curToken = tokens->tokens[i];
    if (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_RIGHT_PAREN) {
        error("Expected Right Parenthesis");
    }
    increaseIndex(&i);
    curToken = tokens->tokens[i];
    if (curToken.type != TOKEN_PUNCTUATOR || curToken.punctuator != TOKEN_LEFT_CURLY) {
        error("Expected Left Curly Brace, %d", curToken.punctuator);
    }
    increaseIndex(&i);
    curToken = tokens->tokens[i];
    output->block = makeBlocks(&i, tokens);
    if (1+i!=maxTokens) {
        error("Extra Tokens");
    }
    return output;

}
AST *parse(Tokens *tokens) {
    maxTokens = tokens->numTokens;
    AST *output = (AST *) malloc(maxTokens * sizeof(AST));
    output->function = makeFunction(tokens);
    return output;
}