#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <math.h>
#include "xalloc.h"
//Christian Rosales 20201294
//la funcion del xrealloc la saque tmbn de mi paideia del ciclo pasado y tmbn me guie con la guia
void *xmalloc (size_t nbytes);
void *xrealloc(void *ptr,size_t size);
int main(void)
{ unsigned int x,unidad,base;
  unsigned int *pt,number=512;

  unidad=1024; 
  x=0;

  do {
    base=pow(2,x)+.5;
    if((pt=(unsigned int *)xmalloc(base*unidad))){
      fprintf(stdout,"Se solicitaron %d bytes y estan ubicados en %p antes del xrealloc\n",base*unidad,pt); 
      pt=xrealloc(pt,number*=4);
      fprintf(stdout,"Se solicitaron %d bytes y estan ubicados en %p después del xrealloc\n",base*unidad,pt);
    }else
       fprintf(stderr,"No hay suficiente memoria\n");       
    x++;
    
  }
  while(x<=6);      
  exit(0);                  
  
}
