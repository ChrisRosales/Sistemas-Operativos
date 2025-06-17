/*
 * ssoo/xalloc.98/xalloc.h
 *
 * Prototipos de las funciones pedidas
 *
 * NO MODIFICAR ESTE FICHERO
 */
// joii
#if defined(NOXALL)

#include <malloc.h>
#define xmalloc(s) malloc(s)
#define xrealloc(p, s) realloc(p, s)
#define xfree(p) free(p)

#else

#include <stddef.h>
void *xmalloc(size_t size);
void *xrealloc(void * ptr, size_t size);
void xfree(void * ptr);

//Funciones preg2
void printTheQueueHF();
void printTheQueueHB();

#endif

