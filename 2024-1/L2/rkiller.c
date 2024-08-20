#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int narg, char *argv[]){
	int i, status, n;
	char cadena[50];
	int fd[2];
	pipe(fd);

	pid_t child, pipchild;
	pid_t ppid = getpid();

	n = atoi(argv[1])-1;
	int pids[n];

	for(i=0;i<n;++i){
		if(child=fork()){
			break;
		}else{
			pipchild = getpid();
			write(fd[1],&pipchild,sizeof(int));
		}
	}

	if(ppid == getpid()){
		for(int j=0;j<n;j++) read(fd[0],&pids[j],sizeof(int));
			sprintf(cadena,"pstree -p %d",getpid());
		for(int j=n-1;j>=0;--j){
			kill(pids[j],SIGTERM);
			system(cadena);
			waitpid(pids[j],NULL,0);
		}
		system(cadena);
	}
	else{
		for(;;);
	}
	return 0;
}