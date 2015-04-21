#include <stdio.h>
#include <unistd.h>
int main(){
	printf("Hello World----test program\n");
	execve("test2",NULL,NULL);
}