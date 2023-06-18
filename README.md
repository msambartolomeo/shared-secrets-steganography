## 72.04 - Criptografía y Seguridad

# Trabajo Práctico de Implementación: "Secreto Compartido con Esteganografía"

## Integrantes:

<center>

| Nombre                                                           | Legajo |
| ---------------------------------------------------------------- | :----: |
| [De Simone, Franco](https://github.com/desimonef)                | 61100  |
| [Dizenhaus, Manuel](https://github.com/ManuelDizen)              | 61101  |
| [Negro, Juan Manuel](https://github.com/j-negro)                 | 61225  |
| [Sambartolomeo, Mauro Daniel](https://github.com/msambartolomeo) | 61279  |

</center>

## Introducción

Este trabajo se centra en la _criptografía visual_, mediante la implementación de un algoritmo de Secreto Compartido para esconder un secreto en sombras, utilizando esteganografía.

Se trabajó con la implementación del _paper_ [**“_(k,n) secret image sharing scheme capable of cheating detection_”**](https://jwcn-eurasipjournals.springeropen.com/articles/10.1186/s13638-018-1084-7) escrito por Yan-Xiao Liu, Quin-Dong Sun y Ching-Nung Yang de la Universidad de Tecnología de Xi’an (China).

El _paper_ utiliza el [**Esquema de Shamir**](https://es.wikipedia.org/wiki/Esquema_de_Shamir) como algoritmo de secreto compartido, que propone un esquema _(k,n)_, siendo _n_ el número total de sombras y _k_ el número mínimo de sombras necesarias para recuperar el secreto.

Junto a la utilización de esteganografía para ocultar la información, describe cómo se puede distribuir una imagen secreta en otras imágenes, que funcionarán de sombras para el esquema de secreto compartido.

A su vez, describe cómo recuperar una imagen secreta a partir de estas sombras, mediante el **método interpolador de Lagrange** (parte de los fundamentos del Esquema de Shamir).

## Implementación

Para este trabajo, se realizó el desarrollo en el lenguaje C11, y formateado con CLang.

## Requerimientos

Los requerimientos necesarios para correr este proyecto son:

- `make`
- `gcc`

## Compilación

Para compilar el programa, alcanza con posicionarse en el directorio raíz del proyecto, y correr el comando:

```
make all
```

Esto generará el ejecutable `shared_secret_steganography` dentro de la carpeta `target`.

## Ejecución

El programa presenta dos modos de ejecición distintas:

- Distribuir una imagen secreta en sombras
- Recuperar una imagen secreta a partir de las sombras

### **Distribuir una imagen secreta en sombras**:

Este modo de ejecución se realiza de la siguiente manera:

```
./target/shared_secret_steganography d <nombre-imagen.bmp> k <directorio donde se encuentran las imágenes a utilizar como sombras>
```

El programa leerá la imagen a esconder y generará `n` sombras, donde `n` es la cantidad de imágenes que se encuentren en el directorio especificado. Cada una de estas sombras tendrá un tamaño de $(k-1)^{-1}$ veces el tamaño de la imagen secreta, y se guardará ocultada en la imagen correspondiente.

Este método de ocultamiento se realiza mediante el método de esteganografía **_LSB_** (_Least Significant Bit_), donde se modifican los _X_ bits menos significativos de cada byte. En particular, se utiliza _LSB4_ para valores de `k` iguales a 3 o 4, y _LSB2_ para valores entre 5 y 8. De esta forma garantizamos que siempre se pueda ocultar cada sombra en la imagen portadora correspondiente.

Por último, se añade un número de sombra en la sección adicional del formato _.bmp_ de cada imagen portadora, para poder identificar qué sombra se encuentra en cada imagen. Esto se hace en el el _offset_ `0x08` del archivo, un espacio reservado para la aplicación creadora de la imagen.

### **Recuperar una imagen secreta a partir de las sombras**:

Este modo de ejecución se realiza de la siguiente manera:

```
./target/shared_secret_steganography r <nombre-imagen.bmp> k <directorio donde se encuentran las imágenes a utilizar como sombras>
```

El programa leerá el directorio de imágenes portadoras y tomará los primeros `k` archivos que encuentre, ignorando directorios y archivos que no sean _.bmp_. Luego, generará los polinomios descriptos en el _paper_ y evaluará el caso de _Cheating_, donde fallará si se detecta alguna sombra inválida. Luego, se reconstruirá la imagen secreta a partir de las sombras, y se guardará en el nombre de imagen especificado.

## Aclaraciones:

- La carpeta con las imágenes que se utilizarán como sombras deben tener **exactamente** `n` imagenes de extension _.bmp_, con `n` mayor o igual a `k`, pues se utilizarán todas las imágenes para construir portadoras.
- La longitud máxima de los _paths_ otorgados para identificar imágenes o directorios es de 4096 caracteres.
- Las imágenes utilizadas deben tener formato _.bmp_. De no ser así, el programa no podrá esconder ni recuperar las imágenes.
- Tanto al distribuir la imagen secreta como al recuperarla, se ignorará todo lo que no sea un archivo _.bmp_ en el directorio especificado, incluyendo subdirectorios y archivos de otros formatos.
