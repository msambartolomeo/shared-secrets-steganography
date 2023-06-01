#pragma once
enum MODE { DISTRIBUTE, RECOVER };

typedef struct Args {
    enum MODE mode;
    char *filename;
    int k;
    char *outputDir;
    int n;
} Args;

Args parseArgs(char *argv[]);

void free_args(Args args);
