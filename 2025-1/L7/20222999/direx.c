#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include "exFATstruct.h"
#define ENTRY_SIZE 32
#define FILE_ENTRY     0x85
#define STREAM_ENTRY   0xC0
#define FILENAME_ENTRY 0xC1
typedef struct {
    uint8_t entryType;
    uint8_t secondaryCount;
    uint16_t setChecksum;
    uint16_t reserved1;
    uint16_t fileAttributes;
    uint32_t reserved2[2];
    uint8_t createdTimestamp[10];
    uint8_t modifiedTimestamp[10];
    uint8_t accessedTimestamp[10];
    uint8_t createTimeZone;
    uint8_t modifyTimeZone;
    uint8_t accessTimeZone;
    uint8_t reserved3[7];
} __attribute__((packed)) FileDirectoryEntry;
typedef struct {
    uint8_t entryType;
    uint8_t generalSecondaryFlags;
    uint8_t reserved1;
    uint8_t nameLength;
    uint16_t nameHash;
    uint16_t reserved2;
    uint64_t validDataLength;
    uint32_t reserved3;
    uint32_t firstCluster;
    uint64_t dataLength;
} __attribute__((packed)) StreamExtensionEntry;
typedef struct {
    uint8_t entryType;
    uint8_t generalSecondaryFlags;
    uint16_t fileName[15]; // UTF-16, 15 caracteres por entrada
} __attribute__((packed)) FileNameEntry;
int main(int argc, char *argv[]) {
    int fd;
    exFatBootSector boot;
    if (argc != 2) {
        printf("Uso: %s <Imagen del sistema de archivos>\n", argv[0]);
        return 1;
    }
    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        perror("No se pudo abrir la imagen");
        return 1;
    }
    if (read(fd, &boot, sizeof(boot)) < 0) {
        perror("No se pudo leer el sector de arranque\n");
        close(fd);
        return 1;
    }
    int bytes_per_sector = (int)pow(2, boot.BytePerSector);
    int sectors_per_cluster = (int)pow(2, boot.SectorPerCluster);
    int cluster_size = bytes_per_sector * sectors_per_cluster;
    uint32_t root_cluster = boot.RootDirFirstCluster;
    off_t root_offset = (boot.ClusterHeapOffset + (root_cluster - 2) * sectors_per_cluster) * bytes_per_sector;
    if (lseek(fd, root_offset, SEEK_SET) < 0) {
        perror("Error al mover el puntero al directorio raíz");
        close(fd);
        return 1;
    }
    uint8_t *buffer = malloc(cluster_size);
    if (!buffer) {
        perror("No se pudo asignar memoria");
        close(fd);
        return 1;
    }
    if (read(fd, buffer, cluster_size) < 0) {
        perror("Error al leer el directorio raíz");
        free(buffer);
        close(fd);
        return 1;
    }
    printf("Entradas válidas en el directorio raíz:\n\n");
    int i = 0;
    while (i < cluster_size) {
        uint8_t entryType = buffer[i];
        if (entryType == 0x00) break; // Fin del directorio
        if (entryType == FILE_ENTRY) {
            FileDirectoryEntry *fileEntry = (FileDirectoryEntry *)&buffer[i];
            StreamExtensionEntry *streamEntry = (StreamExtensionEntry *)&buffer[i + ENTRY_SIZE];
            // Verificar que la entrada secundaria esperada sea una entrada de stream
            if (streamEntry->entryType != STREAM_ENTRY) {
                i += ENTRY_SIZE;
                continue;
            }
            // Busco nombre 
            FileNameEntry *nameEntry = (FileNameEntry *)&buffer[i + 2 * ENTRY_SIZE];
            char filename[31] = {0};
            if (nameEntry->entryType == FILENAME_ENTRY) {
                for (int j = 0; j < 15; j++) {
                    // Solo tomamos el byte bajo del UTF-16
                    filename[j] = (char)(nameEntry->fileName[j] & 0xFF);
                    if (filename[j] == '\0') break;
                }
            } else {
                strcpy(filename, "(sin nombre)");
            }
            printf("Nombre: %s\n", filename);
            printf("Tamaño: %lu bytes\n", (unsigned long)streamEntry->dataLength);
            printf("Primer cluster: %u\n\n", streamEntry->firstCluster);
            i += (1 + fileEntry->secondaryCount) * ENTRY_SIZE;
        } else {
            i += ENTRY_SIZE;
        }
    }
    free(buffer);
    close(fd);
    return 0;
}
