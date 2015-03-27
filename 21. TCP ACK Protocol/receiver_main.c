#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>

struct sockaddr_in server;
struct sockaddr_in from;
int fromlen;
FILE *fp;
static int seen=0;
static int buf_index=0;
int *win_id_array;
unsigned long long int *last_ack;
unsigned long long int min_offset=0;
unsigned long long int read_window=0;
unsigned long long int last_window=0;
unsigned long long int last_window_pack_size=0;
unsigned long long int total_buffer_size=0;
unsigned long long int last_window_flag=0;
unsigned long long int current_window;
unsigned long long int max_window_ack;
void reliablyReceive(int *sock_fd,char*filename);
char* readData(char *buf,char *filename,unsigned long long int* last_ack, char * message_buff);
unsigned long long int find_ack(unsigned long long int seq_no, unsigned long long int last_ack);
pthread_mutex_t file_mutex;
int offset;
int buf_length;
void handler(int sig)
{
	
	exit(0);
}

typedef struct info
{
	char *buff;
	unsigned long long int buf_length;
	unsigned long long int offset;
}info_t;
void * writer_thread ( void *ptr )
{
	
	
	char *data=(char*)ptr;
	//fprintf(stderr,"\noffset %d length %d \n\n ",offset,buf_length);

	//info_t * packet;
	//packet=(info_t *)ptr;
	//fprintf(stderr,"offset %d length %d ",packet->offset,packet->buf_length);
		int i;
	//for(i = 0;i<buf_length;i++)	
	//	fprintf(stderr,"%c", ((data[i])));
//	fprintf(stderr,"\n");	
	
	fseek(fp,offset,SEEK_SET);	
	fwrite(data,1,buf_length,fp);
	free(data);
	///fprintf(stderr,"\n\nthread over\n");
	pthread_mutex_unlock(&file_mutex);
	
	return NULL;
}
int main(int argc, char** argv)
{
	int udpPort;
	if(argc != 3)
	{
		//fprintf(stderr, "usage: %s UDP_port filename_to_write\n\n", argv[0]);
		exit(1);
	}
	pthread_mutex_init(&file_mutex,NULL);
	udpPort = atoi(argv[1]);
	fromlen = sizeof( struct sockaddr_in ); 
	int * sock_fd=malloc(sizeof(int));   	
	*sock_fd=socket(AF_INET,SOCK_DGRAM,0);
	// fprintf(stderr,"unable to read ");
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;   /* ok from any machine */
	server.sin_port = htons(udpPort);   
	int yes=1;
	if (setsockopt(*sock_fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1)
 	{
   		//fprintf(stderr,"Unable to connect maybe Host not found or wrong port\n");
   	} 
	if (bind(*sock_fd,(struct sockaddr *) &server, sizeof(server))) 
	{
	
		//perror("binding udp socket\n");
		exit(1);
	}
	fflush(stderr);
	signal(SIGINT,handler);
	reliablyReceive(sock_fd,argv[2]);
}

void reliablyReceive(int *sock_fd,char* filename)
{
	fp = fopen(filename, "wb+");
	fclose(fp);
	fp = fopen(filename, "rb+");
	last_ack=malloc(sizeof(long long int));
	*last_ack=1;
	char *message_buff;
	char *message_buff2;
	int flag=1;
	char ack[50];
	for ( ;; )
	 {     /* do forever */
		//fprintf(stderr,"Waiting to Read \n");
		int rc;
		char *buf;
		buf=malloc(sizeof(char)*2000);
		memset(buf,'\0',(sizeof(char)*2000));
		if ((rc=recvfrom(*sock_fd, buf, 2000, 0, (struct sockaddr *)&from, &fromlen)) < 0 ) 
		{
			//printf("server error: errno %d\n",errno);
			//perror("reading datagram");
			exit(1);
		}
		else
		{
			
			if(flag%2==0)
				message_buff=readData(buf,filename,last_ack,message_buff);
			else
				message_buff2=readData(buf,filename,last_ack,message_buff2);
			if((read_window>=last_window && seen==1)|| *last_ack==0)
			{
				read_window=0;
				seen=0;
				memset(ack,'\0',sizeof(ack));
				sprintf(ack,"%lld",*last_ack);
				//fprintf(stderr,"asking ack %s\n",ack);
				while(sendto(*sock_fd,ack,strlen(ack),0,(struct sockaddr *)&from,sizeof (struct sockaddr_in))==-1)	
				{
					fprintf(stderr,"\nACK NOT SEND ");
				}
				pthread_mutex_lock(&file_mutex);
				pthread_t thread1;
				//fprintf(stderr,"offset %d\n",min_offset);
				offset=min_offset;
				buf_length=total_buffer_size;
				//info_t* packet =malloc(sizeof(info_t));
				//packet->buff=message_buff;
				//packet->buf_length=total_buffer_size;
				//packet->offset=min_offset;
				//fprintf(stderr,"offset %d length %d ",packet->offset,packet->buf_length);
				//int i;
				//for(i = 0;i < total_buffer_size;i++)	
				//	fprintf(stderr,"%c", ((char *)message_buff)[i])	;
				if(flag%2==0)
					pthread_create (&thread1, NULL,&writer_thread,(void *)message_buff);
				else
					pthread_create (&thread1, NULL,&writer_thread,(void *)message_buff2);
				flag++;
				if(*last_ack==0)
				{
					pthread_join(thread1, NULL);
					fclose(fp);
					exit(0);
				}	
				//pthread_join(thread1, NULL);
				//fprintf(stderr,"writng to file\t");	
				//fseek(fp,0,SEEK_END);
				//fprintf(stderr," total_size %d\n",total_buffer_size);
				//total_buffer_size=0;
				//fprintf(stderr,"\n\nwriten to file \n");
				//fprintf(stderr,"\nACK SEND ");	
			}			
		}
	
	}
}

char* readData(char *buf,char *filename,unsigned long long int* last_ack, char * message_buff)
{
	char seq[50];
	unsigned long long int seq_no;
	unsigned long long int pack_size;
	unsigned long long int offset_no;
	unsigned long long int window_size;
	unsigned long long int window_id;
	int change_window_flag;
	char packet_size[50];
	char offset[50];
	char win_size[50];
	char win_id[50];
	char mssg[3000];//this is the max packet size we will kwwp
	char last_win[20];
	memset(mssg,'\0',sizeof(mssg));
	memset(win_id,'\0',sizeof(win_id));
	memset(seq,'\0',sizeof(seq));
	memset(last_win,'\0',(sizeof(last_win)));
	memset(win_size,'\0',sizeof(win_size));
	memset(packet_size,'\0',sizeof(packet_size));
	unsigned long long int i,j,k=0,l=0,m=0,o=0,p=0,v=0,a=0,b=0,t=0,c=0;
	for(i=0;i<3000;i++)
	{
		if(buf[i]!='.')
		{
			seq[i]=buf[i];
			//fprintf(stderr,"%c", ((char *)buf)[i]);
		}
		else
		{	
			seq_no=atoll(seq);
			for(j=i+1;j<3000;j++,k++)
			{
				if(buf[j]!='.')
				{
					offset[k]=buf[j];
					//fprintf(stderr,"%c", ((char *)buf)[j]);
				}
				else
				{
					offset_no=atoll(offset);
					for(l=j+1;l<=3000;l++,m++)
					{
						if(buf[l]!='.')
						{
							packet_size[m]=buf[l];	
							//fprintf(stderr,"%c", ((char *)buf)[l]);
						}
						else
						{
							pack_size=atoll(packet_size);
							for(o=l+1;o<=3000;o++,p++)
							{	if(buf[o]!='.')
									win_size[p]=buf[o];
								else
								{	o=o+1;
									while(buf[o]!='.')
										last_win[c++]=buf[o++];
									change_window_flag=atoi(last_win);	
									window_size=atoll(win_size);
									current_window=window_size;
									//fprintf(stderr,"current window last size flag %d %d %d %d",window_size,last_window,last_window_size,last_window_flag);
									if((last_window!=current_window) ||  ( (change_window_flag>last_window_flag) ))
									{	
										//pthread_mutex_lock(&file_mutex);
										seen=1;
										min_offset=offset_no;
										last_window=current_window;
										message_buff=malloc(sizeof(char)*(window_size*1000));
										win_id_array=malloc(sizeof(int)*window_size);
										memset(message_buff,'\0',(sizeof(char)*window_size*1000));
										memset(win_id_array,-1,(sizeof(int)*window_size));
										last_window_pack_size=pack_size;
										total_buffer_size=0;
										last_window_flag=change_window_flag;
										//fprintf(stderr,"\n\nnew window\n \n");
										max_window_ack=seq_no;
										//pthread_mutex_unlock(&file_mutex);
			
									}	
									for(v=o+1;v<=3000;v++,t++)
									{
										if(buf[v]!=' ')
											win_id[t]=buf[v];
										else
										{	
											
											window_id=atoll(win_id);
											if(win_id_array[window_id]==-1 && seq_no!=0)//this is not a duplicate add to buffer
											{ 
												win_id_array[window_id]=window_id;
												//fprintf(stderr,"got packet %d id %d size %d WROTE\n",seq_no,window_id,window_size);
												for(a=v+1,b=window_id*pack_size;a<=pack_size+v;a++,b++)
												{
													message_buff[b]=((char *)buf)[a];			
													//fprintf(stderr,"%c", ((char *)buf)[a]);
												}
												//fprintf(stderr,"wrote ");
												//fprintf(stderr," seq %d\n",seq_no);
												total_buffer_size+=pack_size;	
												read_window++;
													break;		
											}
											else if(seq_no==0)
											{	
												//fprintf(stderr,"got packet %d id %d size %d\n",seq_no,window_id,window_size);	
												win_id_array[window_id]=window_id;
												for(a=v+1,b=window_id*last_window_pack_size;a<=pack_size+v;a++,b++)
												{
													message_buff[b]=((char *)buf)[a];			
													//fprintf(stderr,"%c", ((char *)message_buff)[b]);
												}
												total_buffer_size+=pack_size;
												read_window++;
												//fprintf(stderr,"got last packet\n");
												
													break;		
											}
											else	
											{
												//fprintf(stderr,"got DUP packet %d id %d size %d\n",seq_no,window_id,window_size);	
												break;
											}
										}
									}
									break;	
								}	
								
							//	fprintf(stderr,"%c", ((char *)buf)[o]);
							}
							break;
						}
					}
					break;	
				}	
		 	}
		 	break;		
		}		
	}
	//fprintf(stderr,"total_buffer %d\n",total_buffer_size);
	free(buf);	
	if(min_offset>offset_no)
		min_offset=offset_no;
	if(seq_no>max_window_ack)
		max_window_ack=seq_no;								
	//fprintf(stderr,"\n--\n");
	//fprintf(stderr,"seq_no %lld offset %lld packet_size %lld window %lld\n",seq_no,offset_no,pack_size,window_size);
	if(read_window>=last_window)
	{
		if(seq_no==0)
			*last_ack=0;
		else	
			*last_ack= max_window_ack+1;
	}
	else if(seq_no==0)
	{
		//fprintf(stderr,"DUPLICATE PACKET DETECTED\n");
		*last_ack=0;
	}
	else
		*last_ack=1;
	
	return message_buff;
}

