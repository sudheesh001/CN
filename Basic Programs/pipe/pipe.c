#include <stdio.h>
#include <fcntl.h>

int main(){
	int fd1[2];
	int t;
	t=pipe(fd1);
	if(t==-1){
		perror("Error creating pipe1");
	}
	int pid=0,i=0;
	pid=fork();

	if(pid>0){			//Parent process
	close(fd1[0]);
	char buf[9];
	for(i=0;i<9;i++)buf[i]='\0';
	printf("Parent process: Enter filename\n");
	read(0,buf,9);
	printf("Test: %s\n",buf );
	write(fd1[1],buf,9);
	close(0);
	}

	else if(pid==0){		//Child process
		close(fd1[1]);
		char buf3[10];
		for(i=0;i<10;i++)buf3[i]='\0';
		read(fd1[0],buf3,10);
		printf("From child process: %s\n",buf3 );
		int fd=open(buf3,O_RDONLY);
		if(fd==-1)perror("cannot open file");
		int n=0;
		char buf4[10];
		while((n=read(fd,buf4,10))>0)
			printf("%s",buf4 );
	}
	
	return 0;
}