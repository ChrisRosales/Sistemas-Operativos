#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    int i, status, n;
    pid_t child;
    int fd[2];
    pid_t arrPid[10], mypid, val;
    pid_t pidprogram = getpid();

    // Convertir el argumento de la línea de comandos en entero
    n = atoi(argv[1]);
    pipe(fd);

    // Crear n procesos
    for (i = 0; i < n; ++i) {
        child = fork();
        if (child != 0) break;
        else {
            // Código del proceso hijo
            mypid = getpid();
            close(fd[0]); // Cerrar el descriptor de lectura
            write(fd[1], &mypid, sizeof(pid_t)); // Escribir el PID en el pipe
        }
    }

    // Esperar a los hijos
    wait(&status);

    // Código del proceso padre
    for (i = 0; i < n; ++i) {
        close(fd[1]); // Cerrar el descriptor de escritura
        read(fd[0], &arrPid[i], sizeof(pid_t)); // Leer el PID del pipe
    }

    close(fd[0]); // Cerrar el descriptor de lectura

    // Mostrar el árbol de procesos
    char cad[50];
    for (i = n - 1; i >= 0; --i) {
        val = arrPid[i];
        sprintf(cad, "pstree -p %d", pidprogram); // Crear el comando para mostrar el árbol
        system(cad); // Ejecutar el comando
    }

    return 0;
}

