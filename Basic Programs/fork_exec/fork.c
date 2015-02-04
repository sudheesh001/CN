#include <stdio.h>
#include <unistd.h>

int main(){
int pid=0;
pid=fork();
if(pid>0){
	printf("Parent process %d\n",getpid());
	wait();
	printf("blah\n");
}
else if(pid==0){
	printf("Child process %d\n",getpid());
	execve("test",NULL,NULL);
	printf("No output\n");
} 
else{
	perror("Forking error\n");
}
return 0;
}
