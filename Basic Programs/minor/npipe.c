#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
	int i;
	int pipefd[2];
	if(pipe(pipefd)==-1){
		perror("Error opening file");
	}

	int pid=fork();
	if(pid>0){
		wait(0);
		for(i=1;i<3;i++){
		int p=fork();
		if(p==0){
		dup2(pipefd[0],0);
		pipe(pipefd);
		dup2(pipefd[1],1);
		execl(argv[i+1],"kk",(char *)0);
		}
		}

	}
	else{
		dup2(pipefd[1],1);	
		execl(argv[1],"kk",(char *)0);
	}
	
	return 0;
}
