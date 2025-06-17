
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include "exFATstruct.h"

// Kevin Luis Lévano Cuzcano 20201865
void readFAT(const char *image_file) {
    int fd;
    exFatBootSector boot;

    if ((fd = open(image_file, O_RDONLY)) < 0) {
        perror("No se pudo abrir la imagen del disco\n");
        exit(1);
    }

    if (read(fd, &boot, sizeof(boot)) < 0) {
        perror("No se pudo leer el sector de arranque\n");
        close(fd);
        exit(1);
    }
    int bytes_per_sector = 1 << boot.BytePerSector; 
    long FAT_offset = boot.FATOffset * bytes_per_sector; 
    long FAT_size = boot.FATlen * bytes_per_sector; 

    if (lseek(fd, FAT_offset, SEEK_SET) < 0) {
        perror("Error al mover el puntero al inicio de la FAT\n");
        close(fd);
        exit(1);
    }

    int num_entries = FAT_size / 4; 
    uint32_t entry;

    printf("Entradas válidas de la FAT son:\n");
    for (int i = 0; i < num_entries; i++) {
        if (read(fd, &entry, sizeof(entry)) < 0) {
            perror("Error al leer una entrada de la FAT\n");
            close(fd);
            exit(1);
        }

        if (entry != 0x00000000 && entry != 0xFFFFFFFF) {
            printf("%u\n", entry);
        }
    }

    printf("-1\n");
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <imagen de disco>\n", argv[0]);
        exit(1);
    }

    readFAT(argv[1]);

    return 0;
}