/*
*Originally created for testing purpose.
*Made by Kritpal Singh.
*For any further query email me @ kritpal.sing@gmail.com
 Originally created for Mediatek SDK 
Utility to get time from weather underground and set the date .
*/
#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <wchar.h>
#include <signal.h>

#define LOCATION_IP "94.23.155.96"
#define TIME_API_IP "69.163.221.110"
#define AM              0
#define PM              1
#define CONDITION_URL "http://api.wunderground.com/api/qqqqqqqqqq/conditions/q/autoip.json" //Change qqqqq to actual key


char output[180];


char *hostname;
struct hostent *hostinfo = NULL;
int hr = 0, min = 0, sec = 0, date, hour, cnt = 0;
char day[20], month[20], term[3], time_zone[5];
int step = 1;
using namespace std;
int serverSocket;
struct sockaddr_in serverAddr;
char buffer[5000];
int time_images[2], myPid, timerD, temp_cnt;
time_t cur_tm;

typedef struct {
    int year;
    int month;
    int date;
    int utc;

} str_date;

typedef struct {
    int hours;
    int minutes;
    int seconds;

} str_time;

str_date cur_date;
str_time cur_time;

void updateTime(void);
void setTime(void);

main() {
    char temp_buff[512], ch, *temp_ptr, *start_ptr, *end_ptr;
    FILE *fp;
    int cnt = 0, i;
    int ret;
    sprintf(temp_buff, "wget %s", CONDITION_URL); //wget file
    ret = system(temp_buff);
    if (ret != 0) {
        return 0;
    }
    fp = fopen("autoip.json", "r");
    if (fp == NULL) {
        printf("Time: File cannot be opened\n");
        return 0;
    }
    printf("file opened\n");
    fflush(stdout);
    printf("fp=%d\n", fp);
    fseek(fp, 0, SEEK_END); // non-portable
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    while (size) {
        buffer[cnt++] = fgetc(fp);
        size--;
    }

    buffer[cnt] = '\0'; //"display_location":
    printf("file copied\n");
    fflush(stdout);
    temp_ptr = strstr(buffer, "display_location");

    sprintf(temp_buff, "\"local_epoch\":\"");
    temp_ptr = strstr(temp_ptr, temp_buff);
    start_ptr = temp_ptr + strlen(temp_buff);

    printf("start_ptr done\n");
    fflush(stdout);
    end_ptr = strchr(start_ptr, '"');
    i = strlen(start_ptr) - strlen(end_ptr);
    bzero(temp_buff, 255);
    strncpy(temp_buff, start_ptr, i);

    cur_tm = (time_t) atoi(temp_buff);

    sprintf(temp_buff, "\"local_tz_offset\":\"");
    temp_ptr = strstr(temp_ptr, temp_buff);
    start_ptr = temp_ptr + strlen(temp_buff);

    end_ptr = strchr(start_ptr, '"');
    i = strlen(start_ptr) - strlen(end_ptr);
    bzero(temp_buff, 255);
    strncpy(time_zone, start_ptr, i);


    fclose(fp);
    system("rm autoip.json");
    printf("json file removed\n");
    fflush(stdout);
    updateTime();
    setTime();

}

void setTime(void) {

    char buffer[128];
    time_t temp_time;
    struct tm * cur_time;

    temp_time = time(0);
    cur_time = localtime(&temp_time);
    strftime(buffer, 10, "%H", cur_time);
    hr = atoi(buffer);
    hour = hr;
    //    if (hour > 12)
    //        hour = hour - 12;
    //    else if (hour == 0)
    //        hour = 12;

    //    if (hr >= 12)
    //        hr = hr - 12;

    strftime(buffer, 10, "%M", cur_time);
    min = atoi(buffer);
    strftime(buffer, 10, "%S", cur_time);
    sec = atoi(buffer);
    strftime(buffer, 10, "%d", cur_time);
    date = atoi(buffer);
    strftime(buffer, 15, "%A", cur_time);
    strcpy(day, buffer);
    strftime(buffer, 15, "%B", cur_time);
    strcpy(month, buffer);
    strftime(buffer, 3, "%p", cur_time);
    strcpy(term, buffer);

    hr = (hr * 12) + (min / 5);
    return;
}

void updateTime(void) {
    char buffer[128], add_hour[3], add_minutes[3];
    struct tm * current_time;

    current_time = localtime(&cur_tm);
    strftime(buffer, 10, "%H", current_time);
    hr = atoi(buffer);
    hour = hr;
    cur_time.hours = hr;
    //, , , , , 
    //    if (hour > 12)
    //        hour = hour - 12;
    //    else if (hour == 0)
    //        hour = 12;

    //    if (hr >= 12)
    //        hr = hr - 12;

    strftime(buffer, 10, "%M", current_time);
    min = atoi(buffer);
    cur_time.minutes = min;
    strftime(buffer, 10, "%S", current_time);
    sec = atoi(buffer);
    cur_time.seconds = sec;
    strftime(buffer, 10, "%d", current_time);
    date = atoi(buffer);
    cur_date.date = date;
    strftime(buffer, 15, "%A", current_time);
    strcpy(day, buffer);
    strftime(buffer, 15, "%B", current_time);
    strcpy(month, buffer);
    strftime(buffer, 3, "%p", current_time);
    strcpy(term, buffer);
    strftime(buffer, 10, "%m", current_time);
    cur_date.month = atoi(buffer);
    strftime(buffer, 10, "%Y", current_time);
    cur_date.year = atoi(buffer);
    printf("Hour: %d, min: %d, sec: %d, date:%d, month:%d, year:%d\n", cur_time.hours, cur_time.minutes, cur_time.seconds, cur_date.date, cur_date.month, cur_date.year);

    hr = (hr * 12) + (min / 5);

    add_hour[0] = time_zone[1];
    add_hour[1] = time_zone[2];
    add_minutes[0] = time_zone[3];
    add_minutes[1] = time_zone[4];

    if (time_zone[0] == '+') {
        cur_time.hours = cur_time.hours + atoi(add_hour);
        cur_time.minutes = cur_time.minutes + atoi(add_minutes);
        if (cur_time.minutes > 60) {
            cur_time.minutes = cur_time.minutes - 60;
            cur_time.hours = cur_time.hours + 1;
            cur_time.hours > 24 ? cur_time.hours - 24 : cur_time.hours;
        }
    } else if (time_zone[0] == '-') {
        cur_time.hours = cur_time.hours - atoi(add_hour);
        cur_time.minutes = cur_time.minutes - atoi(add_minutes);
        if (cur_time.minutes < 0) {
            cur_time.minutes = 60 - cur_time.minutes;
            cur_time.hours--;
            cur_time.hours = cur_time.hours < 0 ? 24 - cur_time.hours : cur_time.hours;
        }
    }



    printf("Hour: %d, min: %d, sec: %d, date:%d, month:%d, year:%d utc:%d\n", cur_time.hours, cur_time.minutes, cur_time.seconds, cur_date.date, cur_date.month, cur_date.year, cur_date.utc);

//    sprintf(buffer, "date -u %02d%02d%02d%02d%04d.%02d", cur_date.month, cur_date.date, cur_time.hours, cur_time.minutes, cur_date.year, cur_time.seconds, cur_date.utc);
      sprintf(buffer, "date -s \"%02d-%02d-%02d %02d:%02d:%02d\"", cur_date.year,cur_date.month, cur_date.date, cur_time.hours, cur_time.minutes, cur_time.seconds);
    printf("Update time buf=%s\n", buffer);
    system(buffer);

}

