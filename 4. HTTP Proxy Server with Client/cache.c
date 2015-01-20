#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

typedef struct {
    
    char *request;
    char *time;
    char *filename;
    bool isTimeExpire;
    struct cacheEntry *next;
} cacheEntry;


typedef cacheEntry * cacheptr;
cacheEntry *head=NULL;


int add_entry(char *addrequest,char *time,char *filename,bool isTimeExpire)
{
  cacheEntry *newentry = (cacheEntry*)malloc(sizeof(cacheEntry));
  if(newentry==NULL)
  {
   printf("Error in Allocating Memory to new entry\n");
   return -1;
  }
  newentry->request = (char*)malloc(strlen(addrequest));
  bzero(newentry->request, strlen(addrequest));
  strcpy(newentry->request,addrequest);
  
  newentry->time = (char*)malloc(strlen(time));
  bzero(newentry->time, strlen(time));
  strcpy(newentry->time,time);
   
  newentry->filename = (char*)malloc(strlen(filename));
  bzero(newentry->filename, strlen(filename));
  strcpy(newentry->filename,filename);
  
  newentry->isTimeExpire=isTimeExpire;
  newentry->next =(cacheptr)head;
  head=(cacheEntry*)newentry;
  printf("New Request is Added to Front\n");
  
 return 0;
}

struct cacheEntry * search_entry(char *requestrecv)
{
 if(head==NULL) {return NULL;} 
 cacheEntry * temp=head;
 while(temp!=NULL)
 { 
  if(!strncmp(temp->request,requestrecv,strlen(temp->request+1)))
  {  
   printf("Request Exist in Cache\n");
   return (cacheptr)temp;
  }
  else
   temp=(cacheEntry*)temp->next;
 }
 return NULL;
}

void print_cacheEntry()
{
 cacheEntry *temp=head;
 int i=1;
 while(temp!=NULL)
  {
   printf("===============Entry %d Start======================\n\n",i);
   printf("Request:\n%s\nTime:%s\nFilename:%s\nIsTimeExpire:%d\n",
   temp->request,temp->time,temp->filename,temp->isTimeExpire);
   		
   i++;
   temp=(cacheEntry*)temp->next;
  }
}

/*
* Removes node from the last
*/

int delete_entry()
{
 cacheEntry *prev=head;
 cacheEntry *temp=(cacheEntry*)head->next;
 while(temp->next!=NULL)
 {
  temp=(cacheEntry*)temp->next;
  prev=(cacheEntry*)prev->next;
 }
  prev->next=NULL;
  printf("Cache Full \nDeleting Entry From last\n");
  free(temp);
  return 0;
}

int updateremoveaddtoHead(char *requestrecv,char *time,bool isTimeExpire)
{
 cacheEntry *temp, *prev;
 temp=head;
 
 while(temp!=NULL)
 {
   if(!strncmp(temp->request,requestrecv,strlen(temp->request)-2))
   {  
    
    if(time)
    	temp->time = time;
    if(isTimeExpire)	
    	temp->isTimeExpire = isTimeExpire; 
    if(temp==head)
    {
    
    
    return 0;
    }
    else
    {
    
    prev->next=temp->next;
    
 	cacheEntry *newentry = (cacheEntry*)malloc(sizeof(cacheEntry));
    if(newentry==NULL)
    {
      printf("Error in Allocating Memory to new entry\n");
      return -1;
    }
    newentry->request = (char*)malloc(strlen(temp->request));
    bzero(newentry->request, strlen(temp->request));
    strcpy(newentry->request,temp->request);
  
  	newentry->time = (char*)malloc(strlen(temp->time));
  	bzero(newentry->time, strlen(temp->time));
  	strcpy(newentry->time,temp->time);
  
  	newentry->filename = (char*)malloc(strlen(temp->filename));
  	bzero(newentry->filename, strlen(temp->filename));
  	strcpy(newentry->filename,temp->filename);
  
  	newentry->isTimeExpire=temp->isTimeExpire;
  	newentry->next = head; 
    head = newentry;
    free(temp);
    return 0;
    }
  }
  else
  {
   prev=temp;
   temp= (cacheEntry*)temp->next;
  }
 }
 
return -1;

}

time_t get_time_sec(char * tame)
{
struct tm tm;
time_t t;




if (strptime(tame, "%a, %d %b %Y %H:%M:%S %Z", &tm) == NULL)
    /* Handle error */;

#if 0
printf("year: %d; month: %d; day: %d;\n",
        tm.tm_year, tm.tm_mon, tm.tm_mday);
printf("hour: %d; minute: %d; second: %d\n",
        tm.tm_hour, tm.tm_min, tm.tm_sec);
printf("week day: %d; year day: %d\n", tm.tm_wday, tm.tm_yday);
#endif

tm.tm_isdst = -1;      /* Not set by strptime(); tells mktime()
                          to determine whether daylight saving time
                          is in effect */
t = mktime(&tm);
if (t == -1)
    /* Handle error */;
printf("seconds since the Epoch: %ld\n", (long) t);

return t;
}
