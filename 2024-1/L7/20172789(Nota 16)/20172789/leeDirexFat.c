#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include "exFATstruct.h"

#define SIZE 32

void printName(const char *entry){
    char name[12];
    memcpy(name, entry, 11);
    name[11] = '\0';
    printf("%s\n", name);
}

void readRootDir(const char *image_file){
    int fd, n1, n2;
    exFatBootSector boot;
    char entry[SIZE];

    if((fd = open(image_file, O_RDONLY)) < 0){
        perror("No se pudo abrir la imagen del disco\n");
        exit(1);
    }

    if(read(fd, &boot, sizeof(boot) ) < 0){
        perror("No se pudo leer el sector de arranque del disco\n");
        exit(1);
    }

    printf("La data inicia en el sector: %d\n", boot.ClusterHeapOffset);
    n1 = boot.BytePerSector;
    printf("El tamaño de un sector en bytes es de: %d\n", (int)pow(2.0, n1));
    n2 = boot.SectorPerCluster;
    printf("El tamaño de sectores por cluster es de: %d\n", (int)pow(2.0, n2));
    printf("El directorio raiz se encuentra en el cluster: %d\n", boot.RootDirFirstCluster);

    int bytes_per_sector = 1 << boot.BytePerSector;
    int sectors_per_cluster = 1 << boot.SectorPerCluster;

    long rootDirOffset = (boot.ClusterHeapOffset * bytes_per_sector)+(boot.RootDirFirstCluster * bytes_per_sector * sectors_per_cluster);

    if(lseek(fd, rootDirOffset, SEEK_SET)< 0){
        perror("Error al mover el puntero del archivo al inicio del del roorDir\n");
        close(fd);
        exit(1);
    }

    //printf("Estamos en la posicion: %ld\n", rootDirOffset);
    
    while(read(fd, entry, SIZE) == SIZE){
        if(entry[0] == 0x00 || entry[0] == 0x80){
            continue;
        }
        printName(entry);
    }
    
    close(fd);
    return;    
}



void main(int argc, char *argv[]){
    int fd;

    char *image_file = argv[1];
    
    readRootDir(image_file);

    exit(1);
}
