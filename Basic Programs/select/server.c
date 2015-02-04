#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

int main(){
fd_set rfds;
struct timeval tv;
int retval,wr;
mkfifo("pipew1",0666);
mkfifo("piper1",0666);
mkfifo("./piper2",0666);
mkfifo("./pipew2",0666);
mkfifo("./piper3",0666);
mkfifo("./pipew3",0666);

int fd[3];
fd[0]=open("pipew1",O_RDWR|O_NONBLOCK);
if(fd[0]==-1){
	perror("Opening error:");
	return 0;
}
fd[1]=open("pipew2",O_RDWR|O_NONBLOCK);
if(fd[1]==-1){
	perror("Opening error:");
	return 0;
}
fd[2]=open("pipew3",O_RDWR|O_NONBLOCK);
if(fd[2]==-1){
   perror("Opening error:");
   return 0;
}
int i,flag;
tv.tv_sec=5;
tv.tv_usec=0;
char buffer[12]={0};
retval=0;
int maxfd=(fd[0]>fd[1])?fd[0]:fd[1];
maxfd=(maxfd>fd[2])?maxfd:fd[2];
while(1){
sleep(1);
do {
   FD_ZERO(&rfds);
   FD_SET(fd[0], &rfds);
   FD_SET(fd[1], &rfds);
   FD_SET(fd[2], &rfds);
   retval = select(maxfd+1, &rfds, NULL, NULL, &tv);
} while (retval == -1);
if (retval > 0) {
   if (FD_ISSET(fd[0], &rfds)) {
   		read(fd[0],buffer,12);
         buffer[strlen(buffer)]='\0';
         printf("Client 1 messaged: %s\n",buffer);
         i=open("piper2",O_RDWR|O_NONBLOCK);
         if(i){}
         else{perror("Error opening");}
         write(i,buffer,strlen(buffer));
         close(i);
         i=open("piper3",O_RDWR|O_NONBLOCK);
         if(i){}
         else{perror("Error opening");}
         write(i,buffer,strlen(buffer));
         close(i);
   }
   if (FD_ISSET(fd[1], &rfds)) {
   		read(fd[1],buffer,12);
         buffer[strlen(buffer)]='\0';
         printf("Client 2 messaged: %s\n",buffer);
         i=open("piper1",O_RDWR|O_NONBLOCK);
         if(i){}
         else{perror("Error opening");}
         write(i,buffer,strlen(buffer));
         close(i);
         i=open("piper3",O_RDWR|O_NONBLOCK);
         if(i){}
         else{perror("Error opening");}
         write(i,buffer,strlen(buffer));
         close(i);
   }
   if (FD_ISSET(fd[2], &rfds)) {
         read(fd[2],buffer,12);
         buffer[strlen(buffer)]='\0';
         printf("Client 3 messaged: %s\n",buffer);
         i=open("piper2",O_RDWR|O_NONBLOCK);
         if(i){}
         else{perror("Error opening");}
         write(i,buffer,strlen(buffer));
         close(i);
         i=open("piper1",O_RDWR|O_NONBLOCK);
         if(i){}
         else{perror("Error opening");}
         write(i,buffer,strlen(buffer));
         close(i);
   }
}
else if(retval==-1){
   perror("select error");
   break;
}
}
return 0;   
}