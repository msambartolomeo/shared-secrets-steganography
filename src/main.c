#include "bmp.h"
#include "include/test.h"
#include <stdio.h>

int main() {
    printf("Hello World\n");
    BmpImage *bmp = parse_bmp("/home/mauro/Downloads/albert-share.bmp", 3);

    free_bmp(bmp);
}
