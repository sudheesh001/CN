#include<stdio.h>
int power(int n,int m)
{
        int i=0;
        int sum=1;
        for(i=1;i<=m;i++)
                sum *= n;
        return sum;
}
void dec2bin(char bits[], int l, int a)
{
        int i;
        for(i=l-1;i>=0;i--)
        {
                bits[i] = (a & 1) + '0';
                a >>= 1;
        }
}
unsigned char chrpos(int ch,int pos)
{
        return (ch & power(2,pos-1))>>(pos-1);
}
int calcBit(unsigned long c)
{
        int l=0;
        while(c>0)
        {
                l++;
                c>>=1;
        }
        return l;

}
struct checkbit
{
        unsigned int b:1;
};