#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
int main(int narg, char *argv[]){
    int i, n, mypid, p;
    int fd[2][2];
    n = atoi(argv[1]);
    pipe(fd[0]);
    pipe(fd[1]);
    for(i=0; i<2;i++){
        if( !fork() ){
            /*Hijos*/
            int *ptr;
            ptr = (int *)calloc(n, sizeof(int));
            srand(getpid());
            for(i=0; i<n; i++){
                ptr[i] = rand() % 50;
            }

            mypid = getpid();
            if( mypid % 2 == 0){
                for(i=0; i<n; i++){
                    if(ptr[i]%2 == 0) printf("My pid=%d my number=%d\n", mypid, ptr[i]);
                    else{
                        close(fd[0][1]);//cierro la puerta de escritura del hijo 0
                        write(fd[1][1],&ptr[i],sizeof(int) );
                        //escribo en la puerta de escritura del hijo 1
                    }
                }
                close(fd[1][1]);
                while( read(fd[0][0],&p, sizeof(int) ) > 0){
                    //leo de la puerta de lectura del hijo 0
                    printf("My pid=%d my number=%d\n", mypid, p);
                }
                close(fd[0][0]);
            }else{
                for(i=0; i<n; i++){
                    if(ptr[i]%2 != 0) printf("My pid=%d my number=%d\n", mypid, ptr[i]);
                    else{
                        close(fd[1][1]);
                        //cierro la puerta de escritura del hijo 1
                        write(fd[0][1],&ptr[i],sizeof(int) );
                        //escribo en la puerta de escritura del hijo 0
                    }
                }
                close(fd[0][1]);
                // fd [HIJO][PUERTA]
                while( read(fd[1][0],&p, sizeof(int) ) > 0){
                    printf("My pid=%d my number=%d\n", mypid, p);
                }
                close(fd[1][0]);
            }
            for(i=0; i<n; i++){
        		printf("My pid=%d my number=%d\n", mypid, ptr[i]);
    		}
        }
    }
    return 0;
}

