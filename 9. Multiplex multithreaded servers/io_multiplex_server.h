#ifndef __IO_MULTIPLEX_SERVER_H__
#define __IO_MULTIPLEX_SERVER_H__

/* include system header files */
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <fcntl.h>
#include <pthread.h>


/* marco definitions */
#define IP_ADDR_LEN    (16)

/* enum definitions */
typedef enum
{
	EVENT_TYPE_NONE = 0x00,
	EVENT_TYPE_READ = 0x01,
	EVENT_TYPE_WRITE = 0x02
} EVENT_TYPE;

/* structure or function pointer definitions */
typedef void (*process_event_func)(EVENT_TYPE event, void *p_data);

typedef struct
{
	char ip_addr[IP_ADDR_LEN];
	int port;
} listen_addr;

/* function declarations */
extern int init_monitor_event_loop(void);
extern int add_fd_into_monitor_event_loop(int fd, EVENT_TYPE event, process_event_func process_func, void *p_data);
extern void run_monitor_event_loop(void);

extern int create_listen_socket(listen_addr *p_listen_addr);
extern void accept_new_socket(EVENT_TYPE event, void *p_data);

#endif
