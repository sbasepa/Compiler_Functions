#include "compiler.h"
//START OF TOKEN PRINTER
void tokenPrintKeyword(TKeyword keyword) {
    switch (keyword) {
        case TOKEN_INT:       printf("int");        return;
        case TOKEN_VOID:      printf("void");       return;
        case TOKEN_RETURN:    printf("return");     return;
        case TOKEN_IF:        printf("if");         return;
        case TOKEN_ELSE:      printf("else");       return;
        case TOKEN_DO:        printf("do");         return;
        case TOKEN_WHILE:     printf("while");      return;
        case TOKEN_FOR:       printf("for");        return;
        case TOKEN_BREAK:     printf("break");      return;
        case TOKEN_CONTINUE:  printf("continue");   return;
        default:
            error("UNKNOWN ERROR (While Printing Tokens)");
            return;
    }
}

void tokenPrintPunctuator(TPunctuator punc) {
    switch(punc) {
        case TOKEN_LEFT_PAREN:          printf("(");    break;
        case TOKEN_RIGHT_PAREN:         printf(")");    break;
        case TOKEN_LEFT_CURLY:          printf("{");    break;
        case TOKEN_RIGHT_CURLY:         printf("}");    break;
        case TOKEN_SEMICOLON:           printf(";");    break;
        case TOKEN_TILDE:               printf("~");    break;
        case TOKEN_PLUS:                printf("+");    break;
        case TOKEN_PLUS_ASSIGNMENT:     printf("+=");   break;
        case TOKEN_PLUS_PLUS:           printf("++");   break;
        case TOKEN_STAR:                printf("*");    break;
        case TOKEN_MULTIPLY_ASSIGNMENT: printf("*=");   break;
        case TOKEN_FOWARD_SLASH:        printf("/");    break;
        case TOKEN_DIVIDE_ASSIGNMENT:   printf("/=");   break;
        case TOKEN_PERCENT:             printf("%%");    break;
        case TOKEN_PERCENT_ASSIGNMENT:  printf("%%=");   break;
        case TOKEN_SUBTRACT:            printf("-");    break;
        case TOKEN_MINUS_ASSIGNMENT:    printf("-=");   break;
        case TOKEN_MINUS_MINUS:         printf("--");   break;
        case TOKEN_EXCLAMATION:         printf("!");    break;
        case TOKEN_AND:                 printf("&&");   break;
        case TOKEN_OR:                  printf("||");   break;
        case TOKEN_IS_EQUAL_TO:         printf("==");   break;
        case TOKEN_ASSIGNMENT:          printf("=");    break;
        case TOKEN_NOT_EQUAL_TO:        printf("!=");   break;
        case TOKEN_GREATER_THAN:        printf(">");    break;
        case TOKEN_LESS_THAN:           printf("<");    break;
        case TOKEN_GREATER_THAN_EQUAL:  printf(">=");   break;
        case TOKEN_LESS_THAN_EQUAL:     printf("<=");   break;
        default:
            error("UNKNOWN ERROR (While Printing Tokens)");
            break;
    }
}

// Helper function to print token type description
void printTokenTypeDesc(Token token) {
    switch (token.type) {
        case TOKEN_KEYWORD:
            printf("KEYWORD ");
            break;
        case TOKEN_CONSTANT:
            printf("CONSTANT ");
            break;
        case TOKEN_IDENTIFIER:
            printf("ID ");
            break;
        case TOKEN_PUNCTUATOR:
            printf("PUNCTUATOR ");
            break;
        default:
            printf("UNKNOWN ");
            break;
    }
}

void tokenPrinter(Tokens *token) {
    printf("Token Stream:\n");
    printf("==============\n");
    printf("Total Tokens: %d\n\n", token->numTokens);
    
    // Calculate column widths for formatting
    int numWidth = 4;                          // Width for index column
    int typeWidth = 10;                        // Width for type column
    int valueWidth = 15;                       // Width for value column
    
    // Print header
    printf("%-*s | %-*s | %-*s\n", numWidth, "Idx", typeWidth, "Type", valueWidth, "Value");
    printf("%s+%s+%s\n", 
        "----", "------------", "-----------------");
    
    // Print each token
    for (int i = 0; i < token->numTokens; i++) {
        Token curToken = token->tokens[i];
        
        // Print index
        printf("%-*d | ", numWidth, i);
        
        // Print type
        printf("%-*s | ", typeWidth, 
            curToken.type == TOKEN_KEYWORD ? "KEYWORD" :
            curToken.type == TOKEN_CONSTANT ? "CONSTANT" :
            curToken.type == TOKEN_IDENTIFIER ? "ID" :
            curToken.type == TOKEN_PUNCTUATOR ? "PUNCTUATOR" : "UNKNOWN");
        
        // Print value
        if (curToken.type == TOKEN_KEYWORD) {
            tokenPrintKeyword(curToken.keyword);
            for (int j = 0; j < valueWidth - 10; j++) printf(" "); // Padding
        }
        else if (curToken.type == TOKEN_CONSTANT) {
            printf("%-*d", valueWidth, curToken.constant);
        }
        else if (curToken.type == TOKEN_IDENTIFIER) {
            printf("%-*s", valueWidth, curToken.id);
        }
        else if (curToken.type == TOKEN_PUNCTUATOR) {
            printf("'");
            tokenPrintPunctuator(curToken.punctuator);
            printf("'");
            for (int j = 0; j < valueWidth - 3; j++) printf(" "); // Padding
        }
        
        printf("\n");
    }
    
    printf("==============\n");
}

