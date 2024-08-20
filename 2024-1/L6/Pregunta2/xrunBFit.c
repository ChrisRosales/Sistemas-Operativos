#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <math.h>
#include "xalloc.h"
//Christian Rosales 20201294
void *xmalloc (size_t nbytes);

int main(void)
{                 
  // Realiza una serie de asignaciones
    void *a = xmalloc(1024);  // Hacemos uso del xmalloc: 
    void *b = xmalloc(2048);  //tenemos que modificar el xmalloc para que sea best fit y no next fit
    void *c = xmalloc(4096);
    void *d = xmalloc(8192);
   
    // Ahora, libera algunos de los bloques para asegurar que al menos 3 queden libres al final
    xfree(b);
    xfree(d);

    // Finalmente, muestra el estado de la lista de bloques, haciendo uso de la funcion que hemos creado 
    // con anterioridad: 
    xprintq(); //mostramos la lista y vemos que cumple con el best fit
}


