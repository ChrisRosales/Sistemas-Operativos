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
//programa para imprimir el Allocation Bitmap Table
int main(int n, char *name[]){
   int fd, exp;
   exFatBootSector boot;
   if(n!=2){
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
  ///operaciones para llegar al bitmap
    int bytes_per_sector = (int)pow(2, boot.BytePerSector);
    int sectors_per_cluster = (int)pow(2, boot.SectorPerCluster);
    // Cluster donde esta el Bitmap
    //uint32_t allocation_bitmap_cluster = 2;
    // Calcular offset en bytes para el cluster 2 (Allocation Bitmap)
    off_t bitmap_offset = (boot.ClusterHeapOffset)* bytes_per_sector;

    // Calcular tamaño del Bitmap en bytes (1 bit por cluster)
    int bitmap_size = (boot.ClusterCount + 7) / 8;

    if (lseek(fd, bitmap_offset, SEEK_SET) < 0) {
        perror("Error posicionando en la tabla Bitmap");
        close(fd);
        return 1;
    }
    unsigned char *buffer = malloc(bitmap_size);
    if (!buffer) {
        perror("No se pudo asignar memoria");
        close(fd);
        return 1;
    }
    if (read(fd, buffer, bitmap_size) < 0) {
        perror("No se pudo leer la tabla Bitmap");
        free(buffer);
        close(fd);
        return 1;
    }
    printf("Contenido del Bitmap (en hexadecimal):\n");
    //se piden solo los 32 primeros bytes
    for (int i = 0; i < 32; i++) {
        printf("%x ", buffer[i]);
        if ((i + 1) % 4 == 0) printf("\n");  // Para un mejor orden
    }
    printf("\n");
    free(buffer);
    close(fd);

   return 0;
}


