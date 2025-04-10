#include "compiler.h"
//Option for just lexing
int op_lex;
//Option for just parsing
int op_parse;
char *filename;

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

int parse_args(int argc, char **argv){
    if (argc < 2) {
        error("Too Few Args");
    }

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--lex")) {
            op_lex = 1;
            continue;
        }
        if (!strcmp(argv[i], "--parse")) {
            op_parse = 1;
            continue;
        }
        if (argc == i+1){
            filename = argv[i];
            continue;
        }
        error("Unknown Option: %s", argv[i]);
    }
    if (op_lex + op_parse > 1) {
        error("Too Many Args");
    }
    return 0;
}
FILE *getfile(char *filename){
    FILE *output = fopen(filename, "r");
    if (!output) {
        error("Cannot Open File: %s", filename);
    }
    return output;
}

int main(int argc, char **argv) {
    //Sets option variables according to args
    parse_args(argc, argv);
    //Gets the file
    
    FILE *file = getfile(filename);
    Tokens *tokens = getTokens(file);
    if (op_lex == 1) {
        tokenPrinter(tokens);
        return 0;
    }
    AST *ast = parse(tokens);
    freeTokens(tokens);
    if (op_parse == 1) {
        astPrinter(ast);
        return 0;
    }
}