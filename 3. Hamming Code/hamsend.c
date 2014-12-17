#include <stdio.h> //Standard Input Functions
#include <sys/types.h> //Named Pipes (FIFO) Types
#include <sys/stat.h> // Named Pipes (FIFO) Functions
#include <fcntl.h> //File Control Unix Functions
#include <unistd.h> //Unix Standard Functions
#include "hamcom.c"
int main()
{
        char *myfifo = "myfifo"; //FIFO File Path
        int fp; //Open Pipe Return Value
        unsigned char ch;//Original Character
        char str[]="0000000000000000";//This is not part of Hamming Algorithm just for debug
        unsigned short cw=0,cand; //Codeword and temp code operation variable
        int i,ldata,j=0,lcode,rbit=1; //i,j-> Loop variable, ldata-> size of databits
        // lcode-> size of codeword bits, rbit->size of check bits

        printf("Creating Pipe...Waiting for receiver for Process...\n");
        //Creating Named Pipe
        //mkfifo(<filepath>,<permission>)
        if(mkfifo(myfifo,0666) < 0)
        {
                perror("FIFO (Named Pipe) could not be created.\n");
                exit(-1);
        }
        else
                printf("FIFO (Named Pipe) Created...\n");

        //Opening Named Pipe
        //open(<filepath>,<openingmode>);
        fp = open(myfifo,O_WRONLY);
        if(fp < 0)
        {
                perror("FIFO (Named Pipe) could not be opened.\n");
                exit(-2);
        }
        else
                printf("FIFO (Named Pipe) Opened...\n");

        //HAMMING CODE LOGIC STARTS
        printf("Enter Character : ");
        scanf("%c",&ch);

        ldata = calcBit(ch);//Number of bits in original data

        //Logic of finding number of check bits
        while(power(2,rbit)<(ldata+rbit+1))
        {
                rbit++;
        }

        //Length of codeword = length of data + length of check bits
        lcode = ldata + rbit;

        //defining all checkbits
        struct checkbit  r[rbit];
        for(i=0;i<rbit;i++)
        {
                r[i].b = 0;
        }

        //STEP-1 Filling all data bits in codeword
        for(i=3;i<=lcode;i++)
        {
                if(i&(i-1)){ //Condition of Power of 2
                        cand = ch & power(2,j++);
                        cw = cw | (cand<<(i-j));
                }
        }

        //codeword after filling data bits
        dec2bin(str,16,cw);
        printf("%d %s\n",cw,str);

        //STEP-2 calculating all checkbits (parity bits)
        for(i=3;i<=lcode;i++)
        {
                if(i & (i-1)){ //condition for Power of 2
                        for(j=0;j<rbit;j++){
                                if(i & power(2,j)){ //condition for including data bit in parity or not (1,3,5,7,9... include in 1 because all has 1 in last bit)
                                        r[j].b = r[j].b ^ chrpos(cw,i);
                                }
                        }
                }
        }

        //STEP-3 Fill check bits in code word
        for(i=1,j=0;i<=lcode;i++)
        {
                if(!(i&(i-1)))//Conditio nfor only power of 2
                {
                        cw = cw | (r[j++].b << (power(2,j)-1));
                }
        }

        //Final Code word
        dec2bin(str,16,cw);
        printf("\n%d %s\n",cw,str);

        //HAMMING CODE LOGIC ENDS

        //Writing to Named Pipe
        //write(<open_pipe_return_value>,<message>,<msgsize>);
        if(write(fp,&cw,sizeof(cw)) < 0)
        {
                perror("Error writing to FIFO (Named Pipe)\n");
                exit(-3);
        }
        else printf("Message has been sent to FIFO (Named Pipe)\n");

        //Closing Named Pipe
        //close(<open_pipe_return_value>);
        if(close(fp) < 0)
        {
                perror("Error closing FIFO (Named Pipe)\n");
                exit(-4);
        }
        return 0;
}