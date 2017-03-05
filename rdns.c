/**
Originally created for testing purpose.
Made by Kritpal Singh.
For any further query email me @ kritpal.sing@gmail.com
Created for Mediatek Ralink SDK

 */
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include <netdb.h>
char *ip_to_hostname(char *hostname);
int main(int argc,char** argv){

if(argc<2){
return ;
}
printf("Domain=%s\n",ip_to_hostname(argv[1]));
return 0;
}

char *ip_to_hostname(char *hostname)
{
  struct hostent *hent;
  struct in_addr addr;

  if(!inet_aton(hostname, &addr))
    return(NULL);

  if((hent = gethostbyaddr((char *)&(addr.s_addr), sizeof(addr.s_addr),
    AF_INET)))
  {
  char temp[500],*ptr;
  printf("got=%s\n",hent->h_name);
  strcpy(temp,hent->h_name);
if(ptr=strstr(temp,".lan")){
*ptr='\0';
}
  strcpy(hostname,temp);
  }else{
  return (NULL);
  }

  return(hostname);
}
