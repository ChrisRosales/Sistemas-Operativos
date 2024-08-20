//Christian Rosales 20201294
//usando el codigo base, procederemos a modificar ciertas partes y a modular ciertas cosas
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 32768 // tamanho del buff
#define PORT 9734 //puerto
//guiandonos de la solucion que hicimos de la pregunta 3 de la guia, nos basaremos y partiremos de ahi, peusto que piden algo similar
char respuestaBuff[BUF_SIZE];
const char *error_response = "<HTML>\n<BODY>File not found\n</BODY>\n</HTML>";
const char *not_found_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<HTML>\n<BODY>File not found\n</BODY>\n</HTML>";

char *lectura(const char *arch) {
    FILE *f = fopen(arch, "r");
    if (f == NULL) return NULL;

    size_t bytes_total = fread(respuestaBuff, 1, BUF_SIZE - 1, f);
    respuestaBuff[bytes_total] = '\0'; // salto de linea
    fclose(f);
    return respuestaBuff;
}

void enviarRespuesta(int client_sockfd, const char *inicio, const char *complemento) {
    char response[BUF_SIZE];
    sprintf(response, "%s\r\n%s", inicio, complemento);
    write(client_sockfd, response, strlen(response));
}
//bastaria hacer un buen antender Hilo para que ejecute de manera corerecta:
void *atenderHilo(void *arg) {
    long cliente_sock = (long)arg;
    char buff[BUF_SIZE];

    read(cliente_sock, buff, BUF_SIZE - 1);
    char *solicitud = strtok(buff, " "); //hacemos uso del strtok y le mandamos el buff como parametro
    solicitud = strtok(NULL, " "); // Tnos devuelve el arch

    // Remove the leading '/'
    if (solicitud[0] == '/') {
        solicitud++;
    }

    char *contenido = lectura(solicitud);

    if (contenido) {
        enviarRespuesta(cliente_sock, "HTTP/1.1 200 OK\r\nContent-Type: text/html", contenido);
    } else {
        enviarRespuesta(cliente_sock, not_found_response, "");
    }
    //funcionooo
    close(cliente_sock);
    pthread_exit(NULL);
}

int main() {
    int server_sockfd, client_sockfd;
    socklen_t server_len, client_len;
    struct sockaddr_in server_address, client_address;

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT);
    server_len = sizeof(server_address);

    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
    listen(server_sockfd, 5);

    while (1) {
        pthread_t thread_id;
        client_len = sizeof(client_address);
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);

        if (client_sockfd > 0) {
            if (pthread_create(&thread_id, NULL, atenderHilo, (void *)(long)client_sockfd) != 0) { //usamos el pthread_create como en la 1
                perror("Thread creation failed"); //modficamos un poco el codigo base
                close(client_sockfd);
            }
        } else {
            perror("Client accept failed");
        }
    } //yo pertenezco a jesus
}
