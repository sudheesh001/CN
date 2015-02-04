#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char const *argv[])
{
	printf("from test\n");
	if(argc>1){
		printf("Yeyy argc = %d\n",argc );
		char cmdbuf[256];
 		snprintf(cmdbuf, sizeof(cmdbuf), 
          "gnome-terminal -x sh -c '%s ; cat'", argv[1]);
 		printf("%s\n",cmdbuf );
 		int err = system(cmdbuf);
 		if(err){
 			printf("Error: %s\n", cmdbuf);
 		}
	}
	return 0;
}