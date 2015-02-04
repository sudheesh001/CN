#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
	
	int fd;
	while(1){
	fd = open("/dev/vchrdev/",O_RDONLY);
		if(fd>0){
			char c;
			read(fd,&c,1);
			if(c=='0'){
				printf("pendrive inserted\n");
				// system("gnome-terminal -x sh -c 'echo pendrive inserted; cat'");
			}
			if(c=='1'){
				printf("pendrive removed\n");
				// system("gnome-terminal -x sh -c 'echo pendrive inserted; cat'");
			}
		}
	close(fd);
	}
	return 0;
}