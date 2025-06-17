
/*
 * ssoo/xalloc.98/xalloc.c
 *
 * CONTENIDO INICIAL:
 *	Codigo correspondiente a la Seccion 8.7 del libro:
 *	"The C Programing Language", de B. Kernigham y D. Ritchie.
 *
 * En este fichero se incluiran las rutinas pedidas 
 *  
 */
// joii
#include <unistd.h>
#include "xalloc.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Definicion de la cabecera para huecos y bloques. 
 * La union con un campo de tipo Align fuerza que el tama~no
 * de la cabecera sea multiplo del tama~no de este tipo.
 */
typedef long Align;    /* for alignment to long boundary */

union header {   /* block header: */
	struct {
		union header *ptr;  /* next block if on free list */
		size_t size;     /* size of this block */
	} s;
	Align x;             /* force alignment of blocks */
};

typedef union header Header;

/*
 * La lista de huecos esta ordenada por direcciones y es circular.
 * base es el "falso" hueco de tama~no cero que asegura que la lista
 * nunca esta vacia 
 */

static Header base;   /* empty list to get started */  //Nodo base de la lista libre
static Header base_ocupada;   //Nodo base de la lista de bloques ocupados

static Header *freep = NULL;
static Header *ocupadop = NULL; //Apunta al inicio de la lista de bloques ocupados

#define NALLOC 1024
#define MEMORY_POOL_SIZE (1024 * 1024) //1MB
static char memory_pool[MEMORY_POOL_SIZE];
static char *posicion_actual = memory_pool; //Apuntamos al inicio del pool

static Header *morecore(size_t nu)
{
	//En caso no tengamos suficiente espacio en memoria
	if ((posicion_actual + nu * sizeof(Header)) > (memory_pool + MEMORY_POOL_SIZE)) {
        return NULL;
    }

    Header *up = (Header *)posicion_actual;
    posicion_actual += nu * sizeof(Header);

    up->s.size = nu;
    xfree((void *)(up + 1));
    return freep;
}

/* xmalloc: general-purpose storage allocator */
void *xmalloc (size_t nbytes)
{
	Header *p, *prevp;
    size_t nunits;

    nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;

    if ((prevp = freep) == NULL) {
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }

    for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
        if (p->s.size >= nunits) {
            if (p->s.size == nunits) 
                prevp->s.ptr = p->s.ptr;
            else {
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            freep = prevp;

            //Se agrega bloque a la lista de ocupados
            if (ocupadop == NULL) {
                base_ocupada.s.ptr = &base_ocupada;
                base_ocupada.s.size = 0;
                ocupadop = &base_ocupada;
            }

            Header *ocupado_prev = ocupadop;
            Header *ocupado_nuevo = p;
            ocupado_nuevo->s.ptr = ocupado_prev->s.ptr;
            ocupado_prev->s.ptr = ocupado_nuevo;

            return (void *)(p + 1);
        }
        if (p == freep)
            if ((p = morecore(nunits)) == NULL)
                return NULL;
    }
}

/* xfree: put block ap in the free list */
void xfree(void *ap)
{
	Header *bp, *p;

    bp = (Header *)ap - 1;

    //Quitamos bloque de la lista de ocupados
    if (ocupadop != NULL) {
        Header *ocupado_prev = ocupadop;
        Header *busyp = ocupadop->s.ptr;

        do {
            if (busyp == bp) {
                ocupado_prev->s.ptr = busyp->s.ptr;
                if (busyp == ocupadop) {
                	if (ocupadop == busyp->s.ptr) ocupadop = NULL;
                	else	ocupadop = busyp->s.ptr;
                }
                break;
            }
            ocupado_prev = ocupadop;
            busyp = busyp->s.ptr;
        } while (busyp != ocupadop && busyp != &base_ocupada);
    }

    // Agregar a la lista de huecos libres
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
            break;

    if (bp + bp->s.size == p->s.ptr) {
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    } else
        bp->s.ptr = p->s.ptr;

    if (p + p->s.size == bp) {
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    } else
        p->s.ptr = bp;

    freep = p;
}

void *xrealloc(void * ptr, size_t size)
{
	return NULL;
}

void printTheQueueHF() {
    if (freep == NULL) {
        printf("No hay huecos libres!\n");
        return;
    }
    Header *p = freep;
    do {
        printf("Hueco libre en: %p, tamaño: %lu headers\n", (void *)p, p->s.size);
        p = p->s.ptr;
    } while (p != freep);
}

void printTheQueueHB() {
    if (ocupadop == NULL || ocupadop->s.ptr == &base_ocupada) {
        printf("No hay huecos ocupados!\n");
        return;
    }
    Header *p = ocupadop->s.ptr;
    do {
        if (p != &base_ocupada) {
            printf("Hueco ocupado en: %p, tamaño: %lu headers\n", (void *)p, p->s.size);
        }
        p = p->s.ptr;
    } while (p != ocupadop->s.ptr);
}