#include "io_multiplex_server.h"

/* marco definitions */
#define LISTEN_SOCKET_BACKLOG_NUM    (512)
#define MAX_FD_NUM                   (2048)
#define RECV_BUF_SIZE                (4096)



/* structure or function pointer definitions */
typedef struct
{
	EVENT_TYPE event;
	process_event_func process_func;
	void *p_data;
} monitor_fd_info;

typedef struct
{
	int max_fd;
	fd_set read_fds;
	fd_set write_fds;
	fd_set backup_read_fds;
	fd_set backup_write_fds;
	monitor_fd_info fd_info[MAX_FD_NUM];
} monitor_event_loop;

/* variable definitions */
static monitor_event_loop g_monitor_event_loop;

/* function declarations */
static void *process_recv_data_thread(void *p_data);

/* function definitions */
int init_monitor_event_loop(void)
{
	/* code body */
	memset(&g_monitor_event_loop, 0, sizeof(g_monitor_event_loop));
	return 0;
}


int add_fd_into_monitor_event_loop(int fd, EVENT_TYPE event, process_event_func process_func, void *p_data)
{
	/* code body */
	if (fd >= MAX_FD_NUM)
	{
		return (-1);
	}
	else
	{
		if (fd > g_monitor_event_loop.max_fd)
		{
			g_monitor_event_loop.max_fd = fd;
		}

		if (event & EVENT_TYPE_READ)
		{
			FD_SET(fd, &(g_monitor_event_loop.read_fds));
		}

		if (event & EVENT_TYPE_WRITE)
		{
			FD_SET(fd, &(g_monitor_event_loop.write_fds));
		}

		g_monitor_event_loop.fd_info[fd].event = event;
		g_monitor_event_loop.fd_info[fd].process_func = process_func;
		g_monitor_event_loop.fd_info[fd].p_data = p_data;
		printf("Monitoring \n");

		return 0;
	}
}


void run_monitor_event_loop(void)
{
	/* local variables definitions */
	int j = 0;
	int ret_val = 0;
	EVENT_TYPE event =0;
	
	/* code body */
	memcpy(&(g_monitor_event_loop.backup_read_fds), &(g_monitor_event_loop.read_fds), sizeof(g_monitor_event_loop.read_fds));
	memcpy(&(g_monitor_event_loop.backup_write_fds), &(g_monitor_event_loop.write_fds), sizeof(g_monitor_event_loop.write_fds));

	ret_val = select(
				(g_monitor_event_loop.max_fd + 1), 
				&(g_monitor_event_loop.backup_read_fds), 
				&(g_monitor_event_loop.backup_write_fds),
				NULL,
				NULL);
	if (ret_val > 0)
	{
		for (j = 0; j <= g_monitor_event_loop.max_fd; j++)
		{
			event = 0;
			if (FD_ISSET(j, &(g_monitor_event_loop.backup_read_fds)))
			{
				event = event | EVENT_TYPE_READ;
			}
			if (FD_ISSET(j, &(g_monitor_event_loop.backup_write_fds)))
			{
				event = event | EVENT_TYPE_READ;
			}
			if (EVENT_TYPE_NONE != event)
			{
				g_monitor_event_loop.fd_info[j].process_func(event, g_monitor_event_loop.fd_info[j].p_data);
			}
		}
	}
}


int create_listen_socket(listen_addr *p_listen_addr)
{
	/* local variables definitions */
	int socket_fd = -1;
	int resue_addr = 1;
	struct sockaddr_in addr = {0};

	/* code body */
	socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (-1 == socket_fd)
	{
		goto CREATE_LISTEN_SOCKET_ERR_END;
	}

	if (-1 == setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &resue_addr, sizeof(resue_addr)))
	{
		goto CREATE_LISTEN_SOCKET_ERR_END;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(p_listen_addr->port);
	printf("%d\n", addr.sin_port);
	if (0 != p_listen_addr->ip_addr[0])
	{
		if (inet_aton(p_listen_addr->ip_addr, &(addr.sin_addr)) == 0)
		{
			goto CREATE_LISTEN_SOCKET_ERR_END;
		}
	}
	else
	{
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
	}

	if (-1 == bind(socket_fd, (struct sockaddr*)(&addr), sizeof(addr)))
	{
		goto CREATE_LISTEN_SOCKET_ERR_END;
	}

	if (-1 == listen(socket_fd, LISTEN_SOCKET_BACKLOG_NUM))
	{
		goto CREATE_LISTEN_SOCKET_ERR_END;
	}

	return socket_fd;
	
CREATE_LISTEN_SOCKET_ERR_END:
	return -1;
}


void accept_new_socket(EVENT_TYPE event, void *p_data)
{
	/* local variables definitions */
	int *p_new_sock_fd = NULL;
	pthread_t thread_id =0;
	
	/* code body */
	if (event & EVENT_TYPE_READ)
	{
		p_new_sock_fd = malloc(sizeof(int));
		if (NULL != p_new_sock_fd)
		{
			*p_new_sock_fd = accept((int)p_data, NULL, NULL);
			if (-1 != *p_new_sock_fd)
			{
				if (0 == pthread_create(&thread_id, NULL, process_recv_data_thread, p_new_sock_fd))
				{
					pthread_detach(thread_id);
				}
			}
		}
	}
}


static void *process_recv_data_thread(void *p_data)
{
	/* local variables definitions */
	int recv_num = 0;
	int new_socket_fd = *((int*)p_data);
	char recv_buff[RECV_BUF_SIZE] = {0};
	printf("%s\n", recv_buff);
	/* code body */
	while ((recv_num = recv(new_socket_fd, recv_buff, sizeof(recv_buff), 0)) > 0)
	{
		printf("%s\n", recv_buff);
	}
	close(new_socket_fd);
	free(p_data);
}
