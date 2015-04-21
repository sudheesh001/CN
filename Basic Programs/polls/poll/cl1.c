#include<stdio.h>
#include<unistd.h>
#include <stdlib.h>
#include <string.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#define cl1 "./cl1"
#define cl11 "./cl11"


int main()
{
    
    int w,r,k,n;
   mkfifo(cl1,0644);
  mkfifo(cl11,0644);
   char buf[10];
int h=fork();

while(1){


   
   if(h==0){
        
    w=open(cl1,O_WRONLY);
         n=read(0,&buf,10);
       write(w,&buf,n);
       close(w);}
   else if(h>0){

       r=open(cl11,O_RDONLY);
       n=read(r,&buf ,10);
     write(1,&buf,n);   
     close(r);}
  

    
    
    }
}

