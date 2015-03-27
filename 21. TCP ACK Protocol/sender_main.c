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
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

struct addrinfo hints,*result,*ressave;
struct sockaddr *sa;
socklen_t sa_len;
//pthread_mutex_t file_mutex;
unsigned long long int total_length;
char* filename;
FILE *fp;
int increase_size = 50;

char * create_message(unsigned long long int packet_size,unsigned long long int offset,unsigned long long int seq,unsigned long long int window_size,unsigned long long int window_id,unsigned long long int last_window)
{	
	char * mssg;
	char *buf;
	char seq_no[50];//will need to change this value
	char mssg_offset[50];//will need to change this value
	char pack_size[50];
	char win_size[50];
	char win_id[50];
	char last_win[50];
	buf = malloc(sizeof(char)*packet_size+1);
	mssg = malloc(sizeof(char)*packet_size+offset+4+packet_size+window_size+window_id);//final mssg to be send
	memset(pack_size,'\0',(sizeof(pack_size)));
	memset(win_id,'\0',(sizeof(win_id)));
	memset(last_win,'\0',(sizeof(last_win)));
	memset(win_size,'\0',(sizeof(win_size)));
	memset(seq_no,'\0',(sizeof(seq_no)));
	memset(mssg_offset,'\0',(sizeof(mssg_offset)));
	memset(buf,'\0',(sizeof(char)*packet_size)+1);// this may slow up but is a must else we may read junk
	memset(mssg,'\0',(sizeof(char)*packet_size+offset+4+packet_size+window_size+window_id));//this may slow up
	sprintf(seq_no,"%lld",seq);
	sprintf(win_size,"%lld",window_size);
	sprintf(pack_size,"%lld",packet_size);
	sprintf(win_id,"%lld",window_id);
	sprintf(mssg_offset,"%lld",offset);
	sprintf(last_win,"%lld",last_window);
	strcpy(mssg,seq_no);
	strcat(mssg,".");
	strcat(mssg,mssg_offset);
	strcat(mssg,".");
	strcat(mssg,pack_size);
	strcat(mssg,".");
	strcat(mssg,win_size);
	strcat(mssg,".");
	strcat(mssg,last_win);
	strcat(mssg,".");
	strcat(mssg,win_id);
	strcat(mssg," ");
	fflush(stderr);
	int len=strlen(mssg); //[encoding is]= packet_no.offset.packet_size.window_size.last_winodow_flag.win_id
	//fprintf(stderr,"message =%s\n",mssg);
	fseek(fp,offset,SEEK_SET);
	unsigned long long int result_length;
	result_length=fread(buf,packet_size,1,fp);
	if(result_length>0)
	{	
		int i,j = len;
		for(i = 0;i < packet_size;i++)
     	{
     		mssg[j++]=((char *)buf)[i];
     		//fprintf(stderr,"%c", ((char *)buf)[i]);
     	}
     	total_length=len+packet_size;
     		free(buf);
		//fprintf(stderr,"\n");	
     	return mssg;
	}	
	else
	{
		free(buf);
		fprintf(stderr,"unable to read should not happen or The file should already be transferred if this happened\n");	
		exit(0);
		return NULL;	
	}	
}

unsigned long long int transmit(char * mssg,int *sock_fd,unsigned long long int t_length)
{
	int i;
	//for(i = 0;i < 30;i++)
	//		fprintf(stderr,"%c", ((char *)mssg)[i]);
	//fprintf(stderr,"\n");		
	if(sendto(*sock_fd,mssg,t_length+1,0 ,sa,sa_len)<=0)	
	{
		while(sendto(*sock_fd,mssg,t_length+1,0 ,sa,sa_len)<=0);
		return 0;//timeout that means we had a Nack retransmit
	}
	else
	{
		//fprintf(stderr,"Successfully send waiting ACK\n");
		return 1;
	}		
} 

