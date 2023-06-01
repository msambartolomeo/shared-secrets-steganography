#include "args.h"
#include "distribute.h"
#include "recover.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    if (argc != (4 + 1)) {
        fprintf(stderr, "Wrong number of parameters. Try again.\n");
        exit(EXIT_FAILURE);
    }

    Args args = parseArgs(argv);

    switch (args.mode) {
    case DISTRIBUTE:
        distribute(args.filename, args.k, args.n, args.outputDir);
    case RECOVER:
        recover(args.filename, args.k, args.outputDir);
        break;
    }

    free_args(args);

    return 0;
}
