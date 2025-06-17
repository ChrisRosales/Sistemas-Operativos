#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include "exFATstruct.h" 

void mostrarMapaBit(const char *image_file) {
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
    long bitmap_offset = boot.ClusterHeapOffset * bytes_per_sector;
    int bitmap_size = (boot.ClusterCount + 7) / 8;

    if (lseek(fd, bitmap_offset, SEEK_SET) < 0) {
        perror("Error al mover el puntero al inicio del BitMap\n");
        close(fd);
        exit(1);
    }

    uint8_t *bitmap = malloc(bitmap_size);
    if (!bitmap) {
        perror("Error al asignar memoria para el BitMap\n");
        close(fd);
        exit(1);
    }

    if (read(fd, bitmap, bitmap_size) < 0) {
        perror("Error al leer el BitMap\n");
        free(bitmap);
        close(fd);
        exit(1);
    }

    printf("Contenido del BitMap en hexadecimal:\n");
    for (int i = 0; i < bitmap_size; i++) {
        printf("%x ", bitmap[i]);
    }
    printf("\n");

    free(bitmap);
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <imagen de disco>\n", argv[0]);
        exit(1);
    }

    mostrarMapaBit(argv[1]);

    return 0;
}
