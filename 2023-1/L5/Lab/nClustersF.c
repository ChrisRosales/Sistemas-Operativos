#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "structfat.h"

#define FAT_OFFSET 0x200
#define ENTRY_SIZE 1.5

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        printf("Usage: %s <disk image> <m>\n", argv[0]);
        exit(1);
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("No se puede abrir imagen disco");
        exit(1);
    }
    int m = atoi(argv[2]);
    int n = 2; //a partir del 2 recien comienza
    if (m < 1) {
        printf("m debe ser mayor a 0\n");
        exit(1);
    }
    int found_clusters = 0;
    unsigned short entry;
    for (int i = n, c = 1; found_clusters < m; i++) {
        lseek(fd, FAT_OFFSET + i * ENTRY_SIZE, SEEK_SET);
        if (read(fd, &entry, sizeof(entry)) < sizeof(entry)) {
            printf("Se acabo el FAT y no pudo encontrar los %d clusters libres.\n", m);
            exit(1);
        }
        if (entry == 0x000) {
            printf("[%d°] cluster libre %d\n", c, i);
            found_clusters++;
            c++;
        }
    }
    close(fd);
    return 0;
}
