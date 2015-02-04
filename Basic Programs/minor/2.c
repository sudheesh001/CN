#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc, char const *argv[])
{
	int fp;
	fp=open("text.txt",O_WRONLY);
	int i;
	scanf("%d",&i);i++;
	char t[2];
	sprintf(t,"%d",i);
	write(fp,t,2);
	printf("%d",i);
	close(fp);
	return 0;
}
