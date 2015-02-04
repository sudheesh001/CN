#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/select.h>
#include<poll.h>
#include<netinet/in.h>
#include <arpa/inet.h>



  struct sockaddr_in seradd[5],cladd;
   struct pollfd f[25];
   int sfd[5],portno[5]={8000,9886,8010,8015,8020},maxcl[5]={25,15,100,100,1},currcl[5]={0,0,0,0,0},nsfd[5],clno=0,i,j,max,n;
  // char prog[5][]={"./s1"," "," ","./s4","./s5"};
   fd_set rfds;
   struct timeval t;
   char buf[256];

void *s2(void * arg)
{
  int nsfd=*((int *) arg);
  int len;
  while(1)
  {
    int n=recv(nsfd,buf,256,0);
     if(n==0){maxcl[1]--;break;}
     len=strlen(buf);
     bzero(buf,sizeof(buf));
     sprintf(buf,"%d",len);
     send(nsfd,buf,256,0);
  }
}


void s3()
{
    return;
}


int main()
{
   
   pthread_t t1;
   socklen_t cllen;
  for(i=0;i<5;i++){
  if(i!=2 && i!=4)
  sfd[i]=socket(AF_INET,SOCK_STREAM,0);
  else
    sfd[i]=socket(AF_INET,SOCK_DGRAM,0);

   bzero(&seradd[i],sizeof(struct sockaddr_in));   
   bzero(&cladd,sizeof(struct sockaddr_in));
   seradd[i].sin_addr.s_addr=htonl(INADDR_ANY);
   seradd[i].sin_port=htons(portno[i]);
   seradd[i].sin_family=AF_INET;
   bind(sfd[i],(struct sockaddr *)&seradd[i],sizeof(struct sockaddr_in));
   
   if(i!=2 && i!=4)
    listen(sfd[i],maxcl[i]);
  }
  
  FD_ZERO(&rfds);
  max=sfd[0];
  for(i=0;i<5;i++)
  if(i!=2 && i!=4){
  FD_SET(sfd[i],&rfds);
  if(sfd[i]>max)
  max=sfd[i];
   }

   
 start:
   select(max+1,&rfds,NULL,NULL,NULL);

  for(i=0;i<5;i++)
  {
   if(i!=2 && i!=4)
   if(FD_ISSET(sfd[i],&rfds))
    {   
       if(currcl[i]<maxcl[i]){
        cllen=sizeof(struct sockaddr_in);
        nsfd[i]=accept(sfd[i],(struct sockaddr *)&cladd,&cllen);
        currcl[i]++;
        if(i==1)
         pthread_create(&t1,NULL,&s2,&nsfd[i]);
      /*  else
         {
           int h=fork;
            if(h==0)
              {
                 for(int j=0;j<5;j++)
                  close(sfd[i]);
                  dup(nsfd[i],0);
                  execv(prog[i],"A",NULL);
                  
               }
             else
                goto start;
          }
        */
       }
 
    }
    
    else
     {
          if(i==2)
           s3();
     }

}


 pthread_join(t1,NULL);


}
