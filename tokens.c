#include "compiler.h"
#define INITIAL_TOKEN_CAPACITY 50
#define INITIAL_WORD_CAPACITY 5
#define TOKEN_STEP_SIZE INITIAL_TOKEN_CAPACITY
#define WORD_STEP_SIZE INITIAL_WORD_CAPACITY

int isIdentifier(char *c) {
    if (!isalpha(c[0]) && c[0] != '_') {
        return 0;
    }
    for (int i = 1; i < strlen(c); i++) {
        if (!isalnum(c[i]) && c[i] != '_') {
            return 0;
        }
    }
    return 1;
}
int isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f' || c == EOF;
}
char peek(FILE *file) {
    char c = fgetc(file);
    if (c != EOF) {
        ungetc(c, file);
    }
    return c;
}
int isPunctuator(char c) {
    return c == ';' || c == '}' || c == '{' || c == ')' || c == '(' || c == '-'|| c == '~' || c== '+' || c == '*' || c== '/'|| c== '%' || c == '&' || c == '|' || c == '!' || c == '=' || c == '<' || c == '>';
}
Token makePunctuatorToken(char current, FILE *file) {
    Token curToken = {0};
    curToken.type = TOKEN_PUNCTUATOR;
    switch (current) {
        case ';': curToken.punctuator = TOKEN_SEMICOLON; break;
        case '}': curToken.punctuator = TOKEN_RIGHT_CURLY; break;
        case '{': curToken.punctuator = TOKEN_LEFT_CURLY; break;
        case ')': curToken.punctuator = TOKEN_RIGHT_PAREN; break;
        case '(': curToken.punctuator = TOKEN_LEFT_PAREN; break;
        case '~': curToken.punctuator = TOKEN_TILDE; break;
        case '+': 
            if (peek(file) == '=' ) {
                curToken.punctuator = TOKEN_PLUS_ASSIGNMENT;
                current = fgetc(file);
                break;
            } else if (peek(file) == '+') {
                curToken.punctuator = TOKEN_PLUS_PLUS;
                current = fgetc(file);
                break;
            }
            else {
                curToken.punctuator = TOKEN_PLUS;
                break;
            }
        case '-':
            if (peek(file) == '=' ) {
                curToken.punctuator = TOKEN_MINUS_ASSIGNMENT;
                current = fgetc(file);
                break;
            } else if (peek(file) == '-'){
                curToken.punctuator = TOKEN_MINUS_MINUS;
                current = fgetc(file);
                break;
            }
            else {
                curToken.punctuator = TOKEN_SUBTRACT;
                break;
            }
        case '*': 
            if (peek(file) == '=' ) {
                curToken.punctuator = TOKEN_MULTIPLY_ASSIGNMENT;
                current = fgetc(file);
                break;
            }
            else {
                curToken.punctuator = TOKEN_STAR;
                break;
            }
        case '/': 
            if (peek(file) == '=' ) {
                curToken.punctuator = TOKEN_DIVIDE_ASSIGNMENT;
                current = fgetc(file);
                break;
            }
            else {
                curToken.punctuator = TOKEN_FOWARD_SLASH;
                break;
            }
        case '%': 
            if (peek(file) == '=' ) {
                curToken.punctuator = TOKEN_PERCENT_ASSIGNMENT;
                current = fgetc(file);
                break;
            }
            else {
                curToken.punctuator = TOKEN_PERCENT;
                break;
            }
        case '&':
            if (peek(file) == '&') {
                curToken.punctuator = TOKEN_AND;
                current = fgetc(file);
            }
            else {
                error("Only One &");
            }
            break;
        case '|':
            if (peek(file) == '|') {
                curToken.punctuator = TOKEN_OR;
                current = fgetc(file);
            }
            else {
                error("Only One |");
            }
            break;
        case '!':
            if (peek(file) == '=') {
                curToken.punctuator = TOKEN_NOT_EQUAL_TO;
                current = fgetc(file);
            }
            else {
                curToken.punctuator = TOKEN_EXCLAMATION;
            }
            break;
        case '=':
            if (peek(file) == '=') {
                curToken.punctuator = TOKEN_IS_EQUAL_TO;
                current = fgetc(file);
            }
            else {
                curToken.punctuator = TOKEN_ASSIGNMENT;
            }
            break;
        case '<':
            if (peek(file) == '=') {
                curToken.punctuator = TOKEN_LESS_THAN_EQUAL;
                current = fgetc(file);
            }
            else {
                curToken.punctuator = TOKEN_LESS_THAN;
            }
            break;
        case '>':
            if (peek(file) == '=') {
                curToken.punctuator = TOKEN_GREATER_THAN_EQUAL;
                current = fgetc(file);
            }
            else {
                curToken.punctuator = TOKEN_GREATER_THAN;
            }
            break;
    }
    return curToken;
}
char *getWord(FILE *file, char current) {
    int estimatedCapacity = INITIAL_WORD_CAPACITY;
    char *word = malloc(sizeof(char) * estimatedCapacity);
    int numChar = 0;
    do {
        if (numChar == estimatedCapacity - 1) {
            word = realloc(word, sizeof(char) * (estimatedCapacity += WORD_STEP_SIZE));
        }
        word[numChar++] = current;
    } while (!isWhitespace(current = fgetc(file)) && !isPunctuator(current));
    ungetc(current, file);
    word[numChar] = '\0'; // Null-terminate the string
    return word;
}
int isKeyword(char *curWord) {
    return (strcmp(curWord, "continue")== 0) ||(strcmp(curWord, "break")== 0) ||(strcmp(curWord, "for")== 0) ||(strcmp(curWord, "while")== 0) ||(strcmp(curWord, "do")== 0) ||(strcmp(curWord, "else")== 0) ||(strcmp(curWord, "if")== 0) ||(strcmp(curWord, "void")== 0) || (strcmp(curWord, "return")== 0) || (strcmp(curWord, "int")== 0);
}
int isConstant(char *c) {
    for (int i = 0; i < strlen(c); i++) {
        if (!isdigit(c[i])) {
            return 0;
        }
    }
    return 1;
}
Token makeKeywordToken(char *word){
    Token output = {0};
    output.type = TOKEN_KEYWORD;
    if (strcmp(word, "int") == 0) {
        output.keyword = TOKEN_INT;
        return output;
    }
    if (strcmp(word, "void")== 0) {
        output.keyword = TOKEN_VOID;
        return output;
    }
    if (strcmp(word, "return") == 0){
        output.keyword = TOKEN_RETURN;
        return output;
    }
    if (strcmp(word, "if") == 0) {
        output.keyword = TOKEN_IF;
        return output;
    }
    if (strcmp(word, "else") == 0) {
        output.keyword = TOKEN_ELSE;
        return output;
    }
    if (strcmp(word, "do") == 0) {
        output.keyword = TOKEN_DO;
        return output;
    }
    if (strcmp(word, "while") == 0) {
        output.keyword = TOKEN_WHILE;
        return output;
    }
    if (strcmp(word, "for") == 0) {
        output.keyword = TOKEN_FOR;
        return output;
    }
    if (strcmp(word, "break") == 0) {
        output.keyword = TOKEN_BREAK;
        return output;
    }
    if (strcmp(word, "continue") == 0) {
        output.keyword = TOKEN_CONTINUE;
        return output;
    }
    error("Expected %s To Be A Keyword", word);
}
Tokens *getTokens(FILE *file){
    int estimatedTokens = INITIAL_TOKEN_CAPACITY;
    Tokens *output= malloc(sizeof(Tokens));
    output->numTokens = 0;
    output->tokens = malloc(sizeof(Token) * estimatedTokens);
    for (char current = fgetc(file); current != EOF; current = fgetc(file)) {
        if (output->numTokens == estimatedTokens) {
            estimatedTokens += TOKEN_STEP_SIZE;
            output->tokens = realloc(output->tokens, sizeof(Token) * estimatedTokens);
        }
        if (isWhitespace(current)) {
            continue;
        }
        if (current == '#') {
            while (current != '\n'&& current != EOF) {
                current = fgetc(file);
            }
            continue;
        }
        if (current == '/' && peek(file) == '/') {
            while (current != '\n'&& current != EOF) {
                current = fgetc(file);
            }
            continue;
        }
        if (current == '/' && peek(file) == '*') {
            while (current != EOF && current != '*' && (current = fgetc(file)) != '/') {
                current = fgetc(file);
            }
            current = fgetc(file);
            continue;
        }
        if (isPunctuator(current)) {
            output->tokens[output->numTokens] = makePunctuatorToken(current, file);
            output->numTokens++;
            continue;
        }
        char *curWord = getWord(file, current);
        if (isKeyword(curWord)) {
            output->tokens[output->numTokens] = makeKeywordToken(curWord);
            output->numTokens++;
            free(curWord);
            continue;
        }
        if (isIdentifier(curWord)) {
            output->tokens[output->numTokens].id = malloc(sizeof(char) * strlen(curWord)+1);
            strcpy(output->tokens[output->numTokens].id, curWord);
            output->tokens[output->numTokens].type = TOKEN_IDENTIFIER;
            output->numTokens++;
            free(curWord);
            continue;
        }
        if (isConstant(curWord)) {
            output->tokens[output->numTokens].constant = atoi(curWord);
            output->tokens[output->numTokens].type = TOKEN_CONSTANT;
            output->numTokens++;
            free(curWord);
            continue;
        }
        error("Unknown Token: %s", curWord);
    }
    if (output->numTokens != estimatedTokens) {
        output->tokens = realloc(output->tokens, sizeof(Token) * output->numTokens);
    }
    fclose(file);
    return output;
}