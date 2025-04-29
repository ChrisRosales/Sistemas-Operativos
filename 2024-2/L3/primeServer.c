/**********************************************************************\
*                Copyright (C) Michael Kerrisk, 2010.                  *
*                                                                      *
* This program is free software. You may use, modify, and redistribute *
* it under the terms of the GNU Affero General Public License as       *
* published by the Free Software Foundation, either version 3 or (at   *
* your option) any later version. This program is distributed without  *
* any warranty. See the file COPYING for details.                      *
\**********************************************************************/

/* us_xfr_sv.c

   An example UNIX stream socket server. Accepts incoming connections
   and copies data sent from clients to stdout.

   See also us_xfr_cl.c.
*/

#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define SV_SOCK_PATH "/tmp/us_xfr"
#define BUF_SIZE 100
#define BACKLOG 5
// Kevin Luis Levano Cuzcano 20201865

// Controlar procesos zombies
void grimReaper(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        continue;
    }
}

void ejecutarShell(int cfd, int number) {
    char cmd[BUF_SIZE];
    snprintf(cmd, BUF_SIZE, "factor %d", number); // Comando factor para obtener factores
    FILE *fp = popen(cmd, "r");
    if (fp == NULL) {
        perror("popen");
        return;
    }

    char result[BUF_SIZE];
    fgets(result, BUF_SIZE, fp); // Leer el resultado de factor
    pclose(fp);

    printf("Resultado de factor para %d: %s\n", number, result); // Debug: Mostrar resultado de factor

    // Verificar si el número es primo (tiene exactamente dos factores: 1 y él mismo)
    int count = 0;
    char *token = strtok(result, " :\n");
    while (token != NULL) {
        count++;
        token = strtok(NULL, " :\n");
    }

    if (count == 2) { // Número primo
        snprintf(cmd, BUF_SIZE, "%d\n", number);
        printf("Enviando número primo al cliente: %d\n", number); // Debug: Mostrar número primo enviado
        if (write(cfd, cmd, strlen(cmd)) != strlen(cmd)) {
            perror("Error enviando número primo");
        }
    }
}

void ejecutarProceso(int cfd) {
    char buf[BUF_SIZE];
    ssize_t numRead;

    // Leer los números del cliente
    numRead = read(cfd, buf, BUF_SIZE - 1);
    if (numRead <= 0) {
        perror("Error leyendo del cliente");
        close(cfd);
        return;
    }
    buf[numRead] = '\0';
    printf("Números recibidos: %s\n", buf); // Debug: Mostrar números recibidos

    // Leer la cantidad de números
    char *token = strtok(buf, " ");
    int count = atoi(token);
    printf("Cantidad de números a procesar: %d\n", count); // Debug: Mostrar cantidad de números

    // Almacenar todos los números en un arreglo
    int numbers[count];
    int index = 0;
    token = strtok(NULL, " ");
    while (token != NULL) {
        numbers[index] = atoi(token);
        printf("Número almacenado: %d\n", numbers[index]); // Debug: Mostrar números almacenados
        index++;
        token = strtok(NULL, " ");
    }

    // Procesar cada número del arreglo
    for (int i = 0; i < count; i++) {
        printf("Procesando número: %d\n", numbers[i]); // Debug: Mostrar número a procesar
        ejecutarShell(cfd, numbers[i]); // Procesar el número
    }

    // Enviar un mensaje de finalización al cliente para indicar que todos los números se han procesado
    printf("Todos los números procesados. Cerrando conexión con el cliente.\n");
    close(cfd);
}

int main() {
    struct sockaddr_un addr;
    int sfd, cfd;
    struct sigaction sa;

    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT) {
        perror("remove-" SV_SOCK_PATH);
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sfd, BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Servidor en espera de conexiones...\n");
    // Segun lo indicado por el JP el que llega a procesar todo es el SERVIDOR, el cliente solo envia
    for (;;) {
        cfd = accept(sfd, NULL, NULL);
        if (cfd == -1) {
            perror("accept");
            continue;
        }

        printf("Cliente conectado.\n");

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            close(cfd);
        } else if (pid == 0) { // Proceso hijo
            close(sfd); // El proceso hijo no necesita el socket de escucha
            ejecutarProceso(cfd); // Procesar la solicitud del cliente
            _exit(EXIT_SUCCESS);
        } else {
            close(cfd);
        }
    }
}

