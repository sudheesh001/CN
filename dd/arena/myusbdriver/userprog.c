#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define SIG_TEST 44

void receiveData(int n, siginfo_t *info, void *unused) {
	printf("received value %i\n", info->si_int);
	char cmdbuf[256];
 	snprintf(cmdbuf, sizeof(cmdbuf), 
    "gnome-terminal -x sh -c '%s ; cat'", "./first");
 	system(cmdbuf);
}

int main ( int argc, char **argv )
{
	int configfd;
	char buf[10];
	
	struct sigaction sig;
	sig.sa_sigaction = receiveData;
	sig.sa_flags = SA_SIGINFO;
	sigaction(SIG_TEST, &sig, NULL);

	configfd = open("/sys/kernel/debug/signalconfpid", O_WRONLY);
	if(configfd < 0) {
		perror("open");
		return -1;
	}
	sprintf(buf, "%i", getpid());
	if (write(configfd, buf, strlen(buf) + 1) < 0) {
		perror("write"); 
		return -1;
	}
	
	return 0;
}