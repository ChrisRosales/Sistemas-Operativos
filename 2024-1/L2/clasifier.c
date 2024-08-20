#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int na, char *argv[]){
	int a,b,c;
	int n,i,pid, pids[3], status;
	int par_fd[2],impar_fd[2],otro_fd[2],pid_fd[2], fd;

	if(pipe(par_fd)==-1 || pipe(impar_fd)==-1 || pipe(otro_fd)== -1 || pipe(pid_fd)==-1){
		perror("pipe");
		exit(2);
	}


	if(na != 2) {
		fprintf(stderr, "Uso: %s <n>\n",argv[0]);
		exit(1);
	}
	n = atoi(argv[1]);
	for(i=0;i<3;i++) {
		if(!(pid = fork())) { //Hijos
			int j,r, mypid;
			mypid = getpid();
			srand(mypid);			
			for(j=0;j<n;j++) {
			    r = rand() % 528;
			    printf("PID: %d \t NUM: %d\n",mypid,r);
			    if(r%2==0) fd = par_fd[1];
			    else fd = impar_fd[1];
			    write(fd,&r,sizeof(int));
			    int k = r%10;
			    if(k == 1 || k == 2){
			    write(otro_fd[1],&r,sizeof(int));
				}
			}
			close(fd);
			close(otro_fd[1]);
			sleep(1);
			read(pid_fd[0],&pid,sizeof(int));
			pids[0]=pid;
			pid = pid + 1;
			pids[1]=pid;
			pid = pid + 1;
			pids[2]=pid;

			close(pid_fd[0]);
			if(mypid==pids[0]) fd = par_fd[0];	
			else if(mypid==pids[1]) fd = impar_fd[0];
			else if(mypid==pids[2]) fd = otro_fd[0];
			int bytes;
			fcntl(fd, F_SETFL, O_NONBLOCK);
			int flag = 1;
			while(1){
				bytes = read(fd,&r,sizeof(int));
				if(bytes!=-1){
					flag = 0;
					printf("Después: PID %d \t NUM: %d\n",mypid,r);
				}
				else{
					if(flag == 1) printf("Después: PID %d \t Conjunto Vacio\n",mypid);
					break;
				} 
			}
			close(fd);
			exit(0);    
		}
		if(i==0) a = pid;
		write(pid_fd[1],&a,sizeof(int));
	}
	close(pid_fd[1]);
	wait(NULL);
	wait(NULL);
	wait(NULL);
	exit(0);	
}	


	
