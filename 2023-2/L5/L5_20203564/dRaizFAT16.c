#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "structfat.h"

// MAX JACINTO MESTANZA - 20203564

int main(int argc, char *argv[]) {
    int fd;

    // Verificamos que la imagen esté siendo incluída en el argumento
    if (argc != 2) {
        printf("Usage: %s <name image disc>\n", argv[0]);
        exit(1);
    }

    // Verificamos que se pueda abrir la imagen de disco;
    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        perror("Error al abrir la imagen de disco");
        exit(1);
    }

    // Debemos leer el sector de arranque
    Fat16BootSector boot;
    if (read(fd, &boot, sizeof(boot)) != sizeof(boot)) {
        perror("Error al leer el sector de arranque");
        close(fd);
        exit(1);
    }

    // Hallamos el offset del directorio raíz
    int offsetDirRaiz = (boot.reserved_sectors + boot.number_of_fats * boot.fat_size_sectors) * boot.sector_size;
    if (lseek(fd, offsetDirRaiz, SEEK_SET) == -1) {
        perror("Error al buscar en el directorio raíz");
        close(fd);
        exit(1);
    }
    
    // Una vez hallado el sector de arranque y el offset del directo raíz, pasamos a lo que nos piden hallar
    // Leemos e imprimimos las entradas del directorio raíz
    DirEntry entry;
    int cont = 0; // Este contador sirve solo para poder enumerar el número de archivos correctamente :)
    for (int i = 0; i < 512; i++) {
        if (read(fd, &entry, sizeof(entry)) != sizeof(entry)) {
            perror("Error al leer una entrada del directorio");
            close(fd);
            exit(1);
        }
        // Descartamos entradas borradas y etiquetas de volumen
        if (entry.filename[0] != 0xE5 && entry.filename[0] != 0x00 && (entry.attributes & 0x08) == 0) {
            cont++;
            printf("%d) Archivo: %.8s.%.3s\n", cont, entry.filename, entry.ext);
        }
    }

    close(fd);
    return 0;
}