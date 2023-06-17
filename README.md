## 72.04 - Criptografía y Seguridad
# Trabajo Práctico de Implementación: "Secreto Compartido con Esteganografía"

## Introducción
Este trabajo se centra en la *criptografía visual*, mediante la implementación de un algoritmo de Secreto Compartido para esconder un secreto en sombras, utilizando esteganografía.

Se trabajó con la implementación del paper [**“(k,n) secret image sharing scheme capable of cheating detection”**](https://jwcn-eurasipjournals.springeropen.com/articles/10.1186/s13638-018-1084-7) escrito por Yan-Xiao Liu, Quin-Dong Sun y Ching-Nung Yang de la Universidad de Tecnología de Xi’an (China).

El paper utiliza el [**Esquema de Shamir**](https://es.wikipedia.org/wiki/Esquema_de_Shamir) como algoritmo de secreto compartido, que propone un esquema (k,n), siendo *n* el número total de sombras y *k* el número mínimo de sombras necesarias para recuperar el secreto. 

Junto a la utilización de esteganografía para ocultar la información, describe como se puede distribuir una imagen secreta en otras imagenes, que funcionarán de sombras para el esquema de secreto compartido.

A su vez, describe como recuperar una imagen secreta a partir de estas sombras, mediante el uso del método interpolador de LaGrange (parte de los fundamentos del Esquema de Shamir).

## Implementación
Para este trabajo, se realizó el desarrollo en Lenguaje C11, y formateado con CLang. 

Nota: Las imagenes utilizadas deben tener formato *.bmp*. De no ser así, el programa no podrá esconder ni recuperar las imagenes.

## Requerimientos
Los requerimientos necesarios para correr este proyecto son:
- `make`
- `gcc`


## Compilación
Para compilar el programa, alcanza con posicionarse en el directorio raíz del proyecto, y correr el comando:
```
make all
```
Esto generará el ejecutable `shared-secrets-steganography` dentro de la carpeta `target`.

Para ejecutar el programa, tenemos las dos opciones mencionadas en el paper:

1. **Esconder una imagen utilizando un esquema (k,n)**:
```
./target/shared-secrets-steganography d <nombre-imagen.bmp> k <directorio donde se encuentran las imagenes para utilizar como sombras>
```
2. **Recuperar una imagen según un esquema (k,n)**:
```
./target/shared-secrets-steganography r <nombre-imagen.bmp> k <directorio donde se encuentran las imagenes para utilizar como sombras>
```

Algunas aclaraciones:
- La imagen a esconder (en el caso 1) como también la imagen donde se reconstruye el secreto deben encontrarse en la raíz del proyecto.
- La carpeta con las imagenes que se utilizarán como sombras deben tener **exactamente** n imagenes de extension *.bmp*
    - De tener mas, no se podrán distinguir que n imagenes se utilizaron
    - De tener menos, no se tienen suficientes imagenes para generar el esquema de secreto.
- Los archivos pueden tener un nombre con un máximo de 4096 caracteres de largo.
## Integrantes:

<center>

| Nombre   |      Legajo   |
|----------|:-------------:|
| [De Simone, Franco](https://github.com/desimonef) | 61100  |
| [Dizenhaus, Manuel](https://github.com/ManuelDizen) | 61101 |
| [Negro, Juan Manuel](https://github.com/j-negro) | 61225   |
| [Sambartolomeo, Mauro Daniel](https://github.com/msambartolomeo) | 61279 |

</center>