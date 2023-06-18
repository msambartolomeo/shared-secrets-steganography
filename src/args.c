#include "args.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *getModeName(int mode) {
    if (mode == DISTRIBUTE) {
        return "decode";
    } else if (mode == RECOVER) {
        return "encode";
    } else {
        return "error"; // testing purposes
    }
}

Args parseArgs(char *argv[]) {
    Args args;

    if (strcmp(argv[1], "d") == 0) {
        args.mode = DISTRIBUTE;
    } else if (strcmp(argv[1], "r") == 0) {
        args.mode = RECOVER;
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

    args.filename = malloc(strlen(argv[2]) + 1);
    if (args.filename == NULL) {
        perror("malloc error");
        exit(EXIT_FAILURE);
    }
    strcpy(args.filename, argv[2]);
    args.k = atoi(argv[3]);
    if (args.k < 3 || args.k > 8) {
        free(args.filename);
        fprintf(stderr, "k must be a value between 3 and 8.\n");
        exit(EXIT_FAILURE);
    }

    // ¿Validación de nombre de directorio? ¿Que termine en '/' capaz?
    // src:
    // https://stackoverflow.com/questions/1121383/counting-the-number-of-files-in-a-directory-using-c
    int fileCount = 0;
    DIR *dirp = opendir(argv[4]);
    if (dirp == NULL) {
        free(args.filename);
        fprintf(stderr, "4th argument must be a directory\n");
        exit(EXIT_FAILURE);
    }
    struct dirent *entry;
    while ((entry = readdir(dirp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if (entry->d_type == DT_REG) { /* If the entry is a regular file */
            fileCount++;
        }
    }
    if (fileCount < args.k) {
        free(args.filename);
        closedir(dirp);
        fprintf(stderr, "Amount of files in directory is less than k.\n");
        exit(EXIT_FAILURE);
    }
    closedir(dirp);
    args.n = fileCount;
    args.outputDir = malloc(strlen(argv[4]) + 1);
    if (args.filename == NULL) {
        free(args.filename);
        perror("malloc error");
        exit(EXIT_FAILURE);
    }
    strcpy(args.outputDir, argv[4]);

    return args;
}

void free_args(Args args) {
    free(args.filename);
    free(args.outputDir);
}
