//====================================================== file = weblite.c =====
//=  A super light-weight secure HTTP server                                  =
//=   - Uses threads to allow for parallel connections                        =
//=============================================================================
//=  Notes:                                                                   =
//=    1) Compiles for Windows (using Winsock and Windows threads) and Unix   =
//=       (using BSD sockets and POSIX threads)                               =
//=    2) Serves HTML, text, and GIF only.                                    =
//=    3) Is not secure - when weblite is running any file on the machine     =
//=       (that weblite is running on) could be accessed.                     =
//=    4) Sometimes the browser drops a connection when doing a refresh.      =
//=       This is handled by checking the recv() return code in the           =
//=       function that handles GETs.  This is only seen when using           =
//=       Explorer.                                                           =
//=    5) The 404 HTML message does not always display in Explorer.           =
//=    6) Ignore the compile-time warnings regarding unreachable code         =
//=       in main().                                                          =
//=---------------------------------------------------------------------------=
//=  Execution notes:                                                         =
//=   1) Execute this program in the directory which will be the root for     =
//=      all file references (i.e., the directory that is considered at       =
//=      "public.html").                                                      =
//=   2) Open a Web browser and surf http://xxx.xxx.xxx.xxx:8080/yyy where    =
//=      xxx.xxx.xxx.xxx is the IP address or hostname of the machine that    =
//=      weblite is executing on and yyy is the requested object.             =
//=   3) The only non-error output (to stdout) from weblite is a message      =
//=      with the name of the file currently being sent.                      =
//=---------------------------------------------------------------------------=
//=  Build: Windows: Borland: bcc32 -WM weblite.c                             =
//=                  Visual C command line: cl /MT weblite.c wsock32.lib      =
//=                  MinGW: gcc weblite.c -lws2_32 -o weblite                 =
//=         Unix: gcc weblite.c -lpthread -lnsl -lsocket -o weblite           =
//=   Be sure to change the #define in line 48 appropriately                  =
//=---------------------------------------------------------------------------=
//=  Execute: weblite                                                         =
//=---------------------------------------------------------------------------=
//=  History:  KJC (10/08/02) - Genesis                                       =
//=            KJC (09/11/05) - Fixed "GET \./../" security hole              =
//=            KJC (01/29/06) - Add BSD as conditional compile (thanks to     =
//=                             James Poag for POSIX threads howto)           =
//=            KJC (12/06/06) - Fixed pthread call (thanks to Nicholas        =
//=                             Paltzer for finding and fixing the problem)   =
//=            KJC (09/09/09) - Changed port to 8080 and fixed gcc build      =
//=            KJC (09/07/10) - Updated build instructions for minGW          =
//=            KJC (08/20/11) - Updated build instructions for gcc            =
//=            KJC (09/17/13) - Removed broken security check, is non-secure  =
//=============================================================================
#define  WIN                // WIN for Winsock and BSD for BSD sockets (Unix)

//----- Include files ---------------------------------------------------------
#include <stdio.h>          // Needed for printf()
#include <stdlib.h>         // Needed for exit()
#include <string.h>         // Needed for memcpy() and strcpy()
#include <fcntl.h>          // Needed for file i/o stuff
  #include <pthread.h>      // Needed for pthread_create() and pthread_exit()
  #include <sys/stat.h>     // Needed for file i/o constants
  #include <sys/types.h>    // Needed for sockets stuff
  #include <netinet/in.h>   // Needed for sockets stuff
  #include <sys/socket.h>   // Needed for sockets stuff
  #include <arpa/inet.h>    // Needed for sockets stuff
  #include <fcntl.h>        // Needed for sockets stuff
  #include <netdb.h>        // Needed for sockets stuff

//----- HTTP response messages ----------------------------------------------
#define OK_IMAGE  "HTTP/1.0 200 OK\r\nContent-Type:image/gif\r\n\r\n"
#define OK_TEXT   "HTTP/1.0 200 OK\r\nContent-Type:text/html\r\n\r\n"
#define NOTOK_404 "HTTP/1.0 404 Not Found\r\nContent-Type:text/html\r\n\r\n"
#define MESS_404  "<html><body><h1>FILE NOT FOUND</h1></body></html>"

//----- Defines -------------------------------------------------------------
#define  PORT_NUM            8080     // Port number for Web server
#define  BUF_SIZE            4096     // Buffer size (big enough for a GET)

//----- Function prototypes -------------------------------------------------
#ifdef WIN
  void handle_get(void *in_arg);      // Windows thread function to handle GET
#endif
#ifdef BSD
  void *handle_get(void *in_arg);     // POSIX thread function to handle GET
#endif

