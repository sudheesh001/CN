#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "cache.c"
#define STDIN 0
#define STDOUT 1
#define MAX_STR_LEN 1500
#define MAX_ENTRY 5
int count = 0;
void error(char *msg)
{
    perror(msg);
    exit(0);
}


/* 
 * returns the hostname from Request
*/
int getHostFromRequest(char *data,char **shost)
{
    printf("Inside getfromrequest\n");
    char *p = NULL;
    char *pend=NULL;
    p = strstr(data,"HOST:");
    if (p==NULL) return -1;
    p+=6;
    
    pend = strstr(p,"\r\n");
    if (pend==NULL) return -1;
    int l = pend-p;

    *shost = (char *)malloc(l+4);
    bzero(*shost,l+4);
    memcpy(*shost,p,l);
    return l;
}

void createifModifiedRequest(char * ifrequest,char *irequest,char *time) 
{
    printf("Creating if Modified Request in createifModifiedRequest \n");    
    int reqlen = strlen(irequest);
    bzero(ifrequest,MAX_STR_LEN);
    strncpy (ifrequest ,irequest,reqlen-2);
    ifrequest[reqlen-2] = '\0';
    ifrequest = (char *)strcat( ifrequest, "If-Modified-Since: ");
    ifrequest = (char *)strcat( ifrequest, time); 
    ifrequest = (char *)strcat( ifrequest, "\r\n"); 
    ifrequest = (char *)strcat( ifrequest, "\r\n"); 
    printf("if modiefied request created is \n%s\n",ifrequest);
}

/* 
 * returns the expires time/last modified date/date  of the response
*/

bool get_data_time(char *data,char *time)
{   printf("Inside get_data_time\n");
    char *p = NULL;
    char *pend=NULL;
    
    if((p = strstr(data,"Expires:")) != NULL)
    {
        printf("Response from Server contains Expires Field\n");
		p+=9;
		pend = strstr(p,"\r\n");
		if (pend==NULL) return -1;
		int l = pend-p;
		bzero(time,l);
		memcpy(time,p,l);
		return true; //if time is expiretime
		
	}
	else if((p = strstr(data,"Last-Modified:"))!=NULL)
	{
	    printf("Response from Server contains LastModified Field\n");
	    p+=15;
		pend = strstr(p,"\r\n");
		if (pend==NULL) return -1;
		int l = pend-p;
		bzero(time,l+2);
		memcpy(time,p,l);
		return false;
	}
	else
	{
	    printf("Response from Server contains Date Field\n");
	    p = strstr(data,"Date:");
		p+=5;
		pend = strstr(p,"\r\n");
		if (pend==NULL) return -1;
		int l = pend-p;
		bzero(time,l);
		strncpy(time,p,l);
		return false;
	}
	printf(" time in Server Response is %s\n",time); 
	return false;
    
}

int get_data_len(char *data)
{
    char *p = NULL;
    char *pend=NULL;
    p = strstr(data,"Content-Length:");
    if (p==NULL) return -1;
    p+=15;
    pend = strstr(p,"\r\n");
    if (pend==NULL) return -1;
    int l = pend-p;
    char *len=NULL;
    len = (char *)malloc(l);
    memcpy(len,p,l);
    return atoi(len);
}

int get_server_header(char *data, char **header_data,int *header_len, char **other_data,int *other_len) {
    char *p = NULL;
    int read_len = strlen(data)-1;
    p = strstr(data,"\r\n\r\n");
    if(p==NULL) return -1;
    *header_len = p-data;
    *header_len+=4;
    *other_len = read_len-*header_len;
    *header_data = (char *)malloc(*header_len+1);
    *other_data = (char *)malloc(*other_len+1);
    memset(*header_data,'\0',*header_len+1);
    memset(*other_data,'\0',*other_len+1);
    memcpy(*header_data,data,*header_len);
    memcpy(*other_data,data+*header_len,*other_len);
    return *header_len;
}




