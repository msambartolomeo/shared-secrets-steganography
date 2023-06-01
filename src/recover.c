#include "recover.h"

void recover(char * filename, int k, char * directory){
    
    /*
    1. Abrimos directorio (ya chequeamos que hayan archivos)
    No chequeamos que sean .bmps

    2. Agarramos k imagenes cualquiera y por cada una parseamos como .bmp
    Nota: Chequear orden de sombras
    
    3. Extaremos ultimos 2/4 bits (dependiendo de k) de cada byte de cada imagen
    Appendeando todos los 2/4 bits que vamos recuperando, vamos a tener la sombra j Sj
    Recordemos estructura Sj = v1,j || v2,j || v3,j ...
    y a su vez vi,j = {mi,j; di,j}
    
    4. Hacemos dos arreglos: Uno con los mi,j (j es el número de sombra e i va entre 1 y número de bloques t)
    y otro con los di,j

    5. Tomamos puntos (1, m1,j), (2, m2,j)... e interpolamos con Lagrange  
    
    
    */


}
