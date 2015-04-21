#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	printf("Hello world!\n");
	printf("from test\n");
	// if(argc>1){
		printf("Yeyy argc = %d\n",argc );
		char cmdbuf[256];
		char tmp[10];
		printf("Write exec name\n");
		scanf("%s",tmp);
 		snprintf(cmdbuf, sizeof(cmdbuf), 
          "gnome-terminal -x sh -c '%s ; cat'", tmp);
 		printf("%s\n",cmdbuf );
 		int err = system(cmdbuf);
 		if(err){
 			printf("Error: %s\n", cmdbuf);
 		}
	// }		
	return 0;
}