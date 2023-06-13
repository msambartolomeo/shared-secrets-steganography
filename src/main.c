#include "args.h"
#include "distribute.h"
#include "recover.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    if (argc != (4 + 1)) {
        fprintf(stderr, "Wrong number of parameters. Try again.\n");
        return EXIT_FAILURE;
    }

    Args args = parseArgs(argv);

    int exit_status;
    switch (args.mode) {
    case DISTRIBUTE:
        exit_status = distribute(args.filename, args.k, args.n, args.outputDir);
        break;
    case RECOVER:
        exit_status = recover(args.filename, args.k, args.outputDir);
        break;
    }

    free_args(args);

    return exit_status;
}
