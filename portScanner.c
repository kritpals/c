/*
*Originally created for testing purpose.
*Made by Kritpal Singh.
*For any further query email me @ kritpal.sing@gmail.com
 Originally created for Openwrt SDK 
    TCP basic Port scanner code in c
*/
#include<stdio.h>
#include<sys/socket.h>
#include<errno.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>
#include <fcntl.h>

int main(int argc , char **argv)
{
    struct hostent *host;
    int err, i , sock ,start , end;
    char hostname[100];
    struct sockaddr_in sa;
   unsigned long long  sTime=0,eTime=0;
   
char sockA[8][10]={"21","22","23","53","80","515","631","9100"};
    //Get the hostname to scan
    if(argc >= 2){
    //printf("Enter hostname or IP : ");
    sprintf(hostname,"%s",argv[1]);
    }else{
    return;
    }
     start=1;

 end=1024;
    //Initialise the sockaddr_in structure
    strncpy((char*)&sa , "" , sizeof sa);
    sa.sin_family = AF_INET;
     
    //direct ip address, use it
    if(isdigit(hostname[0]))
    {
        printf("Doing inet_addr...");
        sa.sin_addr.s_addr = inet_addr(hostname);
        printf("Done\n");
    }
    //Resolve hostname to ip address
    else if( (host = gethostbyname(hostname)) != 0)
    {
        printf("Doing gethostbyname...");
        strncpy((char*)&sa.sin_addr , (char*)host->h_addr , sizeof sa.sin_addr);
        printf("Done\n");
    }
    else
    {
        herror(hostname);
        exit(2);
    }
     
    //Start the port scan loop
    printf("Starting the portscan loop : \n");
    for( i = start ; i <=end ; i++) 
    {
        //Fill in the port number
        sa.sin_port = htons(i);//atoi(sockA[i]));//515,631,9100
        //Create a socket of type internet
        sock = socket(AF_INET , SOCK_STREAM , 0);
       //printf("Checking on port=%d\n",i);
       
        //Check whether socket created fine or not
        if(sock < 0) 
        {
            perror("\nSocket");
           // exit(1);
        }
        //Connect using that socket and sockaddr structure
        sTime=time(NULL);
        err = connect(sock , (struct sockaddr*)&sa , sizeof sa);
         
        //not connected
        if( err < 0 )
        {
           // printf("%s %-5d %s\n" , hostname , atoi(sockA[i]), strerror(errno));
           // fflush(stdout);
        }
        //connected
        else
        {
            printf("%-5d open\n",  i);
        }
        eTime=time(NULL);
        //printf("Time Start =%llu end=%llu diff=%llu\n",sTime,eTime,eTime-sTime);
        if((eTime - sTime ) > 60){
        printf("Bad device cant do port scan\n");
        return;
        }
        close(sock);
    }
     
    printf("\r");
    fflush(stdout);
    return(0);
} 
