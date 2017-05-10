/* 
 * File:   Check_Internet.c
 * Author: Kritpal
 *
 * Created on 27 April, 2017, 10:16 AM
 * gcc Check_Internet.c -w  -o tcp
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
int sockfd, dns_fd;
#define MAX_LINE 500
#define TCP_NODELAY 1
#define T_A 1 //Ipv4 address

//Function Prototypes
int ngethostbyname(unsigned char *host, int query_type, char * ip, int timeout, char *dns_addr);
void ChangetoDnsNameFormat(unsigned char*, unsigned char*);
unsigned char* ReadName(unsigned char*, unsigned char*, int*);
void get_dns_servers();

//DNS header structure

struct DNS_HEADER {
    unsigned short id; // identification number

    unsigned char rd : 1; // recursion desired
    unsigned char tc : 1; // truncated message
    unsigned char aa : 1; // authoritive answer
    unsigned char opcode : 4; // purpose of message
    unsigned char qr : 1; // query/response flag

    unsigned char rcode : 4; // response code
    unsigned char cd : 1; // checking disabled
    unsigned char ad : 1; // authenticated data
    unsigned char z : 1; // its z! reserved
    unsigned char ra : 1; // recursion available

    unsigned short q_count; // number of question entries
    unsigned short ans_count; // number of answer entries
    unsigned short auth_count; // number of authority entries
    unsigned short add_count; // number of resource entries
};

//Constant sized fields of query structure

struct QUESTION {
    unsigned short qtype;
    unsigned short qclass;
};

//Constant sized fields of the resource record structure
#pragma pack(push, 1)

struct R_DATA {
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};
#pragma pack(pop)

struct RES_RECORD {
    unsigned char *name;
    struct R_DATA *resource;
    unsigned char *rdata;
};
char dns_servers[2][50];

u_char* ReadName(unsigned char* reader, unsigned char* buffer, int* count) {
    unsigned char *name;
    unsigned int p = 0, jumped = 0, offset;
    int i, j;

    *count = 1;

    while (*reader != 0) {
        if (*reader >= 192) {
            offset = (*reader)*256 + *(reader + 1) - 49152; //49152 = 11000000 00000000 ;)
            reader = buffer + offset - 1;
            jumped = 1; //we have jumped to another location so counting wont go up!
        } else {
            //name[p++] = *reader;
        }

        reader = reader + 1;

        if (jumped == 0) {
            *count = *count + 1; //if we havent jumped to another location then we can count up
        }
    }

    if (jumped == 1) {
        *count = *count + 1; //number of steps we actually moved forward in the packet
    }
    return "";
}

/*
 * Perform a DNS query by sending a packet
 * */


void get_dns_servers() {
    FILE *fp;
    char line[200], *p;
    if ((fp = fopen("/etc/resolv.conf", "r")) == NULL) {
        // printf("Resolve file not present\n");
        sprintf(dns_servers[0], "8.8.8.8");
        sprintf(dns_servers[1], "208.67.220.220");

    } else {
        int i = 0;
        while (fgets(line, 199, fp)) {
            if (line[0] == '#') {
                continue;
            }
            if (strncmp(line, "nameserver", 10) == 0) {
                p = strtok(line, " ");
                p = strtok(NULL, " ");
                strcpy(dns_servers[i++], p);

            }
            if (i == 2) {
                break;
            }
        }
        fclose(fp);
    }

}

