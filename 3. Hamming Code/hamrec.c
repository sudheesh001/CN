#include <stdio.h> //Standard Input Functions
#include <sys/types.h> //Named Pipes (FIFO) Types
#include <sys/stat.h> // Named Pipes (FIFO) Functions
#include <fcntl.h> //File Control Unix Functions
#include <unistd.h> //Unix Standard Functions
#include "hamcom.c" //Common function file for hamming code
int main()
{
        char *myfifo = "myfifo"; //FIFO File Path
        int fp; //Open Pipe Return Value
        unsigned char ch;//Original Character
        char str[]="0000000000000000";//This is not part of Hamming Algorithm just for debug
        unsigned short cw=0,cand=0; //Codeword and temp code operation variable
        int i,ldata,j=0,lcode,rbit=0;

        //Opening Named Pipe
        //open(<filepath>,<openingmode>);
        fp = open(myfifo,O_RDONLY);
        if(fp < 0)
        {
                perror("FIFO (Named Pipe) could not be opened for reading.\n");
                exit(-1);
        }
        else
                printf("FIFO (Named Pipe) Opened... Trying to read from\n");

        //Reading From Named Pipe
        //read(<open_pipe_return_value>,<message>,<msgsize>);
        if(read(fp,&cw,sizeof(cw)) < 0)
        {
                perror("Error reading from FIFO (Named Pipe)\n");
                exit(-2);
        }
        else printf("Message Received from FIFO (Named Pipe)\n");

        cw = cw | power(2,9);
        dec2bin(str,16,cw);
        printf("%d %s\n",cw,str);
        //HAMMING CODE LOGIC STARTS

        lcode = calcBit(cw);

        printf("%d\n",lcode);

        for(i=1;i<=lcode;i++)
        {
                if(i & (i-1))
                        continue;
                rbit++;
        }
        struct checkbit r[rbit];

        for(i=0;i<rbit;i++)
        {
                r[i].b = 0;
        }
        for(i=1;i<=lcode;i++)
        {
                for(j=0;j<rbit;j++){
                        if(i & power(2,j)){
                                r[j].b = r[j].b ^ chrpos(cw,i);
                        }
                }
        }

        for(i=0;i<rbit;i++)
        {
                printf(" %d",r[i].b);
                cand = cand | (r[i].b<<i);
        }

        if(cand){
                printf("\nError on %d position",cand);
                printf("\nCorrecting...");
                if(chrpos(cw,cand))
                        cw -= power(2,cand-1);
                else
                        cw += power(2,cand-1);
        }

        printf("\nCORRECT DATA : \n");
        dec2bin(str,16,cw);
        printf("%d %s\n",cw,str);
        cand = 0;
        for(i=1,j=0;i<=lcode;i++){
                if(i & (i-1)){
                        cand = cand | (chrpos(cw,i)<<j++);
                }
        }
        printf("%c\n",cand);

        //HAMMING CODE LOGIC ENDS
        if(close(fp) < 0)
        {
                perror("Error closing FIFO (Named Pipe)\b");
                exit(-3);
        }

        if(unlink(myfifo) < 0)
        {
                perror("Error deleting FIFO (Named Pipe)\b");
                exit(-4);
        }
        return 0;
}