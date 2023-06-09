#include "include/recover.h"
#include "include/bmp.h"
#include "include/shadow.h"
#include "include/steganography.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LENGTH 4096

int recover(char *filename, int k, char *directory) {

    /*
    1. Abrimos directorio (ya chequeamos que hayan archivos)
    No chequeamos que sean .bmps

    2. Agarramos k imagenes cualquiera y por cada una parseamos como .bmp
    Nota: Chequear orden de sombras

    3. Extaremos ultimos 2/4 bits (dependiendo de k) de cada byte de cada imagen
    Appendeando todos los 2/4 bits que vamos recuperando, vamos a tener la
    sombra j Sj Recordemos estructura Sj = v1,j || v2,j || v3,j ... y a su vez
    vi,j = {mi,j; di,j}

    4. Hacemos dos arreglos: Uno con los mi,j (j es el número de sombra e i va
    entre 1 y número de bloques t) y otro con los di,j

    5. Tomamos puntos (1, m1,j), (2, m2,j)... e interpolamos con Lagrange
    */

    DIR *dir;
    struct dirent *ent;

    Shadow *shadows = malloc(sizeof(Shadow) * k);
    if (shadows == NULL) {
        perror("Could not allocate memory for shadows.\n");
        return EXIT_FAILURE;
    }

    if ((dir = opendir(directory)) == NULL) {
        perror("Could not open directory.\n");
    }
    int i = 0;

    char path[MAX_PATH_LENGTH];
    BmpImage *new_img = malloc(sizeof(BmpImage));
    if (new_img == NULL) {
        free(shadows);
        perror("Could not allocate memory for new image.\n");
        return EXIT_FAILURE;
    }

    while ((ent = readdir(dir)) != NULL && i < k) {
        if (ent->d_type != DT_REG) {
            continue;
        }
        strcpy(path, directory);
        strcat(path, "/");
        strcat(path, ent->d_name);
        BmpImage *img = parse_bmp(path, k);
        if (img == NULL) {
            if (errno == EINVAL) {
                // Imágen no válida
                fprintf(stderr,
                        "File %s is not a valid bmp image, ignoring it.\n",
                        path);
                continue;
            } else {
                // Error de malloc, de apertura del archivo, o de lectura.
                fprintf(stderr, "Unexpected error while parsing file %s\n",
                        path);
                perror("Unexpected error.\n");
                free_bmp(new_img);
                closedir(dir);
                free_shadows(shadows, i);
                return EXIT_FAILURE;
            }
        }
        // Tomamos la primera imagen como base
        if (i == 0) {
            memcpy(new_img->header, img->header, HEADER_SIZE);
            new_img->extra = malloc(sizeof(uint8_t) * img->extra_size);
            if (new_img->extra == NULL) {
                perror("Malloc error.\n");
                exit(1);
            }
            memcpy(new_img->extra, img->extra, img->extra_size);
            new_img->extra_size = img->extra_size;
        }
        int shadow_size = (img->image_size) / (k - 1);
        shadows[i].size = shadow_size;
        shadows[i].idx = get_shadow_number(img);
        uint8_t *shadowRec =
            recoverShadow(img, shadow_size, k <= 4 ? LSB4 : LSB2);
        shadows[i++].bytes = shadowRec;
        free_bmp(img);
    }
    closedir(dir);

    if (i < k) {
        fprintf(stderr, "Not enough images to recover secret.\n");
        if (i == 0)
            free(new_img);
        else
            free_bmp(new_img);
        free_shadows(shadows, i);
        return EXIT_FAILURE;
    }

    Secret *secret = recover_secret(shadows, k);
    free_shadows(shadows, k);
    if (secret == NULL) {
        free(new_img->extra);
        free(new_img);
        return EXIT_FAILURE;
    }
    new_img->image = secret->bytes;
    new_img->image_size = secret->size;

    if (output_bmp(new_img, filename) != 0) {
        perror("Could not write output file.\n");
        free_bmp(new_img);
        free(secret);
        return EXIT_FAILURE;
    }
    free(secret);
    free_bmp(new_img);
    return EXIT_SUCCESS;
}
