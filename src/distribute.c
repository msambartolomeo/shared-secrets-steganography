#include "include/distribute.h"
#include "bmp.h"
#include "shadow.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

void distribute(char *filename, int k, int n, char *directory) {
    BmpImage *secret_bmp = parse_bmp(filename, k);
    if (secret_bmp == NULL) {
        if (errno == EINVAL) {
            fprintf(stderr, "Secret image is not a valid bmp image");
        } else {
            perror("Error parsing secret image");
        }
        exit(EXIT_FAILURE);
    }
    uint8_t **shadows =
        image_processing(secret_bmp->image, secret_bmp->image_size, k, n);
    if (shadows == NULL) {
        fprintf(stderr, "for compilation\n");
    }

    /*
    Acá tengo las sombras Sj calculadas. Ahora, ¿que tengo que hacer?

    1) Abro directorio y consigo los n archivos
    2) Para cada archivo, agarro una sombra y
        2.1) Inserto, para cada byte de la imagen, en los ultimos dos bits, lo
    que corresponda de la sombra 2.2) Seteo en el header el número de sombra
    */

    // src:
    // https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(directory)) != NULL) {
        int i = 0;
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            /*Abro archivo y genero sombra*/
            BmpImage *img = parse_bmp(ent->d_name, k);
            // TODO: Me suena que hay que concatenar "directory"/"ent->d_name"
            if (hideShadowBytes(img, shadows[i++]) == -1) {
                fprintf(stderr, "Error in steganography process.\n");
                exit(EXIT_FAILURE);
            }
            /* Acá falta, teniendo el arreglo de bytes cambiado, guardarlo en el
               archivo original imagino que es un memcpy pero por las dudas no
               quiero meter la pata
            */
        }
        closedir(dir);
    } else {
        /* could not open directory */
        perror("");
        return EXIT_FAILURE;
    }

    free_bmp(secret_bmp);
}