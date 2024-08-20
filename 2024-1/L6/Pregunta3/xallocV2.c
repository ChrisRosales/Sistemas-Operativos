
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

#include <string.h>
#include <stdio.h>
#include "xalloc.h"
#include <unistd.h>
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
static Header base;   /* empty list to get started */

/*
 * freep apuntara al hueco situado en la lista antes del hueco
 * por el que comenzara la busqueda.
 * Necesario para implementar la estrategia next-fit
 */
static Header  *freep = NULL;  /* start of the free list */


#define NALLOC 1024

/*
 * morecore: ask system for more memory 
 *
 * Esta funcion se llama desde xmalloc cuando no hay espacio.
 * Aumenta el tama~no de la zona de datos como minimo en NALLOC*sizeof(Header)
 * y a~nade esta nueva zona a la lista de huecos usando xfree.
 *
 */
//guiandonos de la guia de este ciclo y de mi paideia de el ciclo pasado:
size_t numSizeOfTypeInBlock(size_t nu){

	size_t val=sizeof(Header)/nu;
	if(sizeof(Header)%nu!=0)val++;
	return val;
}
static Header *morecore(size_t nu)
{
	char *cp;
	Header *up;
	size_t nAlig = numSizeOfTypeInBlock(sizeof(Align));
	if (nu < NALLOC)	
		nu = NALLOC;
	cp= sbrk(nu * sizeof(Align)*nAlig);
	if (cp == (char *) -1) /* no space at all */
		return NULL;
	up = (Header *) cp;
	up ->s.size = nu;
	xfree((void *)(up+1));
	return freep;
}

/* xmalloc: general-purpose storage allocator */
void *xmalloc (size_t nbytes)
{
	Header  *p, *prevp;
	size_t nunits,nAligns;
	nAligns = numSizeOfTypeInBlock(sizeof(Align));

	/* 
	   Calcula cuanto ocupara la peticion medido en tama~nos de
	   Align (incluyendo la propia cabecera). 
	   El termino "sizeof(Header)-1" provoca un redondeo por exceso.
	   El termino "+ 1" es para incluir la propia cabecera.
	*/
	nunits = (nbytes+sizeof(Align)*nAligns-1)/(sizeof(Align)*nAligns) + 1;

	//nunits_byte = nunits*sizeof(Align);
	/* En la primera llamada se construye una lista de huecos con un
	   unico elemento de tama~no cero (base) que se apunta a si mismo */
	if (( prevp = freep) == NULL ) { /* no free list yet */
		base.s.ptr = freep = prevp = & base; 
		base.s.size = 0;
	}

	/*
	   Recorre la lista circular de huecos, empezando por el siguiente al
	   que apunta freep, hasta que encuentra uno que satisface la peticion
	   o da toda una vuelta a la lista (no hay espacio suficiente)
	*/

	for (p= prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
		//revisar línea 110
		if (p->s.size >= nunits + nAligns) {  /* big enough */
			if (p->s.size == nunits)  /* exactly */
				prevp->s.ptr = p->s.ptr;
			else {  /* allocate tail end */
				p->s.size -= nunits;
				p+= p->s.size;
				p->s.size = nunits;
			}
			freep = prevp; /* estrategia next-fit */
			return (void *)(p+1); /* devuelve un puntero a la
						 zona de datos del bloque */
		}
		/* Si ha dado toda la vuelta pide mas memoria y vuelve
		   a empezar */
		if (p == freep) /* wrapped around free list */
			if ((p = morecore(nunits*sizeof(Align))) == NULL)
				return NULL;  /* none left */
	}
}


