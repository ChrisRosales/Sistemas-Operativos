#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include "exFATstruct.h"

int main(int n, char *name[]){
   int fd, exp;
   exFatBootSector boot;
   if(n!=3){
     printf("Uso %s <Image File System\n>", name[0]);
     exit(1);
   }
   if((fd = open(name[1], O_RDONLY)) < 0){
        perror("No se pudo abrir la imagen del disco\n");
        exit(1);
   }
    if(read(fd, &boot, sizeof(boot)) <0){
        perror("No se pudo leer el sector de arranque\n");
        exit(1);
   }
//ahora para las n entradas
    int bytes_por_sector = 1 << boot.BytePerSector;
    int sectores_por_cluster= 1 << boot.SectorPerCluster;
    int tamCluster= bytes_por_sector* sectores_por_cluster;
    int inicio_FAT = boot.FATOffset * bytes_por_sector;
//para posicionarme al inicio de la fat
    if(lseek(fd,inicio_FAT, SEEK_SET) < 0){
        perror("Error al posicionarse en la FAT\n");
        close(fd);
        exit(1);
    }
//leer entradas con malloc
    int num= atoi(name[2]);
    if (num <= 0) {
        printf("El número de entradas debe ser mayor que 0\n");
        exit(1);
    }
//manejo dinamico de memoria ya que es petición del enunciado 
    uint32_t *entradas= (uint32_t *)malloc(num * sizeof(uint32_t));
    if(entradas==NULL){
        perror("No hay memoria\n");
        close(fd);
        exit(1);
   }
   if(read(fd, entradas, num*sizeof(uint32_t))<0){
      perror("No se pueden leer las entradas");
        free(entradas);
        close(fd);
        exit(1);
   }
//aqui iniciaré a imprimir las n entradas de la FAT
   printf("Primeras %d entradas de la Fat:\n", num);
   for(int i=0; i<num ;i++){
      printf("FAT[%d] = 0x%08x\n", i, entradas[i]);
   }
   free(entradas);
   close(fd);
   return 0;
}

//    free(entradas);
//    close(fd);
//    return 0;
//}

