#pragma once
enum MODE { DISTRIBUTE, RECOVER };

typedef struct Args {
    enum MODE mode;
    char *filename;
    int k;
    char *outputDir;
} Args;

Args parseArgs(char *argv[]);
