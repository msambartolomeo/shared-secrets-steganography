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
#define RESERVED_INDEX 0x08


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

    Shadow* shadows = malloc(sizeof(Shadow) * k);

    if ((dir = opendir(directory)) != NULL) {
        int i = 0;

        char path[MAX_PATH_LENGTH];

        while ((ent = readdir(dir)) != NULL && i<k) {
            if (strcmp(ent->d_name, ".") == 0 ||
                strcmp(ent->d_name, "..") == 0) {
                continue;
            }
            strcpy(path, directory);
            strcat(path, "/");
            strcat(path, ent->d_name);
            BmpImage *img = parse_bmp(path, k);
            int shadow_size = (img->image_size) * 2 / (2*k-2);
            shadows[i].size = shadow_size;
            shadows[i].idx = img->header[RESERVED_INDEX];
            uint8_t* shadowRec = recoverShadow(img, shadow_size, k <= 4 ? LSB4 : LSB2);
            shadows[i++].bytes = shadowRec;
            free_bmp(img);
        }
        closedir(dir);
        uint8_t * secret = recover_secret(shadows, k);
        for(int i=0; i<10; i++) {
            printf("%d ", secret[i]);
        }
        free_shadows(shadows, k);
        free(secret);
    } else {
        perror("could not open directory");
    }

    return EXIT_SUCCESS;
}