unsigned long long int  receive_protocol(int * sock_fd)
{
	char return_buf[50];
	return_buf[0]='\0';
	int i;
	unsigned long long int returned_ack;
	//fprintf(stderr,"waitng for rec\n");
	memset(return_buf,'\0',(sizeof(return_buf)));
	if ((recvfrom(*sock_fd, &return_buf, 50, 0 , NULL, NULL) <= 0 ))
	{
//		fprintf(stderr,"Unable to READ ACK\n");	
		return 0;
	}
	else
	{
		//for(i = 0;i < strlen(return_buf);++i)
		//	fprintf(stderr,"%c ", ((char *)return_buf)[i]);
		//transferre0.809383392d+=packet_size;
		returned_ack= atoll(return_buf);
		if(returned_ack==0)//all packets delivered
		{
			fprintf(stderr,"Receivecd Ack . Last packet succesfully delivered\n");
			fclose(fp);	
			exit(0);
		}
	}
	//fprintf(stderr,"  received ack %lld \n",returned_ack);
	return returned_ack;
}

void start_Protocol(int *sock_fd,char* filename, unsigned long long int bytesToTransfer)
{
	unsigned long long int transferred = 0;
	unsigned long long int window_size = 1;
	unsigned long long int packet_size = 10000;																				//840
	unsigned long long int current_seq=1;
	unsigned long long int last_ack;
	unsigned long long int last_packet_size;
	unsigned long long int total_ack;
	unsigned long long int count_receive=0;
	unsigned long long int window_id=0;
	unsigned long long int start_seq=0;
	unsigned long long int end_seq=0;
	unsigned long long int start[5000],end[5000];
	unsigned long long int offset[5000];
	unsigned long long int current_packet_size[5000];
	unsigned long long int total_message_size[5000];
	char * mssg_pointer[5000];
	int change_window=0;
	int all_delivered=0;
	int max_size;
	while(transferred<bytesToTransfer)
	{
		if(packet_size>(bytesToTransfer-transferred))// only one packet needs to be send
		{
			window_size=1;
			packet_size=bytesToTransfer-transferred;
			//fprintf(stderr,"Last packet \n");
		}
		int i;
		
		
		start[0]=transferred;
        end[0]=transferred+packet_size;
        window_id=0;
        change_window++;
		for(i=1;i<window_size;i++)
        {
        	start[i]=end[i-1];
        	if(start[i]+packet_size>=bytesToTransfer)
     		{
				packet_size=bytesToTransfer-start[i]; //this is the last packet
				last_packet_size=packet_size;				
				window_size=i+1;	//this will be the new max size of last window
				end[i]=start[i]+packet_size;
				//fprintf(stderr,"Last packet \n");
				break;			
			}		        		
			end[i]=start[i]+packet_size;
        	//fprintf(stderr,"transferred =%d start %d end %d\n",transferred,start[i],end[i]);
        }
        
        start_seq=current_seq;
        max_size=window_size;
		for(i=0;i<window_size;i++)
        {
			offset[i]=start[i];
        	current_packet_size[i]=end[i]-start[i];
        	if(offset[i]+current_packet_size[i]==bytesToTransfer)
        	{
				total_ack=current_seq-1;        			
				//fprintf(stderr,"Last packet \n");
        	 	end_seq=current_seq;
        	 	current_seq=0;
				all_delivered=1;
			}
        	mssg_pointer[i]=create_message(current_packet_size[i],offset[i],current_seq,window_size,window_id,change_window);
        	total_message_size[i]=total_length;
        	//current_seq!=0 && current_seq!=1217 && current_seq!=2100 && current_seq!=3500 && current_seq!=5000
        	//if(current_seq%4!=0)//drop every odd seq packet
        	//{	
				if(transmit(mssg_pointer[i],sock_fd,total_message_size[i])==1)
        		{	
        			if(current_seq!=0)
        				current_seq++;
        			transferred+=current_packet_size[i];
        		}
        		else// something bad happened , go for retransmission
        		{
        			fprintf(stderr,"Window retransmit unexpected behavior \n");
        	 		transferred-=current_packet_size[i];
        	 		i--;
        	 		//window_id/2;
        		}
        		window_id++;
        	//}
        	//else
        	//{ 
        	//	transferred+=current_packet_size[i];
        	//	if(current_seq!=0)
        	//		current_seq++;	//just for simulating forced drop
			//	window_id++;	//just for simulating forced drop
			//	fprintf(stderr,"droped %d\n",current_seq-1);
			//}	        	
		}
		
		//fprintf(stderr," round over window size was %d \n",window_size);
		/******************************Window has been sent look for ACK******************************************/
		//change_window=0;
		end_seq=start_seq;
		last_ack=receive_protocol(sock_fd);
		//fprintf(stderr,"last ack %lld  wanted %lld \n", last_ack,current_seq);
		if(last_ack!=current_seq || current_seq==0)//all packets did not got delivered
		{
			//fprintf(stderr,"congestion last ack %lld  wanted %lld \n", last_ack,current_seq);
			while(last_ack!=current_seq || current_seq==0)
			{
				for(i=0;i<window_size;i++)
        			{
        				//fprintf(stderr,"window_id  %s\n",mssg_pointer[i]);
        				//fprintf(stderr,"rt ");
        				if(offset[i]+current_packet_size[i]==bytesToTransfer)//last packet rt
							start_seq=0;
        				transmit(mssg_pointer[i],sock_fd,total_message_size[i]);
        				
             			}
             			//fprintf(stderr,"\n");
        			last_ack=receive_protocol(sock_fd);
        			start_seq=end_seq;
        			
			}
			
			if(window_size!=1)
			{
				window_size = window_size/2;
	
			}
			else
			{	
				window_size+=1;
			}
		}
		else 
		{	
			if(window_size<53)																//window_size
				window_size+=1;
			
		}
		//fprintf(stderr,"round over window_id %lld  %lld\n",window_size,change_window);	
		for(i=0;i<max_size;i++)
			free(mssg_pointer[i]);	
	}
}	


