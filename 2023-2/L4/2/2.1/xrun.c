#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <math.h>


#include "xalloc.h"


void *xmalloc (size_t nbytes);
int sizeh();
int size(void *p);
void xprintq();


int main(void)
{ unsigned int x,unidad,base;
  unsigned int *pt;

  unidad=1024; 
  x=0;

  printf("El tamaño del header es : %d bytes \n",sizeh());
  do {
    base=pow(2,x)+.5;
    if((pt=(unsigned int *)xmalloc(base*unidad))){ 
       fprintf(stdout,"Se solicitaron %d bytes Se proporcionaron %d headers ubicados en %p\n",base*unidad,size(pt),pt);
       xprintq();

    }  
    else
       fprintf(stderr,"No hay suficiente memoria\n");       
    x++; }
  while(x<=6);    

  
    
  exit(0);                  
  
}


