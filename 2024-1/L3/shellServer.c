// Christian Rosales 20201294
// usando el codigo base proporcionado en el paideia
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h> /* Type definitions used by many programs */
#include <stdio.h>     /* Standard I/O functions */
#include <stdlib.h>    /* Prototypes of commonly used library functions,
                           plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>    /* Prototypes for many system calls */
#include <errno.h>     /* Declares errno and defines error constants */
#include <string.h>    /* Commonly used string-handling functions */
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#define SV_SOCK_PATH "/tmp/us_xfr"
#define BUF_SIZE 100
#define BACKLOG 5

// Manejador de señal para evitar procesos zombies
void grimReaper(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        continue;
    }
}
void execute_command(int cfd, char *cmd) {
    // Redirigir la salida estándar y la salida de error al socket del cliente
    dup2(cfd, STDOUT_FILENO);
    dup2(cfd, STDERR_FILENO);
    close(cfd); // No es necesario después de dup2

    // Ejecutar el comando del shell
    execl("/bin/sh", "sh", "-c", cmd, (char *)NULL);
    // Si execl retorna, hubo un error
    perror("execl");
}

int main() {
    struct sockaddr_un addr;
    int sfd, cfd;
    ssize_t numRead;
    char buf[BUF_SIZE];

    // Establecer manejador de señal para SIGCHLD
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

    // Asegurarse de que la dirección no existe antes de bind
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

    for (;;) {
        cfd = accept(sfd, NULL, NULL);
        if (cfd == -1) {
            perror("accept");
            continue;
        }

        // Leer el comando del cliente
        numRead = read(cfd, buf, BUF_SIZE - 1);
        if (numRead <= 0) {
            close(cfd); // Cerrar conexión si falla la lectura
            continue;
        }
        buf[numRead] = '\0'; // Asegurar que el buffer sea una cadena válida

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            close(cfd);
        } else if (pid == 0) { // Proceso hijo
            execute_command(cfd, buf); // Ejecutar comando
            _exit(EXIT_SUCCESS);
        } else { // Proceso padre
            close(cfd); // El padre cierra el socket del cliente
        }
    }
}
