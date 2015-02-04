#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <string.h>
#include <arpa/inet.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<pthread.h>
#include<iostream>
#include<vector>
#include<sys/select.h>
#define SA struct sockaddr
#define LISTENQ 100
#define noofprocesses 3
#define noofthreads 3
using namespace std;
#define portno 9886

char buf[256];
int sfd,n;

void *receiver(void *ptr)
{
  while(1)
  {
     bzero(buf,sizeof(buf));
     while( recv(sfd,buf,256,0)<0);
      write(1,buf,256);cout<<endl;
   }

}


void *sender(void *ptr)
{
   while(1)
   {
     bzero(buf,sizeof(buf));
      while((n=read(0,buf,256))<0);
      send(sfd,buf,256,0);
   }
 
}

int main()
{

   struct sockaddr_in seradd;
   bzero(&seradd,sizeof(struct sockaddr_in));
   pthread_t t1,t2;
   
   sfd=socket(AF_INET,SOCK_STREAM,0);
   
   seradd.sin_family=AF_INET;
   seradd.sin_port=htons(portno);
   seradd.sin_addr.s_addr=INADDR_ANY;

   if(connect(sfd,(struct sockaddr *)&seradd,sizeof(struct sockaddr_in))<0)
  printf("connect error");
   
  printf("connected\n");

  
   pthread_create(&t1,NULL,&receiver,NULL);
   pthread_create(&t2,NULL,&sender,NULL);
  
   pthread_join(t1,NULL);
 
   

}
