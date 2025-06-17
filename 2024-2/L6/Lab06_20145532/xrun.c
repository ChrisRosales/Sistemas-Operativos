#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <math.h>
// joii
void *xmalloc (size_t nbytes);
void xfree(void *ap);
void printTheQueueHF();
void printTheQueueHB();

int main(void)
{ 
    unsigned int x, unidad, base;
    unsigned int *pt;

    unidad = 1024; 
    x = 0;

    do {
        base = pow(2, x) + 0.5;
        if ((pt = (unsigned int *)xmalloc(base * unidad))) {
            fprintf(stdout, "Se solicitaron %d bytes y están ubicados en %p\n", base * unidad, pt);
        } else {
            fprintf(stderr, "No hay suficiente memoria\n");       
        }
        x++; 
    } while (x <= 8);

    printf("\nEstado de la cola de huecos libres:\n");
    printTheQueueHF();

    printf("\nEstado de la cola de huecos ocupados:\n");
    printTheQueueHB();

    // Liberar el último bloque y verificar el estado de las listas
    printf("\nSe libera el ultimo bloque...\n\n");
    xfree(pt);
    
    printf("Estado de la cola de huecos libres:\n");
    printTheQueueHF();

    printf("\nEstado de la cola de huecos ocupados:\n");
    printTheQueueHB();

    exit(0); 
}