bool processandsendRequest(char *request,char *hostname,int port,int i,cacheEntry *oldentry)
{
 printf("Inside processandsendRequest \n");
 printf("Creating Connection to Server\n");
  int newfd;
  struct sockaddr_in serv_addr;
  struct hostent *hostserver;
  newfd = socket(AF_INET, SOCK_STREAM, 0);
  if (newfd < 0)  
   {
    error("error opening socket\n");
    exit(0);
   }
   hostserver = gethostbyname(hostname);
   int hostport = port;
   if (hostserver == NULL)
   {
   fprintf(stderr,"error, no such host\n"); 
   exit(0);
   }

   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)hostserver->h_addr,(char *)&serv_addr.sin_addr.s_addr,hostserver->h_length);
   serv_addr.sin_port = htons(hostport);

   if (connect(newfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
   {
    error(" connecting\n");
    exit(0);
   }
   printf("Request send to Server is\n%s\n",request);
   if (send(newfd, request,strlen(request), 0) == -1)
       perror("error in sending MSG to server\n");
   //printf("send to server with newfd\n");
   int size = 0;
   int nbytes =0;
   char buf[MAX_STR_LEN];
   memset(buf,0,MAX_STR_LEN);
   char *header_data=NULL;
   char *other_data =NULL;
   int header_len,content_len,data_len; 
   bool firstbuf=true;
   bool sendFromCache = false;
   bool isTimeExpire = true;
   char *filename=NULL;
   //char *etime=NULL;
   char etime[MAX_STR_LEN];
   FILE *file;
   char *fname =NULL;
   fname = "file";
   filename = (char*)malloc(100);
   bzero(filename,100);
   //printf("Just before Oldentry check\n");
      
   //now read from server   
   do
   {  
     nbytes  = recv(newfd, buf,MAX_STR_LEN,0);    
     //printf("Receive %d from server\n",nbytes);
     if(nbytes<0)
     {
      perror("recv error from server \n");
      exit(0);
     }

     char*  p =strstr(buf,"304 Not Modified");
     
     if(p!=NULL)
     {
	  printf("Response is 304 Not Modified,So sends cache data"); 
	  sendFromCache = true;
	  updateremoveaddtoHead(request,NULL,NULL);
	  
	 //#if 0 
	 printf("\n*******************************************\n\n");
     print_cacheEntry();
     printf("\n********************************************\n\n");
     //#endif     
     
	 return sendFromCache;
	 }
    else
       printf("Response from Server is not 304, means Change in Data from Host\n");

     printf("Response from Server is 200 Okay,Sending data\n");
     printf("Reading From Server\n");
     get_server_header(buf,&header_data,&header_len,&other_data,&data_len);
     
     if(firstbuf)
      {
        
        if(oldentry != NULL)
        {
          printf("Request Already In Cache\n");
       	  filename = oldentry->filename;
          printf("Filename with Request is %s\n",filename);     
          file = fopen(filename,"wb+");
          if(file==NULL) { printf("error in opeining file when oldentry is not null");exit(0);}
          //fseek(file, 0, SEEK_SET); /* apend file (add text to a file or create a file if it does not exist at begining*/            
    	}
   		else
    	{
       	  printf("Request is New\n");
          //request is new and create file name and buffers data in that and then send to client
          sprintf(filename,"%s%d",fname,count);
          //printf("filename is %s\n",filename);
	      file = fopen(filename,"wb+"); /* apend file (add text to a file or create a file if it does not exist.*/        
	      if(file==NULL) { printf("error in opeining file when oldentry is not null");exit(0);}
	      //fseek(file, 0, SEEK_SET);    
	      //printf("End of else of getting filename\n");
        }
     	content_len   = get_data_len(buf);
     	//char etime[MAX_STR_LEN];
     	memset(etime,0,MAX_STR_LEN);
     	isTimeExpire =  get_data_time(buf,etime);
     	
     	//printf("#########line 319 time fetch from request is %s\n",etime);
        firstbuf= false;
      }
     
     #if 0
     fetch 200 Ok or 304 Ok. if 304 then break this loop and fetch from the cache else continue do its
     Request
		GET /index.html HTTP/1.0
		Host: www.example.com
		If-Modified-Since: Wed, 19 Oct 2005 10:50:00 GMTe:  Fri Dec 31 23:59:59 1999
     Response
        HTTP/1.1 304 Not Modified
	 	Date: Fri, 31 Dec 1999 23:59:59 GMT
		[blank line here]
		
	 #endif
	 
	
     //printf("headerlen is %d,Datalen is %d and contentlen is %d\n",header_len,data_len,content_len);
     //printf("size before is  %d and nbytes read is %d\n",size,nbytes);
     printf("\n\n*********Reading Data from Server************\n\n");
     size +=data_len;
     //printf("size soo far is %d\n",size);
     
     int mbytes = send(i,buf,nbytes,0);
     //printf("bytes send to client is %d\n",mbytes);
     if(mbytes<0)
      {
       perror("recv 0 from server \n");
       exit(0);
      }
       fwrite (buf , 1 ,mbytes , file );//write data to file
     memset(buf,0,MAX_STR_LEN);  
     //printf("\n\n***********************************\n\n");
   }while(size <= content_len );
  fflush(file); 
  fclose(file);
  //Update Cache Here
  //update cacheEntry corresponding to request Expire Time and add this in front
  //fetch expiretime from function.returs true if fetched time is Expire time else false for date modified
  if(oldentry == NULL) //means newentry then have to add to cache
  {
   if(count < MAX_ENTRY)
   {    
    printf("Adding Entry to Cache\n");
     add_entry(request,etime,filename,isTimeExpire);
     count++;
     printf("Count of cache entry is %d\n",count);
     //#if 0 
	 printf("\n*******************************************\n\n");
     print_cacheEntry();
     printf("\n********************************************\n\n");
     //#endif
   }
   else
   {
     
     printf("Deleting from last and Adding to front\n");
     delete_entry();
     add_entry(request,etime,filename,isTimeExpire);
     
     printf("Count of cache entry is %d\n",count);
     //#if 0 
	 printf("\n*******************************************\n\n");
     print_cacheEntry();
     printf("\n********************************************\n\n");
     //#endif
     
   }
  }
  else
  {
    printf("Updating Cache Entry.\nRemoving and Adding to Front\n"); 
     updateremoveaddtoHead(request,etime,isTimeExpire);
   
     printf("Count of cache entry is %d\n",count);
     //#if 0 
	 printf("\n*******************************************\n\n");
     print_cacheEntry();
     printf("\n********************************************\n\n");
     //#endif
   //this function checks for entry first, if present,remove it and add to head, else add to head directly
  }
  printf("\nTotal recieved response bytes from Server: %d\n",size);
  printf("\n**********Ready To Receive Client Request************\n\n");
 close(newfd);  
 return sendFromCache;        
  printf("\n**********Ready To Receive Client Request************\n\n");
}
void sendCacheFile(char* fname,int clientfd)
{
    printf("Inside sendCacheFile\n");
 		printf("Sending Cache File to Client\n");
	     char* filename = fname;
	     char sendbuf[MAX_STR_LEN];
	     bzero(sendbuf,MAX_STR_LEN);
	     
	     //int block =1;   
		 unsigned long fileLen;
		 //int nbytes=0;
	     //Get file length         
	     FILE* fp = fopen(filename,"rb");
	    
	      fseek(fp, 0, SEEK_END);
		 fileLen=ftell(fp);
		 fseek(fp, 0, SEEK_SET);
  		  //printf("fiesize from cache is %ld\n",fileLen);
  		  double n=0;
	     while(n != fileLen)
	     {
	       int nbytes = fread(sendbuf,1,MAX_STR_LEN,fp);
	       n= n +nbytes;
	       int mbytes = send(clientfd,sendbuf,nbytes,0);
     	    printf("bytes send to client from Cache File is %d\n",mbytes);
     	   if(mbytes<0)
      	   {
       	 	perror("recv 0 from server \n");
       		exit(0);
      	   }
      	   memset(sendbuf,0,MAX_STR_LEN);
	     }    
	     fclose(fp); 
}


void chkforIfModified(cacheEntry* oldEntry,int clientfd)
{     
	   printf("Inside chkforIfModified Request \n");
	   char ifModRequest[MAX_STR_LEN];
   	   bzero(ifModRequest,MAX_STR_LEN); 
   	   createifModifiedRequest(ifModRequest,oldEntry->request,oldEntry->time);//return modified Request
   	   printf("IfModified Request is\n%s\n",ifModRequest);
   	   bool sendFromCache;
   	   char* whost =NULL;
   	   //get host from request
       int k = getHostFromRequest(ifModRequest,&whost);
       //int j;
       //printf("In cacheEntryFound\n");
       //for(j=0;j<k;j++)
       //printf("char is %c\n",whost[j]);
       printf("\nHost after in cacheEntry getfromHost is %s\n",whost);
       char* ahost =NULL;
       ahost =(char*)malloc(k+4);  
       //char ahost[k];
       bzero(ahost,k+4);
       memcpy(ahost,whost,k);
       //printf("hostname is after cacheEntryFound after %s\n",ahost);
	   sendFromCache = processandsendRequest(ifModRequest,ahost,80,clientfd,oldEntry); //inside processandsendRequest updateCache
	   //if true means response was 304,else functio already send the data to clientfd
	   free(whost);
	   free(ahost);
	   if(sendFromCache)
	   { //send from cache request
	     printf("Sending Data from Cache only\n");
	     sendCacheFile(oldEntry->filename,clientfd);    
	     printf("updating the Cache Entry and bringing this entry to front\n");
	      printf("\n**********Ready To Receive Client Request************\n\n");
	     
	   }//end of sendfromcache
}

void cacheEntryFound(cacheEntry *oldEntry,int clientfd)
{
    printf("Inside cacheEntryFound \n");
    //printf("Inside cacheEntryFound assuming data is present in cache\nlet data present earlier was expire time\n");
  bool expiretime = oldEntry->isTimeExpire;
  printf("Inside cacheEntry and Value of Expiretime is %d\n",oldEntry->isTimeExpire);
  //printf("Time present in Cache for Entry is expire time and is:%s\n",oldEntry->time);
  //expiretime = true; 
  if(expiretime)
  {  
       time_t rawtime;
   	time_t currenttimeinsec,expiretimeinsec;
   	currenttimeinsec =  time(NULL) + 18000;
      
	struct tm * timeinfo;

  	time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
  	printf ( "Current local time and date: %s", asctime (timeinfo) );

   	printf("Currenttime is: %ld\n",currenttimeinsec);
   	printf("time Inside Cache is Expire time is\n%s\n",oldEntry->time);
    expiretimeinsec = get_time_sec(oldEntry->time);
    printf("expiretimeinsec is: %ld\n",expiretimeinsec);
   
// for testing purpose &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
time_t diff = expiretimeinsec - currenttimeinsec;
 printf("DIfference between both is @@@@@@@@@@@@@@@@@@@@  %d\n",diff);

  	if((expiretimeinsec - currenttimeinsec) > 0) //means not expire and return the filename and send data from file to client 
    {
    printf("Time is not expire and sending from cache\n");
    sendCacheFile(oldEntry->filename,clientfd);
     //update the Cache List
    updateremoveaddtoHead(oldEntry->request,NULL,NULL);
    
    printf("Count of cache entry is %d\n",count);
     //#if 0 
	 printf("\n*******************************************\n\n");
     print_cacheEntry();
     printf("\n********************************************\n\n");
     //#endif
   
    
    }//end of not expiretime
   	else //means data expire, send request to server and get data, update the cache structure with new information
    {
       printf("Time is expired and checking if previous data got modified or not by sending ifModified Request\n");
      chkforIfModified(oldEntry,clientfd);           
    } //end of data expire
  }//end of expiretime truegetHostFromRequest
  else //if expiretime=false,then time is date modiefiled
  {
   	   chkforIfModified(oldEntry,clientfd); 	   
  }//end of else of expire time
  close(clientfd);       
}

/* 
 *  
 */


void extract_URL(char *url, char *hostname, int *port, char *identifier)
{
    printf("Extracting URL from Request\n");
    char protocol[MAX_STR_LEN], scratch[MAX_STR_LEN], *ptr=0, *nptr=0;
    strcpy(scratch, url);
    ptr = (char *)strchr(scratch, ':');
    if (!ptr)
    {
	fprintf(stderr, "Wrong url: no protocol specified\n");
	exit(0);
    }
    strcpy(ptr, "\0");
    strcpy(protocol, scratch);
    if (strcmp(protocol, "http"))
    {
	fprintf(stderr, "Wrong protocol: %s\n", protocol);
	exit(0);
    }

    strcpy(scratch, url);
    ptr = (char *)strstr(scratch, "//");
    if (!ptr)
    {
	fprintf(stderr, "Wrong url: no server specified\n");
	exit(0);
    }
    ptr += 2;

    strcpy(hostname, ptr);
    nptr = (char *)strchr(ptr, ':');
    if (!nptr)
    {
	*port = 80; /* use the default HTTP port number */
	nptr = (char *)strchr(hostname, '/');
    }
    else
    {	
	sscanf(nptr, ":%d", port);
	nptr = (char *)strchr(hostname, ':');
    }

    if (nptr)
      *nptr = '\0';

    nptr = (char *)strchr(ptr, '/');
    
	if (!nptr)
	    {
	fprintf(stderr, "Wrong url: no file specified\n");
	exit(0);
    }
    
    strcpy(identifier, nptr);

}



char* createRequest(char *request,char * hostname,char *identifier) 
{
    printf("Create Request for Sending to Server\n");
        char mem[MAX_STR_LEN];
        request = mem;	
	strcpy (request ,"GET ");
	request = (char *)strcat( request, identifier);
	request = (char *)strcat( request, " HTTP/1.0\r\nHOST: ");
	request = (char *)strcat( request, hostname);
	request = (char *)strcat( request, "\r\n"); 
	request = (char *)strcat( request, "\r\n"); 
        return request;
}




int main(int argc, char *argv[])
{
 if (argc < 3) {
   fprintf(stderr,"usage %s SERVER_IP PORT_NAME MAX_CLIENTS\n", argv[0]);
   exit(0);
  } 
 printf("\n**************Welcome To Http Proxy Server************\n\n");
 fd_set master_list;
 fd_set read_fds;
 int fdmax,sockfd,newfd,nbytes;
 // listening socket descriptor
 struct sockaddr_storage remoteaddr; // client address
 socklen_t addrlen;
 //char buffer[MAX_STR_LEN];
 //char buf[MAX_STR_LEN];
 char request[MAX_STR_LEN]; 
 //char hostname[MAX_STR_LEN];
 //char *hostname =NULL;
 //char identifier[MAX_STR_LEN];
 // buffer for client data

 int yes=1,i=0,rv;
 // for setsockopt() SO_REUSEADDR, below
 struct addrinfo hints, *ai, *p;
 FD_ZERO(&master_list);
 FD_ZERO(&read_fds);
 // clear the master_list and temp sets
 // get us a socket and bind it
 memset(&hints, 0, sizeof hints);
 hints.ai_family = AF_INET;
 hints.ai_socktype = SOCK_STREAM;
 hints.ai_flags = AI_PASSIVE;
 if ((rv = getaddrinfo(argv[1], argv[2], &hints, &ai)) != 0) {
  fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
  exit(1);
 }
 for(p = ai; p != NULL; p = p->ai_next) 
 {
   sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
   if (sockfd < 0) {
      continue;
      }
  // lose the pesky "address already in use" 0 message
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  if (bind(sockfd, p->ai_addr, p->ai_addrlen) < 0) {
      close(sockfd);
      continue;
      }
     break;
  }
 // if we got here, it means we didn't get bound
 if (p == NULL) {
   fprintf(stderr, "selectserver: failed to bind\n");
   exit(2);
  } 
  freeaddrinfo(ai); // all done with this
  // listen
  if (listen(sockfd, 10) == -1) 
   {
    perror("listen\n");
    exit(3);
   }
 // add the sockfd to the master_list set
 FD_SET(sockfd, &master_list);
 fdmax = sockfd; // so far, it's this one

 while (1) 
   { 
     read_fds = master_list; // copy it
     //printf("before select\n");
     if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) 
     {
            perror("error in select\n");
            exit(4);
     }
     //printf("select returns\n");
     for(i=0;i<=fdmax;i++)
      {
       if (FD_ISSET(i, &read_fds))
      	{
	     if(i==sockfd) //new client added to system
	      {     
		    addrlen = sizeof remoteaddr;
       	  	newfd = accept(sockfd,(struct sockaddr *)&remoteaddr,&addrlen);
	        if (newfd < 0)  printf("error on accept\n");
	     	FD_SET(newfd,&master_list); //ADD new client to readfs file
          	if(newfd>fdmax) fdmax=newfd;  //set it to max
         	
          }
	     else // handle data from a client
          {
            if ((nbytes = recv(i,request, MAX_STR_LEN, 0)) <= 0) //client hungup
			 { 
               if (nbytes == 0) //client leaves the chat session.have to inform all other clients about its departure
		 		{
 	    	   	 printf("\nClient Exited\n");
				} 
		 		else 
		 		{
                 perror("error in receving bytes\n");
               	}
             close(i);
             FD_CLR(i,&master_list);  	
	        }
	        else
	        {    
	        	 printf("\n**********Ready To Receive Client Request************\n\n");
                 printf("Request from client is %s\n",request);
                 
                 //check request in cacheEntry, if found return pointer to cacheEntry else NULL
                 cacheEntry* oldEntry =(cacheEntry*)search_entry(request);
                 if(oldEntry != NULL)
                 { 
                   printf("Entry Present in cacheEntry\n");
                   cacheEntryFound(oldEntry,i);
                   close(i);
                   FD_CLR(i,&master_list);
                 }
                 else //if oldEntry is NULL, means there is no entry in cacheEntry for this Request
                 {
                   printf("New Request, add to cacheEntry\n");
                   //process the request receive from above,create connection to webserver and then send it to client sockfd(i)
                   char *whost =NULL;
       			   int k = getHostFromRequest(request,&whost);
    
       			   char* ahost =NULL;
       			   ahost = (char*)malloc(k);  
			       bzero(ahost,k);
      			   memcpy(ahost,whost,k);
			       //printf("hostname is after after %s\n",ahost);
                   //printf("hostname is %s\n",whost);
	       	       processandsendRequest(request,ahost,80,i,NULL); // this will also update the cacheEntry,NULL means newCacheEntry 
	       	       free(whost);
	       	       free(ahost);
	  	           close(i);
                   FD_CLR(i,&master_list);
		         }		
		     }//recvbuf >0
            } //end handle data from client   
           } //end getting new connections
         } //end of for loop for each file descriptor for(int i=0;i<=fdmax;i++)
       } //end of while
     close(sockfd);
     return 0; 
}


