#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <math.h>
#include "xalloc.h"
#include <stddef.h>

void *xmalloc (size_t nbytes);

int main(void) {
    printf("El tamaño del header es: %d bytes\n", sizeh());

    // Realiza una serie de asignaciones
    void *a = xmalloc(1024);  // Hacemos uso del xmalloc: 
    void *b = xmalloc(2048);
    void *c = xmalloc(4096);
    void *d = xmalloc(8192);
   
    // Ahora, libera algunos de los bloques para asegurar que al menos 3 queden libres al final
    xfree(b);
    xfree(d);

    // Finalmente, muestra el estado de la lista de bloques, haciendo uso de la funcion que hemos creado 
    // con anterioridad: 
    xprintq(); //mostramos la lista y vemos que es secuencial y cumple con el next fit

    exit(0);
} 

