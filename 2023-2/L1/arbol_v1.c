#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int narg,char *argv[]){
    int i,j,n,mypid;
    pid_t child;
    char cad[50];

    if(narg == 1){
        printf("Mensaje de error");
        exit(EXIT_FAILURE);
    }


    n=atoi(argv[1]);
    for(i=0;i<n;i++){
        for(j=0;j<=i;j++){
            if(!(child = fork())){
                break;
            }
            
        }
        if(child!=0)break;
    
    }
    pause();
    exit(EXIT_SUCCESS);
    return 0;

}
