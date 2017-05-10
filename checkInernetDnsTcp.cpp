/* 
 * File:   checkInernetDnsTcp.cpp
 * Author: Kritpal
 *
 * Created on 27 April, 2017, 10:16 AM
 * g++ checkInternetDnsTcp.cpp -lpthread -w  -o /tftpboot/tcp
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
#include <arpa/inet.h>
#include <errno.h>

#define MAX_LINE 200
#define TCP_NODELAY 1
#define T_A 1 //Ipv4 address

class CheckInternet {
public:
    CheckInternet();
    int ping(char *url, int timeOut);
    static void *thread_call(void *data);

private:
    int status;
    pthread_mutex_t calculating; // = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t done; // = PTHREAD_COND_INITIALIZER;
    int sockfd, dns_fd;
    char dns_servers[2][50];
    char host[250];
    u_char* ReadName(unsigned char* reader, unsigned char* buffer, int* count);
    int ngethostbyname(unsigned char *host, int query_type, char * ip);
    void ChangetoDnsNameFormat(unsigned char* dns, unsigned char* host);
    int pingUrl(char *host);

    int do_or_timeout(char *url, struct timespec *max_wait);

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
    //Pointers to resource record contents

    struct RES_RECORD {
        unsigned char *name;
        struct R_DATA *resource;
        unsigned char *rdata;
    };

};

u_char* CheckInternet::ReadName(unsigned char* reader, unsigned char* buffer, int* count) {
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
    return (u_char*) "";
}

/*
 * Perform a DNS query by sending a packet
 * */
int CheckInternet::ngethostbyname(unsigned char *host, int query_type, char * ip) {
    unsigned char buf[9192], *qname, *reader;
    int i, j, stop;

    struct sockaddr_in a;

    struct RES_RECORD answers; //the replies from the DNS server
    struct sockaddr_in dest;

    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;

   // printf("Resolving %s\n with dns=%s", host, dns_servers[0]);

    dns_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //UDP packet for DNS queries

    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr(dns_servers[0]); //dns servers

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
    if (sendto(dns_fd, (char*) buf, sizeof (struct DNS_HEADER) + (strlen((const char*) qname) + 1) + sizeof (struct QUESTION), 0, (struct sockaddr*) &dest, sizeof (dest)) < 0) {
        //        perror("sendto failed");
        return 0;
    }
    //    printf("Done");

    //Receive the answer
    i = sizeof dest;
    int size = 0;
    //    printf("\nReceiving answer...");
    if ((size = recvfrom(dns_fd, (char*) buf, 65536, 0, (struct sockaddr*) &dest, (socklen_t*) & i)) < 0) {
        //        perror("recvfrom failed");
        return 0;
    }
    //    printf("Done");

    dns = (struct DNS_HEADER*) buf;

    //move ahead of the dns header and the query field
    reader = &buf[sizeof (struct DNS_HEADER) + (strlen((const char*) qname) + 1) + sizeof (struct QUESTION)];
    //Start reading answers
    stop = 0;
    for (i = 0; i < ntohs(dns->ans_count); i++) {
        ReadName(reader, buf, &stop);
        reader = reader + stop;
        
        answers.resource = (struct R_DATA*) (reader);
        reader = reader + sizeof (struct R_DATA);
        //printf("Reading position =%d\n",reader-buf);
        if (ntohs(answers.resource->type) == 1) //if its an ipv4 address
        {
            int ip1;
            memmove((char*)&ip1,reader,ntohs(answers.resource->data_len));
            a.sin_addr.s_addr = (ip1); //working without ntohl
            printf("IP : %s\n", inet_ntoa(a.sin_addr));
            sprintf(ip, "%s", inet_ntoa(a.sin_addr));
            reader = reader + ntohs(answers.resource->data_len);
            break;
        } else {
            ReadName(reader, buf, &stop);
            reader = reader + stop;
        }
    }

    return 1;
}

/*
 * This will convert www.google.com to 3www6google3com 
 * got it :)
 * */
