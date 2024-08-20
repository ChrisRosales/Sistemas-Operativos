#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int narg,char *argv[]){

    int i,j,n;
    pid_t mypid,child;
    int fd[2];
    char buffer[64];
    char valor[20];
    int n1,n2,n3;

    pipe(fd);

    if(narg == 1){
        printf("Mensaje de error");
        exit(EXIT_FAILURE);
    }
    
    n = atoi(argv[1]);
    //dup();

    if(!(child = fork())){
        //printf("Hola\n");
        close(fd[0]);
        dup2(fd[1],STDOUT_FILENO);
        execl("/usr/bin/factor","factor",argv[1],NULL);
        //write(fd[1],valor,20);
        close(fd[1]);
        sleep(5);
    }

    if(child!=0){
        dup2(fd[0],STDIN_FILENO);
        close(fd[1]);
        read(fd[0],valor,19);
        //printf("%s\n",valor);
        
	
	    sscanf(valor,"%d: %d %d",&n1,&n2,&n3);
        //printf("Los números son:%d,%d,%d\n",n1,n2,n3);
        if(n3 == 0){
            printf("EL numero es primo\n");        
        }
        else{
            printf("EL numero no es primo\n");
        }
    }



}
