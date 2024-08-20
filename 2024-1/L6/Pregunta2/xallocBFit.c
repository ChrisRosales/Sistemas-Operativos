#include <unistd.h>
#include "xalloc.h"
#include <stdio.h>

typedef long Align;

union header {
    struct {
        union header *ptr;
        size_t size;
    } s;
    Align x;
};

typedef union header Header;

static Header base;
static Header *freep = NULL;

#define NALLOC 1024

static Header *morecore(size_t nu)
{
    char *cp;
    Header *up;

    if (nu < NALLOC)
        nu = NALLOC;
    cp = sbrk(nu * sizeof(Header));
    if (cp == (char *) -1)
        return NULL;
    up = (Header *) cp;
    up->s.size = nu;
    xfree((void *)(up + 1));
    return freep;
}

void *xmalloc(size_t nbytes)
{
    Header *p, *prevp, *bestp, *bestprevp; //aqui hacemos uso de 2 variables auxiliares, creando 2 best 
    size_t nunits;

    nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;

    if ((prevp = freep) == NULL) {
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }

    bestp = NULL; //inicializamos bestp
    bestprevp = NULL; //inicializamos nuestra segunda auxiliar

    for (p = prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
        if (p->s.size >= nunits) {
            if (bestp == NULL || p->s.size < bestp->s.size) {
                bestp = p; // aqui guardamos el mejor
                bestprevp = prevp; //guardamos el mejor previo
            }
        }
        if (p == freep)
            break;
    }

    if (bestp == NULL) {
        if ((p = morecore(nunits)) == NULL)
            return NULL;
        return xmalloc(nbytes);  // Llamar de nuevo a xmalloc después de solicitar más memoria
    }

    if (bestp->s.size == nunits)
        bestprevp->s.ptr = bestp->s.ptr;
    else {
        bestp->s.size -= nunits;
        bestp += bestp->s.size;
        bestp->s.size = nunits;
    }

    freep = bestprevp;
    return (void *)(bestp + 1); //devolvemos el mejor
}

void xfree(void *ap)
{
    Header *bp, *p;

    bp = (Header *)ap - 1;

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

void *xrealloc(void *ptr, size_t size)
{
    return NULL;
}

int sizeh()
{
    return sizeof(Header);
}

unsigned size(void *p)
{
    Header *ptr;
    ptr = (Header *)p - 1;
    return ptr->s.size;
}

void xprintq()
{
    Header *p;
    p = freep;
    do {
        printf("%p %ld \n", p, p->s.size);
        p = p->s.ptr;
    } while (p != freep);
}