#include<iostream>
#include<stdio.h>
using namespace std;
int main()
{
  FILE *fp=popen("./temp.out","r");
  char buf[60];
  fscanf(fp,"%s",buf);
  cout<<buf<<endl;
  return 0;
}