void reliablyTransfer(char* hostname, char* hostUDPport, char* filename, unsigned long long int bytesToTransfer)
{
	/***************SETUP UDP PROTOCOL*****************/
	//pthread_mutex_init(&file_mutex,NULL);
	int * sock_fd=malloc(sizeof(int));
	memset(&hints,0x00,sizeof(struct addrinfo));
	hints.ai_family= AF_INET;
	//hints.ai_flags=AI_PASSIVE;
	hints.ai_socktype=SOCK_DGRAM;
	hints.ai_protocol=IPPROTO_UDP;
	getaddrinfo(hostname,hostUDPport,&hints,&result);
	do
    	{	/* each of the returned IP address is tried*/
    		*sock_fd=socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    		if(*sock_fd>= 0)
    			break; /*success*/
    		else
    			{}	
	}while ((result=result->ai_next) != NULL);
	sa=malloc(result->ai_addrlen);
  	memcpy(sa, result->ai_addr, result->ai_addrlen);
  	sa_len=result->ai_addrlen;
	freeaddrinfo(ressave);
	
	struct timeval timeout;      
	timeout.tv_sec = 0;
	timeout.tv_usec = 100;
	int return_value=setsockopt (*sock_fd, SOL_SOCKET,SO_SNDTIMEO, &timeout,sizeof(timeout));
	if(return_value<0)
        	fprintf(stderr,"setsockopt failed \n");  
	else
		fprintf(stderr,"setsockopt enabled \n"); 
	
	      
	timeout.tv_sec = 0;
	timeout.tv_usec = 35000;//10 millisec timeout
	return_value=setsockopt (*sock_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout,sizeof(timeout));
	if(return_value<0)
        	fprintf(stderr,"setsockopt failed \n");  
	else
		fprintf(stderr,"setsockopt receiver end enabled \n"); 
	
	
	fp = fopen(filename,"rb");
    	start_Protocol(sock_fd,filename,bytesToTransfer);  
    
	
}

int main(int argc, char** argv)
{
	char* udpPort;
	unsigned long long int numBytes;
	
	if(argc != 5)
	{
		fprintf(stderr, "usage: %s receiver_hostname receiver_port filename_to_xfer bytes_to_xfer\n\n", argv[0]);
		exit(1);
	}
	udpPort =argv[2];
	filename=argv[3];
	numBytes = atoll(argv[4]);
	reliablyTransfer(argv[1], udpPort, argv[3], numBytes);
} 