int ngethostbyname(unsigned char *host, int query_type, char * ip, int timeout, char *dns_addr) {
    unsigned char buf[9126], *qname, *reader;
    int i, j, stop;
    struct sockaddr_in a;
    struct RES_RECORD answers[20]; //the replies from the DNS server
    struct sockaddr_in dest;

    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;
    struct timeval tm;
    tm.tv_sec = timeout;
    tm.tv_usec = 0;
    dns_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //UDP packet for DNS queries
    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr(dns_addr); //dns servers
    //Set the DNS structure to standard queries
    dns = (struct DNS_HEADER *) &buf;
    dns->id = (unsigned short) htons(getpid());
    dns->qr = 0; //This is a query
    dns->opcode = 0; //This is a standard query
    dns->aa = 0; //Not Authoritative
    dns->tc = 0; //This message is not truncated
    dns->rd = 1; //Recursion Desired
    dns->ra = 0; //Recursion not available! hey we dont have it (lol)
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1); //we have only 1 question
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;
    //point to the query portion
    qname = (unsigned char*) &buf[sizeof (struct DNS_HEADER)];

    ChangetoDnsNameFormat(qname, host);
    qinfo = (struct QUESTION*) &buf[sizeof (struct DNS_HEADER) + (strlen((const char*) qname) + 1)]; //fill it

    qinfo->qtype = htons(query_type); //type of the query , A , MX , CNAME , NS etc
    qinfo->qclass = htons(1); //its internet (lol)

    //    printf("\nSending Packet...");
    setsockopt(dns_fd, SOL_SOCKET, SO_RCVTIMEO, (char *) &tm, sizeof (tm));
    if (sendto(dns_fd, (char*) buf, sizeof (struct DNS_HEADER) + (strlen((const char*) qname) + 1) + sizeof (struct QUESTION), 0, (struct sockaddr*) &dest, sizeof (dest)) < 0) {
        //        perror("sendto failed");
        return 0;
    }
    i = sizeof dest;
    int size = 0;
    if ((size = recvfrom(dns_fd, (char*) buf, 65536, 0, (struct sockaddr*) &dest, (socklen_t*) & i)) < 0) {
        //        perror("recvfrom failed");
        return 0;
    }

    dns = (struct DNS_HEADER*) buf;

    //move ahead of the dns header and the query field
    reader = &buf[sizeof (struct DNS_HEADER) + (strlen((const char*) qname) + 1) + sizeof (struct QUESTION)];

    stop = 0;

    for (i = 0; i < ntohs(dns->ans_count); i++) {
        ReadName(reader, buf, &stop);
        reader = reader + stop;
        answers[i].resource = (struct R_DATA*) (reader);
        reader = reader + sizeof (struct R_DATA);
        if (ntohs(answers[i].resource->type) == 1) //if its an ipv4 address
        {
            int ip1;
            char *p = &ip1;
            for (j = 0; j < ntohs(answers[i].resource->data_len); j++) {
                p[j] = reader[j];
            }

            a.sin_addr.s_addr = (ip1); //working without ntohl
            //printf("IP : %s\n", inet_ntoa(a.sin_addr));
            sprintf(ip, "%s", inet_ntoa(a.sin_addr));
            reader = reader + ntohs(answers[i].resource->data_len);
            break;
        } else {
            ReadName(reader, buf, &stop);
            reader = reader + stop;
        }
    }

    return 1;
}

void ChangetoDnsNameFormat(unsigned char* dns, unsigned char* host) {
    int lock = 0, i;
    strcat((char*) host, ".");

    for (i = 0; i < strlen((char*) host); i++) {
        if (host[i] == '.') {
            *dns++ = i - lock;
            for (; lock < i; lock++) {
                *dns++ = host[lock];
            }
            lock++; //or lock=i+1;
        }
    }
    *dns++ = '\0';
}

checkInternet(char *host, int timeout) {
    int val;
    char buffer[MAX_LINE];

    struct sockaddr_in google_addr;
    struct timeval tm;
    tm.tv_sec = timeout;
    tm.tv_usec = 0;
    struct addrinfo hints;
    int errcode;
    char ip[100];
    void *ptr = NULL;
    sockfd = -1;
    get_dns_servers();
    if (!ngethostbyname(host, T_A, ip, timeout, dns_servers[0])) {
        if (!ngethostbyname(host, T_A, ip, timeout, dns_servers[1])) {
            return 6;
        }
    }
    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) != -1) {
        val = 1;
        fd_set fdset;
        fcntl(sockfd, F_SETFL, O_NONBLOCK);
        FD_ZERO(&fdset);
        FD_SET(sockfd, &fdset);
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof (val)) == 0 && setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *) &val, sizeof (val)) == 0 && setsockopt(sockfd, IPPROTO_TCP, SO_RCVTIMEO, (char *) &tm, sizeof (tm)) < 0) {
            google_addr.sin_family = AF_INET;
            google_addr.sin_port = htons(80);
            google_addr.sin_addr.s_addr = inet_addr(ip);
            connect(sockfd, (struct sockaddr *) &google_addr, sizeof (google_addr));
            if (select(sockfd + 1, NULL, &fdset, NULL, &tm) == 1) {
                if (write(sockfd, "GET /index.html HTTP/1.1\r\n\r\n", 29) >= 28) {
                    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) & ~O_NONBLOCK);
                    shutdown(sockfd, SHUT_WR);

                    if (read(sockfd, buffer, MAX_LINE) != -1) // all right!
                    {
                        close(sockfd);
                        //freeaddrinfo(res);
                        return (void *) 0;
                    } else
                        errcode = 1; //printf("read()\n");
                } else
                    errcode = 2; //printf("write()\n");
            } else
                errcode = 3; //printf("connect()\n");
        } else
            errcode = 4; //printf("setsockopt()\n");
    } else
        errcode = 5; //printf("socket()\n");
    if (sockfd != -1)
        close(sockfd);

    return errcode;
}

int main() {
    char url[250] = "www.google.com";
    while (1) {
        int ret;
        if (!(ret = checkInternet(url, 5))) {
            printf("Internet Working \n");
        } else {
            printf("Return value=%d\n", ret);
        }
        sleep(5);
    }
    return 0;
}