/* xfree: put block ap in the free list */
void xfree(void *ap)
{
	Header *bp, *p;

	bp = (Header *)ap - 1;  /* point to block header */

	/*
	   Bucle que recorre la lista de huecos para buscar el hueco
	   anterior al bloque que se va a liberar.
	   Del bucle se sale cuando se encuentran los dos huecos
	   de la lista (el apuntado por p y el apuntado por p->s.ptr)
	   entre los que se incluira el nuevo hueco (el apuntado por bp).
	   Hay dos casos:
		- La direccion del nuevo hueco es mayor (bp > p) o
		menor (bp < p->s.ptr) que la de ningun otro hueco de la
		lista (corresponde al break)
		- La direccion del nuevo hueco esta comprendida entre
		dos huecos de la lista (corresponde a la salida normal
		del for)
	*/

	for (p= freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
		if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
			break;  /* freed block at start or end of arena */ 

	/* Comprueba compactacion con hueco posterior */
	if (bp + bp->s.size == p->s.ptr) {  /* join to upper nbr */
		bp->s.size += p->s.ptr->s.size;
		bp->s.ptr = p->s.ptr->s.ptr;
	} else
		 bp->s.ptr = p->s.ptr;

	/* Comprueba compactacion con hueco anterior */
	if (p + p->s.size == bp) {         /* join to lower nbr */
		p->s.size += bp->s.size;
		p->s.ptr = bp->s.ptr;
	} else
		p->s.ptr = bp;

	freep = p; /* estrategia next-fit */
}

void *xrealloc(void * ptr, size_t size)
{
	//size contiene el número de bytes a reservar, osea no contiene la cabecera
	//nuestro puntero sí incluye la cabecera
	//Teniendo en cuenta que en la guia estaba el ejercicio de implementar el xrealloc,
	//procederemos a implementarlo de manera similar a como la hemos implementado.
	if(ptr==NULL)return NULL;
	//printf("Llegue");
	Header *p,*prevp,*new_ptr;
	size_t nunits,nAligns;
	nAligns = numSizeOfTypeInBlock(sizeof(Align));

	//convertimos el número de bytes a número de bloques con cabecera incluida
	nunits = (size+sizeof(Align)*nAligns-1)/(sizeof(Align)*nAligns) + 1;
	
	new_ptr = (Header *)ptr - 1;
	// se quiere disminuir el tamaño del bloque


	Header *bp;
	bp = (Header *)ptr - 1; 
	//encontramos el hueco siguiente y el anterior al proceso actual

	p= freep;
	if(!(bp > p && bp < p->s.ptr)|(p >= p->s.ptr && (bp > p || bp < p->s.ptr))){
		p=p->s.ptr;
	}

	for (; !(bp > p && bp < p->s.ptr);prevp = p, p = p->s.ptr)
		if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
			break;

	Header *next_block=p->s.ptr;

	if(nunits +1 <= new_ptr->s.size){
		//printf("Hola\n");
		//printf("%d %d\n\n",nunits +1,new_ptr->s.size);
		if(nunits+1==new_ptr->s.size&&new_ptr->s.ptr==new_ptr+new_ptr->s.size)return ptr;

		
		Header* new_hole;

		new_hole = new_ptr+nunits;
		new_hole->s.size=new_ptr->s.size-nunits;
		new_hole->s.ptr=p->s.ptr;
		p->s.ptr=new_hole;
		
		new_ptr->s.size=nunits;

		xfree((void*)(new_hole));

		return (void*)(new_ptr + 1);
	}

	//se va a aumentar el tamaño del bloque

	
	if(p->s.ptr==new_ptr+new_ptr->s.size &&new_ptr->s.size+next_block->s.size>nunits){
		//si el hueco adyacente superior, más el bloque actual pueden satisfacer la solicitud
		//de expansión de memoria
		//posicionamos al puntero en el espacio que le corresponde al nuevo bloque
		next_block+=nunits;
		new_ptr->s.size=nunits;
		//le disminuimos el tamaño al hueco y hacemos que apunte al siguiente 
		//hueco del bloque hueco antes de disminuir en tamaño
		next_block->s.size=p->s.ptr->s.size-(nunits-new_ptr->s.size);
		next_block->s.ptr=p->s.ptr;
		return (void*)(new_ptr + 1);
		
	}else if(p+p->s.size==new_ptr &&p->s.size+new_ptr->s.size>nunits){
		//printf("Hola");
		

		//existe un hueco en la parte posterior y puede reubicarse ahí
		size_t blockDiff = nunits - new_ptr->s.size;
		Header *aux;
		//guardamos el puntero a donde está la información

		aux = new_ptr;
		//nos movemos para abarcar el nuevo espacio y actualizamos el size del bloque
		p->s.size-= blockDiff;
		new_ptr-=blockDiff;
		new_ptr->s.size=nunits;
		memcpy(new_ptr,aux,aux->s.size*sizeof(Align));
		//si el bloque alcanza exacto para el realloc, el puntero anterior al p
		//debe apuntar al siguiente del p
		if(p->s.size+new_ptr->s.size==nunits+1){
			prevp->s.ptr=p->s.ptr;
		}		
		return (void*)(new_ptr + 1);
	}else if(p->s.ptr==new_ptr+new_ptr->s.size&& p+p->s.size==new_ptr){
		
		//existen huecos adyacente por la derecha e izquierda

		//validamos que tengan tamaño suficiente
		if(p->s.size+next_block->s.size+new_ptr->s.size>=nunits+1){
			size_t blockDiffBack=nunits-(next_block->s.size + new_ptr->s.size);
			Header *aux;
			aux = new_ptr;
			//nos posicionamos en el nuevo lugar reservado
			new_ptr-=blockDiffBack;

			//copiamos toda la información 
			memcpy(new_ptr,aux,aux->s.size*sizeof(Align));

			//actualizamos el size del bloque anterior
			p->s.size-=blockDiffBack;
			new_ptr->s.size=nunits;
			//vemos cuando el hueco es exacto y cuando no lo es
			if(p->s.size+next_block->s.size+new_ptr->s.size>nunits){
				//los huecos juntos satisfacen más de la memoria pedida
				p->s.ptr=next_block->s.ptr;
			}else{
				prevp->s.ptr=next_block->s.ptr;
			}
			
			return (void*)(new_ptr + 1);
		}
	}

	//no se puedo reubicar en la parte posterior/superior

	Header *aux;
	aux = new_ptr;
	new_ptr=(Header*)xmalloc(nunits*sizeof(Align));
	memset(new_ptr,0,nunits*sizeof(Align));
	if(new_ptr)memcpy(new_ptr,aux,aux->s.size*sizeof(Align));
	
	return (void*)(new_ptr + 1);
}
