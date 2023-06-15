#include "include/distribute.h"
#include "include/bmp.h"
#include "include/shadow.h"
#include "include/steganography.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LENGTH 4096

int distribute(char *filename, int k, int n, char *directory) {
    BmpImage *secret_bmp = parse_bmp(filename, k);
    if (secret_bmp == NULL) {
        if (errno == EINVAL) {
            fprintf(stderr, "Secret image is not a valid bmp image");
        } else {
            perror("Error parsing secret image");
        }
        return EXIT_FAILURE;
    }
    Shadow *shadows =
        image_processing(secret_bmp->image, secret_bmp->image_size, k, n);
    if (shadows == NULL) {
        free_bmp(secret_bmp);
        fprintf(stderr, "for compilation\n");
        return EXIT_FAILURE;
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

        char path[MAX_PATH_LENGTH];

        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type != DT_REG) {
                continue;
            }
            strcpy(path, directory);
            strcat(path, "/");
            strcat(path, ent->d_name);
            BmpImage *img = parse_bmp(path, k);
            if (hideShadowBytes(img, &shadows[i], k <= 4 ? LSB4 : LSB2) == -1) {
                free_bmp(img);
                fprintf(stderr, "Error in steganography process.\n");
                return EXIT_FAILURE;
            }
            save_shadow_number(img, shadows[i++].idx);

            if (output_bmp(img, path) != 0) {
                free_bmp(img);
                perror("");
                return EXIT_FAILURE;
            }
            free_bmp(img);
        }
        closedir(dir);
    } else {
        perror("could not open directory");
    }

    free_shadows(shadows, n);
    free_bmp(secret_bmp);

    return EXIT_SUCCESS;
}