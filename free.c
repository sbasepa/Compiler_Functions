#include "compiler.h"

void freeTokens(Tokens *tokens) {
    if (!tokens) return;

    for (int i = 0; i < tokens->numTokens; i++) {
        if (tokens->tokens[i].type == TOKEN_IDENTIFIER && tokens->tokens[i].id) {
            free(tokens->tokens[i].id); // Free memory for identifiers
        }
    }

    free(tokens->tokens); // Free the array of tokens
    free(tokens);         // Free the Tokens structure itself
}