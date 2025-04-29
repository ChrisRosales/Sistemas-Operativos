/*  This program, server5.c, begins in similar vein to our last server,
    with the notable addition of create threads.
    The variables and the procedure of creating and naming a socket are the same.  */
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// Kevin Luis Lévano Cuzcano 20201865
char ret_buf[32768];
char * error_return = "<HTML>\n<BODY>File not found\n</BODY>\n</HTML>";

char * read_file(char * filename) {
    FILE *f;
    int i;

    f = fopen(filename, "r");
    if (f == NULL) {
        return error_return;
    }

    i = fread(ret_buf, 1, 32768, f);
    if (i == 0) {
        fclose(f);
        return error_return;
    }

    ret_buf[i] = '\0';
    fclose(f);
    return ret_buf;
}

void enviarRespuesta(int c_sockfd, char *content_type, char *content) {
    char response[32768];
    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n%s", content_type, strlen(content), content);
    write(c_sockfd, response, strlen(response));
}

void *thread_func(void * tsocket) {
    long  c_sockfd = (long)tsocket;
    char buffer[4000], *method, *url, *version;
    int n;

    n = read(c_sockfd, buffer, sizeof(buffer));
    buffer[n] = '\0';

    method = strtok(buffer, " ");
    url = strtok(NULL, " ");
    version = strtok(NULL, "\r");

    if (strcmp(method, "GET") == 0) {
        char *filename = url + 1;
        if (strcmp(filename, "") == 0) {
            filename = "index.html";
        }
        
        char *content_type;
        if (strstr(filename, ".html") != NULL) {
            content_type = "text/html";
        } else {
            content_type = "application/octet-stream";
        }
        
        char *content = read_file(filename);
        enviarRespuesta(c_sockfd, content_type, content);
    } else {
        enviarRespuesta(c_sockfd, "text/html", error_return);
    }

    close(c_sockfd);
    pthread_exit(NULL);
}

int main() {
    long server_sockfd, client_sockfd;
    int server_len, client_len;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(9734);
    server_len = sizeof(server_address);
    bind(server_sockfd, (struct sockaddr *)&server_address, (socklen_t)server_len);

    listen(server_sockfd, 5);

    while(1) {
        pthread_t thread_id;

        client_len = sizeof(client_address);
        client_sockfd = accept(server_sockfd,
            (struct sockaddr *)&client_address, (socklen_t *)&client_len);

        int ret = pthread_create(&thread_id, NULL, thread_func, (void *)client_sockfd);
        if (ret != 0) {
            printf("Error from pthread: %d\n", ret);
            exit(1);
        }
    }
}

// http://127.0.0.1:9734/index.html

