#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "structfat.h"

// MAX JACINTO MESTANZA - 20203564

// Función para leer la siguiente entrada en la FAT y obtener el siguiente clúster.
unsigned short sig_clister(int fd, short cluster_act, const Fat16BootSector *boot) {
    unsigned int fatOffset = boot->reserved_sectors * boot->sector_size;
    unsigned int entryOffset = fatOffset + (cluster_act * 2); // Se asignan 2 bytes por el tipo de entrada en FAT16
    unsigned short cluster_siguiente;
   
    lseek(fd, entryOffset, SEEK_SET);
    if (read(fd, &cluster_siguiente, sizeof(cluster_siguiente)) != sizeof(cluster_siguiente)) {
        perror("Error al leer la entrada de la FAT");
        exit(EXIT_FAILURE);
    }
   
    return cluster_siguiente;
}

// Función para extraer el archivo dado una entrada de directorio.
void extraer_img(int fd, const Fat16BootSector *boot, const DirEntry *entry, const char *arch_nomb) {
    unsigned int clusterSize = boot->sectors_per_cluster * boot->sector_size;
    unsigned int dataStart = (boot->reserved_sectors + boot->number_of_fats * boot->fat_size_sectors + 
    ((boot->root_dir_entries * 32) + (boot->sector_size - 1)) / boot->sector_size) * boot->sector_size;

    int output_img = open(arch_nomb, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_img < 0) {
        perror("Error al abrir el archivo de salida");
        exit(EXIT_FAILURE);
    }

    unsigned short cluster = entry->starting_cluster;
    unsigned int fileSize = entry->file_size;
    unsigned char buffer[clusterSize];

    while (fileSize > 0) {
        unsigned int clusterOffset = dataStart + (cluster - 2) * clusterSize;
        unsigned int readSize = fileSize < clusterSize ? fileSize : clusterSize;

        lseek(fd, clusterOffset, SEEK_SET);
        if (read(fd, buffer, readSize) != readSize) {
            perror("Error al leer el contenido del archivo");
            close(output_img);
            exit(EXIT_FAILURE);
        }

        if (write(output_img, buffer, readSize) != readSize) {
            perror("Error al escribir en el archivo de salida");
            close(output_img);
            exit(EXIT_FAILURE);
        }

        fileSize -= readSize;
        if (fileSize > 0) {
            cluster = sig_clister(fd, cluster, boot);
            if (cluster >= 0xFFF8) {
                break; // Fin de la cadena de clústeres.
            }
        }
    }

    close(output_img);
}

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

    DirEntry entry;

    for (int i = 0; i < boot.root_dir_entries; i++) {
        if (read(fd, &entry, sizeof(entry)) != sizeof(entry)) {
            perror("Error al leer una entrada del directorio");
            close(fd);
            exit(EXIT_FAILURE);
        }
        // Descartamos entradas borradas y etiquetas de volumen
        if (entry.filename[0] != 0xE5 && entry.filename[0] != 0x00 && (entry.attributes & 0x08) == 0) {
            // Verificamos si se ha encontrado el archivo de MINIX comparando tanto el nombre como la extensión
            if (strncmp((const char *)entry.filename, "MINIX3~1", 8) == 0 && strncmp((const char *)entry.ext, "JPEG", 3) == 0) {
                extraer_img(fd, &boot, &entry, "MINIX3.JPEG");
                printf("Imagen extraída con éxito!\n");
                break; // Encuentra la entrada correcta y rompe el ciclo
            }
        }
        
    }
    return 0;
}