void CheckInternet::ChangetoDnsNameFormat(unsigned char* dns, unsigned char* host) {
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

int CheckInternet::pingUrl(char *host) {
    int val;
    char buffer[MAX_LINE];

    struct sockaddr_in google_addr;
    struct timeval tm;
    tm.tv_sec = 3;
    tm.tv_usec = 0;
    char ip[50];
    sockfd = -1;
    if (ngethostbyname((unsigned char*) host, T_A, ip)) {
        if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) != -1) {
            val = 1;
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof (val)) == 0 && setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *) &val, sizeof (val)) == 0 && setsockopt(sockfd, IPPROTO_TCP, SO_RCVTIMEO, (char *) &tm, sizeof (tm)) < 0) {
                google_addr.sin_family = AF_INET;
               
                google_addr.sin_port = htons(80);
                google_addr.sin_addr.s_addr = inet_addr(ip);
                if (connect(sockfd, (struct sockaddr *) &google_addr, sizeof (google_addr)) == 0) {
                    if (write(sockfd, "GET /index.html HTTP/1.1\r\n\r\n", 29) >= 28) {
                        shutdown(sockfd, SHUT_WR);
                        if (read(sockfd, buffer, MAX_LINE) != -1) // all right!
                        {
                            close(sockfd);
                            status = 0;
                            return 0;
                        } else
                            printf("read()\n");
                    } else
                        printf("write()\n");
                } else
                    printf("connect()\n");
            } else
                printf("setsockopt()\n");
        } else
            printf("socket()\n");

    } else
        printf("cannot resolve IP.\n"); // this is is the most common error.

    //    if (sockfd != -1)
    //        close(sockfd);
    status = 1;
    return 1;
}

void *CheckInternet::thread_call(void *context) {
    int oldtype;
    CheckInternet *cI = (CheckInternet*) context;
    /* allow the thread to be killed at any time */
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);
    //printf("Host=%s\n", cI->host);
    /* ... calculations and expensive io here, for example:
     * infinitely loop
     */
    cI->pingUrl((char*) cI->host);
    //sleep(10);
    /* wake up the caller if we've completed in time */
    pthread_cond_signal(&cI->done);
    pthread_exit((void*) 1);
    return NULL;
}

/* note: this is not thread safe as it uses a global condition/mutex */
int CheckInternet::do_or_timeout(char *url, struct timespec *max_wait) {
    struct timespec abs_time;
    pthread_t tid = NULL;
    int err;
    strcpy(host, url);
    pthread_mutex_init(&calculating, NULL);
    pthread_cond_init(&done, NULL);
    pthread_mutex_lock(&calculating);

    /* pthread cond_timedwait expects an absolute time to wait until */
    clock_gettime(CLOCK_REALTIME, &abs_time);
    abs_time.tv_sec += max_wait->tv_sec;
    abs_time.tv_nsec += max_wait->tv_nsec;
    pthread_attr_t attrs;
    pthread_attr_init(&attrs);

    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);
    pthread_create(&tid, &attrs, thread_call, (void*) this);

    /* pthread_cond_timedwait can return spuriously: this should
     * be in a loop for production code
     */
    err = pthread_cond_timedwait(&done, &calculating, &abs_time);
    pthread_cancel(tid);
    pthread_join(tid, NULL);
    if (err == ETIMEDOUT) {
        fprintf(stderr, "%s: calculation timed out\n", __func__);
        status = 1;
    }
    //if (!err)
    pthread_mutex_unlock(&calculating);
    pthread_mutex_destroy(&calculating);
    pthread_cond_destroy(&done);
    pthread_attr_destroy(&attrs);
    if (sockfd != -1)
        close(sockfd);
    if (dns_fd != -1) {
        close(dns_fd);
        dns_fd = -1;
    }
    return err;
}

CheckInternet::CheckInternet() {
    status = -1;
    sockfd = -1;
    dns_fd = -1;

    FILE *fp;
    char line[200], *p;
    if ((fp = fopen("/etc/resolv.conf", "r")) == NULL) {
        printf("Resolve file not present\n");
        sprintf(this->dns_servers[0], "8.8.8.8");
        sprintf(this->dns_servers[1], "208.67.220.220");

    } else {
        int i = 0;
        while (fgets(line, 199, fp)) {
            if (line[0] == '#') {
                continue;
            }
            if (strncmp(line, "nameserver", 10) == 0) {
                p = strtok(line, " ");
                p = strtok(NULL, " ");
                strcpy(this->dns_servers[i++], p);

            }
            if (i == 2) {
                break;
            }
        }
        fclose(fp);
    }

}

int CheckInternet::ping(char* url, int timeOut) {
    struct timespec max_wait;
    memset(&max_wait, 0, sizeof (max_wait));
    // system("date");
    /* wait at most 2 seconds */
    max_wait.tv_sec = timeOut;
    do_or_timeout(url, &max_wait);
    return status;
}

int main() {

    while (1) {
        system("date");
        CheckInternet cI;
        char host[250] = "www.google.com";
        if (cI.ping(host, 5)) {
            printf("No internet\n");
        } else {
            printf("Internet working\n");
            sleep(5);
        }


    }
    return 0;
}