//END OF TOKEN PRINTER
//START OF AST PRINTER
#include <stdio.h>
#include <string.h>

// Node ID counter for unique identifiers
static int currentNodeId = 1;
static FILE *outputFile = NULL; // File pointer for output

// Forward declarations
int printExpression(AST_expression *expr);
int printStatement(AST_statement statement);
int printBlockItem(AST_block_item item);
int printBlock(AST_block *block);
int printFactor(AST_factor *factor);
int printDeclaration(AST_declaration declaration);
int printForInit(AST_for_init init);

// Get operator strings
const char* getBinOpString(AST_binary_operator binop) {
    switch(binop) {
        case AST_PLUS:              return "+";
        case AST_STAR:              return "*";
        case AST_SUBTRACT:          return "-";
        case AST_FOWARD_SLASH:      return "/";
        case AST_PERCENT:           return "%";
        case AST_AND:               return "&&";
        case AST_OR:                return "||";
        case AST_IS_EQUAL_TO:       return "==";
        case AST_ASSIGNMENT:        return "=";
        case AST_NOT_EQUAL_TO:      return "!=";
        case AST_GREATER_THAN:      return ">";
        case AST_LESS_THAN:         return "<";
        case AST_GREATER_THAN_EQUAL: return ">=";
        case AST_LESS_THAN_EQUAL:   return "<=";
        case AST_PLUS_ASSIGNMENT:   return "+=";
        case AST_MINUS_ASSIGNMENT:  return "-=";
        case AST_MULTIPLY_ASSIGNMENT: return "*=";
        case AST_DIVIDE_ASSIGNMENT: return "/=";
        case AST_PERCENT_ASSIGNMENT: return "%=";
        default:                    return "UNKNOWN";
    }
}

const char* getUnOpString(AST_unary_operator unop) {
    switch (unop) {
        case AST_NEGATE:        return "-";
        case AST_TILDE:         return "~";
        case AST_NOT:           return "!";
        case AST_MINUS_MINUS:   return "--";
        case AST_PLUS_PLUS:     return "++";
        default:                return "UNKNOWN";
    }
}

// Get a new node ID
int getNextNodeId() {
    return currentNodeId++;
}

int printFactor(AST_factor *factor) {
    int nodeId = getNextNodeId();
    
    if (factor->type == AST_FACTOR_INT) {
        fprintf(outputFile, "node%d [label=\"IntConstant\\n%d\"]\n", nodeId, factor->integer);
        return nodeId;
    }
    
    if (factor->type == AST_FACTOR_ID) {
        fprintf(outputFile, "node%d [label=\"Identifier\\n'%s'\"]\n", nodeId, factor->id);
        return nodeId;
    }
    
    if (factor->type == AST_FACTOR_EXPRESSION) {
        fprintf(outputFile, "node%d [label=\"ParenExpr\"]\n", nodeId);
        int exprId = printExpression(factor->expression);
        fprintf(outputFile, "node%d -> node%d\n", nodeId, exprId);
        return nodeId;
    }
    
    if (factor->type == AST_FACTOR_UNOP) {
        fprintf(outputFile, "node%d [label=\"UnaryOp\\n%s\"]\n", nodeId, getUnOpString(factor->operator));
        int factorId = printFactor(factor->factor);
        fprintf(outputFile, "node%d -> node%d\n", nodeId, factorId);
        return nodeId;
    }
    
    error("Expected Unop, Expression, Int, or Id While Printing");
    return nodeId;
}

