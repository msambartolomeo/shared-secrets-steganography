#include "include/test.h"
#include <dirent.h>
#include <main.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

static int mode;
static char *filename;
static int k;
static char *outputDir;

int main(int argc, char *argv[]) {

    if (argc != (4 + 1)) {
        fprintf(stderr, "Wrong number of parameters. Try again.");
        exit(EXIT_FAILURE);
    }

    parseArgs(argv);

    return 0;
}

// Utils

void parseArgs(char *argv[]) {
    if (strcmp(argv[1], "-d") == 0) {
        mode = DECODE;
    } else if (strcmp(argv[1], "-r") == 0) {
        mode = ENCODE;
    } else {
        fprintf(stderr, "Wrong value for first argument. Must be 'r' or 'd'\n");
        exit(EXIT_FAILURE);
    }

    char *extension = strrchr(argv[2], '.');
    if (!extension || strcmp(extension, ".bmp") != 0) {
        fprintf(stderr,
                "Second argument should be a file with .bmp extension.\n");
        exit(EXIT_FAILURE);
    }

    filename = malloc(strlen(argv[2]) + 1);
    strcpy(filename, argv[2]);

    int aux_k = atoi(argv[3]);
    if (aux_k < 3 || aux_k > 8) {
        fprintf(stderr, "k must be a value between 3 and 8.\n");
        exit(EXIT_FAILURE);
    }
    k = aux_k;

    // ¿Validación de nombre de directorio? ¿Que termine en '/' capaz?
    // src:
    // https://stackoverflow.com/questions/1121383/counting-the-number-of-files-in-a-directory-using-c
    int fileCount = 0;
    DIR *dirp = opendir(argv[4]);
    if (dirp == NULL) {
        fprintf(stderr, "4th argument must be a directory\n");
        exit(EXIT_FAILURE);
    }
    struct dirent *entry;
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_REG) { /* If the entry is a regular file */
            fileCount++;
        }
    }
    if (fileCount < k) {
        fprintf(stderr, "Amount of files in directory is less than k.\n");
        exit(EXIT_FAILURE);
    }
    closedir(dirp);

    outputDir = malloc(strlen(argv[4]) + 1);
    strcpy(outputDir, argv[4]);

    printf("Arguments: %s %s %d %s\n", getModeName(mode), filename, k,
           outputDir);
}

char *getModeName(int mode) {
    if (mode == DECODE) {
        return "decode";
    } else if (mode == ENCODE) {
        return "encode";
    } else {
        return "error"; // testing purposes
    }
}
