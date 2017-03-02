/*
*Originally created for testing purpose.
*Made by Kritpal Singh.
*For any further query email me @ kritpal.sing@gmail.com
 Originally created for Mediatek SDK 
Utility to send upnp search command and get UPNP info for a device using SSDP protocol .
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include<sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
       #include <netinet/ip.h>
#include <sys/ioctl.h>

int getIp() {

    int checkip = 0;
    int sd, len;
    int k = 0;
    struct sockaddr_in serv;
    char m_searchbuff[1028] = "";
    sd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sd == -1) {
    perror("Socket:");
       // sending.number = 0;
       return 0;
    } else {
        serv.sin_family = PF_INET;
        serv.sin_port = htons(1900);
        serv.sin_addr.s_addr = inet_addr("239.255.255.250");
        struct timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;
       // sending.number = 0;
        char prevIP[500] = "";
        
        const char device[] = "br0"; //For Almod+
        setsockopt(sd, SOL_SOCKET, SO_BINDTODEVICE, device, sizeof (device));
        if (setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof (tv)) < 0) {
            perror("Error");
            close(sd);
            return -1;
        }
          system("echo \"0\" > /sys/devices/virtual/net/br0/bridge/multicast_snooping");
        //system("echo \"0\" > /sys/devices/virtual/net/br-lan/bridge/multicast_snooping");
        len = sizeof (serv);
        sprintf(m_searchbuff, "%s", "M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nMAN: \"ssdp:discover\"\r\nMX: 4\r\nST: ssdp:all\r\n\r\n");
        int i = sendto(sd, &m_searchbuff, sizeof (m_searchbuff), 0, (struct sockaddr *) &serv, (socklen_t) len);
        if (i < 0) {
            perror("sendto");
            close(sd);
            //sending.number=0;
            return 0;
        }
        //printf("sent : %s\n", m_searchbuff);
        bzero(m_searchbuff, 128);
       // bool result = false;
        int j = 0;
        while (recvfrom(sd, &m_searchbuff, sizeof (m_searchbuff), 0, (struct sockaddr *) &serv, (socklen_t*) & len) > 0) {
            char *str, temp[500];
            int i = 0;
           
            printf("upnp data=%s\n",m_searchbuff );
        }
        perror("while error");
    }


    
    
   // system("echo \"1\" > /sys/devices/virtual/net/br-lan/bridge/multicast_snooping");
      system("echo \"1\" > /sys/devices/virtual/net/br0/bridge/multicast_snooping");

    close(sd);
    return 0;
}


int main()
{
getIp();
return 0;
}