//===== Main program ========================================================
int main()
{
#ifdef WIN
  WORD wVersionRequested = MAKEWORD(1,1);    // Stuff for WSA functions
  WSADATA wsaData;                           // Stuff for WSA functions
#endif
  int                  server_s;             // Server socket descriptor
  struct sockaddr_in   server_addr;          // Server Internet address
  int                  client_s;             // Client socket descriptor
  struct sockaddr_in   client_addr;          // Client Internet address
  struct in_addr       client_ip_addr;       // Client IP address
#ifdef WIN
  int                  addr_len;             // Internet address length
#endif
#ifdef BSD
  socklen_t            addr_len;             // Internet address length
  pthread_t            thread_id;            // Thread ID
#endif
  int                  retcode;              // Return code

#ifdef WIN
  // This stuff initializes winsock
  WSAStartup(wVersionRequested, &wsaData);
#endif

  // Create a server socket
  server_s = socket(AF_INET, SOCK_STREAM, 0);
  if (server_s < 0)
  {
    printf("*** ERROR - socket() failed \n");
    exit(-1);
  }

  // Fill-in server (my) address information and bind the socket
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT_NUM);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  retcode = bind(server_s, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (retcode < 0)
  {
    printf("*** ERROR - bind() failed \n");
    exit(-1);
  }

  // Set-up the listen
  listen(server_s, 100);

  // Main loop to accept connections and then spin-off thread to handle the GET
  printf(">>> weblite is running on port %d <<< \n", PORT_NUM);
  while(1)
  {
    addr_len = sizeof(client_addr);
    client_s = accept(server_s, (struct sockaddr *)&client_addr, &addr_len);
    if (client_s == -1)
    {
      printf("ERROR - Unable to create a socket \n");
      exit(1);
    }

#ifdef WIN
    if (_beginthread(handle_get, 4096, (void *)client_s) < 0)
#endif
#ifdef BSD
    if (pthread_create(&thread_id, NULL, handle_get, (void *)client_s) != 0)
#endif
    {
      printf("ERROR - Unable to create a thread to handle the GET \n");
      exit(1);
    }
  }

  return(0);
}

//===========================================================================
//=  This is is the thread function to handle the GET                       =
//===========================================================================
#ifdef WIN
void handle_get(void *in_arg)
#endif
#ifdef BSD
void *handle_get(void *in_arg)
#endif
{
  int            client_s;             // Client socket descriptor
  char           in_buf[BUF_SIZE];     // Input buffer for GET request
  char           out_buf[BUF_SIZE];    // Output buffer for HTML response
  int            fh;                   // File handle
  int            buf_len;              // Buffer length for file reads
  char           command[BUF_SIZE];    // Command buffer
  char           file_name[BUF_SIZE];  // File name buffer
  int            retcode;              // Return code

  // Set client_s to in_arg
  client_s = (int) in_arg;

  // Receive the (presumed) GET request from the Web browser
  retcode = recv(client_s, in_buf, BUF_SIZE, 0);

  // If the recv() return code is bad then bail-out (see note #3)
  if (retcode <= 0)
  {
    printf("ERROR - Receive failed --- probably due to dropped connection \n");
#ifdef WIN
    closesocket(client_s);
    _endthread();
#endif
#ifdef BSD
    close(client_s);
    pthread_exit(NULL);
#endif
  }

  // Parse out the command from the (presumed) GET request and filename
  sscanf(in_buf, "%s %s \n", command, file_name);

  // Check if command really is a GET, if not then bail-out
  if (strcmp(command, "GET") != 0)
  {
    printf("ERROR - Not a GET --- received command = '%s' \n", command);
#ifdef WIN
    closesocket(client_s);
    _endthread();
#endif
#ifdef BSD
    close(client_s);
    pthread_exit(NULL);
#endif
  }

  // It must be a GET... open the requested file
  //  - Start at 2nd char to get rid of leading "\"
#ifdef WIN
  fh = open(&file_name[1], O_RDONLY | O_BINARY, S_IREAD | S_IWRITE);
#endif
#ifdef BSD
  fh = open(&file_name[1], O_RDONLY, S_IREAD | S_IWRITE);
#endif

  // If file does not exist, then return a 404 and bail-out
  if (fh == -1)
  {
    printf("File '%s' not found --- sending an HTTP 404 \n", &file_name[1]);
    strcpy(out_buf, NOTOK_404);
    send(client_s, out_buf, strlen(out_buf), 0);
    strcpy(out_buf, MESS_404);
    send(client_s, out_buf, strlen(out_buf), 0);
#ifdef WIN
    closesocket(client_s);
    _endthread();
#endif
#ifdef BSD
    close(client_s);
    pthread_exit(NULL);
#endif
  }

  // Generate and send the response
  printf("Sending file '%s' \n", &file_name[1]);
  if (strstr(file_name, ".gif") != NULL)
    strcpy(out_buf, OK_IMAGE);
  else
    strcpy(out_buf, OK_TEXT);
  send(client_s, out_buf, strlen(out_buf), 0);
  while(1)
  {
    buf_len = read(fh, out_buf, BUF_SIZE);
    if (buf_len == 0) break;
    send(client_s, out_buf, buf_len, 0);
  }

  // Close the file, close the client socket, and end the thread
  close(fh);
#ifdef WIN
    closesocket(client_s);
    _endthread();
#endif
#ifdef BSD
    close(client_s);
    pthread_exit(NULL);
#endif
}