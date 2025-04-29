/**********************************************************************\
*                Copyright (C) Michael Kerrisk, 2010.                  *
*                                                                      *
* This program is free software. You may use, modify, and redistribute *
* it under the terms of the GNU Affero General Public License as       *
* published by the Free Software Foundation, either version 3 or (at   *
* your option) any later version. This program is distributed without  *
* any warranty. See the file COPYING for details.                      *
\**********************************************************************/

/* us_xfr_cl.c

   An example UNIX domain stream socket client. This client transmits contents
   of stdin to a server socket.

   See also us_xfr_sv.c.
*/

#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
// Kevin Luis Levano Cuzcano 20201865
#define SV_SOCK_PATH "/tmp/us_xfr"
#define BUF_SIZE 100

void escribirArchivo(char *filename, char *data) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%s\n", data);
    fclose(file);
}

int main(int argc, char *argv[]) {
    struct sockaddr_un addr;
    int sfd;
    ssize_t numRead;
    char buf[BUF_SIZE];

    // Crear el socket del cliente
    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Construir la dirección del servidor y conectar
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Conectado al servidor.\n");

    // Leer los números y enviarlos al servidor
    if (argc > 1) {
        snprintf(buf, BUF_SIZE, "%s ", argv[1]); // El primer número indica la cantidad
        for (int i = 2; i < argc; i++) {
            strncat(buf, argv[i], BUF_SIZE - strlen(buf) - 1);
            strncat(buf, " ", BUF_SIZE - strlen(buf) - 1);
        }
    } else {
        // Leer desde la entrada estándar
        if (fgets(buf, BUF_SIZE, stdin) != NULL) {
            int count = 0;
            char temp[BUF_SIZE];
            strcpy(temp, buf);
            char *token = strtok(temp, " ");
            while (token != NULL) {
                count++;
                token = strtok(NULL, " ");
            }
            // Añadir la cantidad de números al inicio del buffer
            char header[BUF_SIZE];
            snprintf(header, BUF_SIZE, "%d ", count);
            memmove(buf + strlen(header), buf, strlen(buf) + 1);
            memcpy(buf, header, strlen(header));
        }
    }

    if (write(sfd, buf, strlen(buf)) != strlen(buf)) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    printf("Números enviados al servidor: %s\n", buf);

    // Leer la respuesta del servidor

    while ((numRead = read(sfd, buf, BUF_SIZE)) > 0) {
        buf[numRead] = '\0'; // Convertir a cadena
        printf("%s", buf);
        escribirArchivo("data.txt", buf);
    }

    if (numRead == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    close(sfd);
    return 0;
}


