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
{ unsigned int unidad;
  unsigned int *pt[4];

  unidad=1024; 
  
  for(int i=0;i<4;i++){
      if((pt[i]=(unsigned int *)xmalloc(unidad))){ 
       fprintf(stdout,"Se solicitaron %d bytes Se proporcionaron %d headers ubicados en %p\n",unidad,size(pt[i]),pt);
       xprintq();

    }     
  }
     
  //Se libra uno de los bloques y como no hay espacios vacios adelante o atras de el
  //No se compacta y se crea un nuevo bloque vacio
  //Cuando liberamos 2, se crean dos de estos bloques vacios separados, más uno que teniamos 
  //del bucle for.
  
  xfree(pt[0]);
  //xprintq();
  printf("\n");


  xfree(pt[2]);
  xprintq();
  printf("\n");

  
    
  exit(0);                  
  
}


