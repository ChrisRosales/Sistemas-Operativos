#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fatentry.h"

#define CLUSTER_SIZE 4096

void print_cluster_chain(int fd, Fat32BootSector *boot, int start_cluster);

int main(int argc, char **argv) {
    Fat32BootSector boot;
    FatEntry entry;
    int fd, i, offset, size_block, num_fat;

    if (argc != 2) {
        printf("Usage: %s <disk image name>\n", argv[0]);
        exit(1);
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        perror("Could not open disk image");
        exit(1);
    }

    // Asumir que la imagen es FAT32
    if (read(fd, &boot, sizeof(boot)) < 0) {
        perror("Could not read boot sector");
        exit(1);
    }

    lseek(fd, boot.reserved_sectors * boot.sector_size, SEEK_SET);
    for (i = 0; ; i++) {
        if (read(fd, &entry, sizeof(entry)) < 0) {
            perror("Could not read disk image");
            exit(1);
        }
        if (!entry.filename[0]) break;
        if ((entry.attributes != 0x0F) && (entry.attributes & 0x20)) {
            printf("File [%.8s.%.3s] First Cluster [%d] Size [%d]\n",
                   entry.filename, entry.ext, entry.starting_cluster, entry.file_size);
            print_cluster_chain(fd, &boot, entry.starting_cluster);
        }
    }

    close(fd);
    return 0;
}

void print_cluster_chain(int fd, Fat32BootSector *boot, int start_cluster) {
    unsigned int cluster = start_cluster;
    unsigned int next_cluster;
    unsigned int sector_offset;
    unsigned int byte_offset;
    printf("Chain of clusters: ");
    while (1) {
        printf("%u ", cluster);
        sector_offset = boot->reserved_sectors + (cluster * sizeof(unsigned int));
        byte_offset = sector_offset * boot->sector_size;
        lseek(fd, byte_offset, SEEK_SET);
        read(fd, &next_cluster, sizeof(unsigned int));
        next_cluster &= 0x0FFFFFFF;
        if (next_cluster >= 0x0FFFFFF8) break;
        cluster = next_cluster;
    }
    printf("\n");
}
