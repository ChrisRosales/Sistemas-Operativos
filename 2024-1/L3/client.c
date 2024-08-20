//Christian Rosales 20201294
//codigo base proporcioando por el paideia
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>  /* Type definitions used by many programs */
#include <stdio.h>      /* Standard I/O functions */
#include <stdlib.h>     /* Prototypes of commonly used library functions,
                           plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>     /* Prototypes for many system calls */
#include <errno.h>      /* Declares errno and defines error constants */
#include <string.h>     /* Commonly used string-handling functions */


#define SV_SOCK_PATH "/tmp/us_xfr"

#define BUF_SIZE 100

//ahora nos guiaremos de la pregunta 1 de la solucion de la guia del cliente que se hizo en UNIX:
int main() {
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

    // Leer el comando de la entrada estándar (stdin)
    printf("Enter command: ");
    if (fgets(buf, BUF_SIZE, stdin) == NULL) {
        printf("Error reading command\n");
        exit(EXIT_FAILURE);
    }

    // Enviar el comando al servidor
    numRead = strlen(buf);
    if (write(sfd, buf, numRead) != numRead) {
        perror("partial/failed write");
        exit(EXIT_FAILURE);
    }

    // Leer y mostrar la respuesta del servidor
    while ((numRead = read(sfd, buf, BUF_SIZE)) > 0) {
        if (write(STDOUT_FILENO, buf, numRead) != numRead) {
            perror("partial/failed write to stdout");
            break;
        }
    }

    if (numRead == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    close(sfd);  // Cerrar el socket
    return 0;
}
