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

    Args args = parse_args(argv);

    int exit_status;
    switch (args.mode) {
    case DISTRIBUTE:
        exit_status = distribute(args.filename, args.k, args.n, args.outputDir);
        if (exit_status == EXIT_FAILURE)
            printf(
                "Failed to distribute secret image %s into %d shares in %s\n",
                args.filename, args.n, args.outputDir);
        break;
    case RECOVER:
        exit_status = recover(args.filename, args.k, args.outputDir);
        if (exit_status == EXIT_FAILURE)
            printf("Failed to recover image %s from %d shares in %s\n",
                   args.filename, args.k, args.outputDir);
        break;
    }

    free_args(args);

    return exit_status;
}