int printExpression(AST_expression *expr) {
    int nodeId = getNextNodeId();
    
    if (expr->type == AST_EXPRESSION_FACTOR) {
        fprintf(outputFile, "node%d [label=\"Factor\"]\n", nodeId);
        int factorId = printFactor(expr->factor);
        fprintf(outputFile, "node%d -> node%d\n", nodeId, factorId);
        return nodeId;
    }
    
    if (expr->type == AST_EXPRESSION_BINOP) {
        fprintf(outputFile, "node%d [label=\"BinaryOp\\n%s\"]\n", nodeId, getBinOpString(expr->binaryOperator));
        
        int leftId = printExpression(expr->left);
        fprintf(outputFile, "node%d -> node%d [label=\"left\"]\n", nodeId, leftId);
        
        int rightId = printExpression(expr->right);
        fprintf(outputFile, "node%d -> node%d [label=\"right\"]\n", nodeId, rightId);
        
        return nodeId;
    }
    
    error("Expected A Binary Operation Or Factor While Printing");
    return nodeId;
}

int printDeclaration(AST_declaration declaration) {
    int nodeId = getNextNodeId();
    fprintf(outputFile, "node%d [label=\"Declaration\\n'%s'\"]\n", nodeId, declaration.id);
    
    int exprId = printExpression(declaration.expression);
    fprintf(outputFile, "node%d -> node%d [label=\"init\"]\n", nodeId, exprId);
    
    return nodeId;
}

int printForInit(AST_for_init init) {
    if (init.type == AST_FOR_INIT_DECLARATION) {
        return printDeclaration(*init.declaration);
    }
    if (init.type == AST_FOR_INIT_EXPR) {
        return printExpression(init.expr);
    }
    
    int nodeId = getNextNodeId();
    fprintf(outputFile, "node%d [label=\"Empty\"]\n", nodeId);
    return nodeId;
}

int printStatement(AST_statement statement) {
    int nodeId = getNextNodeId();
    
    if (statement.type == AST_STATEMENT_RETURN) {
        fprintf(outputFile, "node%d [label=\"Return\"]\n", nodeId);
        int exprId = printExpression(statement.ret_statement);
        fprintf(outputFile, "node%d -> node%d [label=\"value\"]\n", nodeId, exprId);
        return nodeId;
    }
    
    if (statement.type == AST_STATEMENT_EXPRESSION) {
        fprintf(outputFile, "node%d [label=\"ExprStmt\"]\n", nodeId);
        int exprId = printExpression(statement.expression);
        fprintf(outputFile, "node%d -> node%d\n", nodeId, exprId);
        return nodeId;
    }
    
    if (statement.type == AST_STATEMENT_CONTINUE) {
        fprintf(outputFile, "node%d [label=\"Continue\"]\n", nodeId);
        return nodeId;
    }
    
    if (statement.type == AST_STATEMENT_BREAK) {
        fprintf(outputFile, "node%d [label=\"Break\"]\n", nodeId);
        return nodeId;
    }
    
    if (statement.type == AST_STATEMENT_IF) {
        if (statement.if_statement->type != AST_IF_ELSE) {
            fprintf(outputFile, "node%d [label=\"If\"]\n", nodeId);
            
            int condId = printExpression(statement.if_statement->condition);
            fprintf(outputFile, "node%d -> node%d [label=\"cond\"]\n", nodeId, condId);
            
            int thenId = printStatement(statement.if_statement->if_statement);
            fprintf(outputFile, "node%d -> node%d [label=\"then\"]\n", nodeId, thenId);
        } else {
            fprintf(outputFile, "node%d [label=\"IfElse\"]\n", nodeId);
            
            int condId = printExpression(statement.if_statement->condition);
            fprintf(outputFile, "node%d -> node%d [label=\"cond\"]\n", nodeId, condId);
            
            int thenId = printStatement(statement.if_statement->if_statement);
            fprintf(outputFile, "node%d -> node%d [label=\"then\"]\n", nodeId, thenId);
            
            int elseId = printStatement(statement.if_statement->else_statement);
            fprintf(outputFile, "node%d -> node%d [label=\"else\"]\n", nodeId, elseId);
        }
        return nodeId;
    }
    
    if (statement.type == AST_STATEMENT_DO) {
        fprintf(outputFile, "node%d [label=\"Do\"]\n", nodeId);
        
        int bodyId = printStatement(*statement.do_statement->statement);
        fprintf(outputFile, "node%d -> node%d [label=\"body\"]\n", nodeId, bodyId);
        
        int condId = printExpression(statement.do_statement->condition);
        fprintf(outputFile, "node%d -> node%d [label=\"cond\"]\n", nodeId, condId);
        
        return nodeId;
    }
    
    if (statement.type == AST_STATEMENT_WHILE) {
        fprintf(outputFile, "node%d [label=\"While\"]\n", nodeId);
        
        int condId = printExpression(statement.while_statement->condition);
        fprintf(outputFile, "node%d -> node%d [label=\"cond\"]\n", nodeId, condId);
        
        int bodyId = printStatement(*statement.while_statement->statement);
        fprintf(outputFile, "node%d -> node%d [label=\"body\"]\n", nodeId, bodyId);
        
        return nodeId;
    }
    
    if(statement.type == AST_STATEMENT_FOR) {
        fprintf(outputFile, "node%d [label=\"For\"]\n", nodeId);
        
        int initId = printForInit(statement.for_statement->init);
        fprintf(outputFile, "node%d -> node%d [label=\"init\"]\n", nodeId, initId);
        
        if (statement.for_statement->type == AST_FOR_CONDITION || 
            statement.for_statement->type == AST_FOR_BOTH) {
            int condId = printExpression(statement.for_statement->condition);
            fprintf(outputFile, "node%d -> node%d [label=\"cond\"]\n", nodeId, condId);
        }
        
        if (statement.for_statement->type == AST_FOR_UPDATER || 
            statement.for_statement->type == AST_FOR_BOTH) {
            int updateId = printExpression(statement.for_statement->updater);
            fprintf(outputFile, "node%d -> node%d [label=\"update\"]\n", nodeId, updateId);
        }
        
        int bodyId = printStatement(*statement.for_statement->statement);
        fprintf(outputFile, "node%d -> node%d [label=\"body\"]\n", nodeId, bodyId);
        
        return nodeId;
    }
    
    if (statement.type == AST_STATEMENT_NULL) {
        fprintf(outputFile, "node%d [label=\"NullStmt\"]\n", nodeId);
        return nodeId;
    }
    
    if (statement.type == AST_STATEMENT_BLOCK) {
        fprintf(outputFile, "node%d [label=\"CompoundStmt\"]\n", nodeId);
        int blockId = printBlock(statement.block);
        fprintf(outputFile, "node%d -> node%d\n", nodeId, blockId);
        return nodeId;
    }
    
    error("Expected Expression Or Return Statement While Printing");
    return nodeId;
}

