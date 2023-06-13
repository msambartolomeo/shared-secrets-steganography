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

uint8_t * recover_secret(Shadow * shadows, size_t k);

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

        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 ||
                strcmp(ent->d_name, "..") == 0) {
                continue;
            }
            strcpy(path, directory);
            strcat(path, "/");
            strcat(path, ent->d_name);
            BmpImage *img = parse_bmp(path, k);
            int shadow_size = (img->extra_size + img->image_size) * 2 / (2*k-2);
            shadows[i].size = shadow_size;
            shadows[i].idx = img->header[RESERVED_INDEX];
            uint8_t* shadowRec = recoverShadow(img, shadow_size, k <= 4 ? LSB4 : LSB2);
            shadows[i++].bytes = shadowRec;
            free_bmp(img);
            //hacer los frees!
        }
        closedir(dir);
        //uint8_t * secret = recover_secret(shadows);
    } else {
        perror("could not open directory");
    }

    return EXIT_SUCCESS;
}

uint8_t * recover_secret(Shadow * shadows, size_t k) {

    size_t block_count = IMAGE_SIZE * 2 / (2*k-2);

    uint8_t* secret = NULL;

    for(size_t i=0; i<block_count; i++) {
        v_ij* vs = malloc(sizeof(v_ij) * k);
        for(size_t j=0; j<k; j++) {
            vs[j].m = shadows[j].bytes[i*2];
            vs[j].d = shadows[j].bytes[(i*2)+1];
        }
    }

    return secret;

    //cómo recupero el bloque i? --> necesito tomar el iésimo par de bytes de cada sombra

    //tomo el nro de sombra


}
