/**
Originally created for testing purpose.
Made by Kritpal Singh.
For any further query email me @ kritpal.sing@gmail.com
 */
#include <cstdlib>
#include <string.h>
#include <ctime>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <math.h>
#include "curl/curl.h"
#include "curl/easy.h"
#include <net/if.h>
#include <pthread.h>
static size_t write_data2(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);

    return written;
}
int DownloadFile(char *urlPath, char *fileName) {

    CURL *curl;
    FILE *fp;
    CURLcode res;
    char *outfilename = fileName;
    curl = curl_easy_init();
    if (curl) {
        fp = fopen(outfilename, "wb");

        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0); //no one should see the url so we are doing verbose 0

        curl_easy_setopt(curl, CURLOPT_CAINFO, "/almond/cacert.pem");

        curl_easy_setopt(curl, CURLOPT_URL, urlPath);

        //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data2);

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

       // curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);


        // VERIFYPEER needed to ignore certificate key
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);

        printf("fp before free=%d\n", fileno(fp));
        fclose(fp);
        if (res == CURLE_OK) {
            return 0;
        } else {
            printf("downloading from weather data failed");
            return -1;
        }
    } else
        printf("curl_easy_init failed");
    return 0;
}
int main(){
char fetch[200];
 sprintf(fetch, "https://www.google.com:8080/api");
 while(1){
 DownloadFile(fetch,"/tmp/autoip.json");
 printf("File downloaded\n");
sleep(5);
}
}