int printBlockItem(AST_block_item item) {
    if (item.type == AST_BLOCK_DECLARATION) {
        return printDeclaration(item.declaration);
    }
    
    if (item.type == AST_BLOCK_STATEMENT) {
        return printStatement(item.statement);
    }
    
    error("Expected Statement Or Expression While Printing");
    return getNextNodeId();
}

int printBlock(AST_block *block) {
    int nodeId = getNextNodeId();
    fprintf(outputFile, "node%d [label=\"Block\"]\n", nodeId);
    
    for (int i = 0; i < block->num_items; i++) {
        int itemId = printBlockItem(block->block_item[i]);
        fprintf(outputFile, "node%d -> node%d\n", nodeId, itemId);
    }
    
    return nodeId;
}

void astPrinter(AST *ast) {
    char *filename = "COMPILERTEMP18h8hw8h2828h28h28h2h8wbsd892j";
    // Open file for writing
    outputFile = fopen(filename, "w");
    if (!outputFile) {
        perror("Failed to open file for writing");
        return;
    }

    // Reset node counter
    currentNodeId = 1;
    // Print DOT format header
    fprintf(outputFile, "digraph AST {\n");
    fprintf(outputFile, "  node [shape=box, fontname=\"Courier\", fontsize=12];\n");
    fprintf(outputFile, "  edge [fontname=\"Courier\", fontsize=10];\n");
    
    // Print root function node - use getNextNodeId() to ensure consistent ID assignment
    int functionNodeId = getNextNodeId(); // Should return 1 since we reset above
    fprintf(outputFile, "  node%d [label=\"Function\\n'%s'\"];\n", functionNodeId, ast->function->id);
    
    // Print block and connect to function
    int blockId = printBlock(ast->function->block);
    fprintf(outputFile, "  node%d -> node%d [label=\"body\"];\n", functionNodeId, blockId);
    
    // Print DOT format footer
    fprintf(outputFile, "}\n");
    
    // Close the file
    fclose(outputFile);
    outputFile = NULL;
    system("dot -Tpng COMPILERTEMP18h8hw8h2828h28h28h2h8wbsd892j -o ast.png");
    system("rm -f COMPILERTEMP18h8hw8h2828h28h28h2h8wbsd892j");
}
//END OF AST PRINTER
