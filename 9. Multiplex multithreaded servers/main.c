#include "io_multiplex_server.h"

/* enum definitions */
typedef enum
{
	EXIT_CODE_SUCCESS = 0,
	EXIT_CODE_ERROR
} EXIT_CODE;

/* function definitions */
int main(void)
{
	/* local variables definitions */
	int i = 0;
	int socket_fd = -1;
	listen_addr addr_array[] = {{"\0", 9001}, {"\0", 9002}, {"\0", 9003}};
	
	/* code body */
	for (i = 0; i < (sizeof(addr_array) / sizeof(addr_array[0])); i++)
	{
		socket_fd = create_listen_socket(&(addr_array[i]));
		if (-1 == socket_fd)
		{
			goto MAIN_ERR_END;
		}
		else
		{
			add_fd_into_monitor_event_loop(socket_fd, EVENT_TYPE_READ, accept_new_socket, (void*)socket_fd);
		}
	}

	while (1)
	{
		run_monitor_event_loop();
	}
	
	return EXIT_CODE_SUCCESS;
	
MAIN_ERR_END:
	return EXIT_CODE_ERROR;
}
