/*
*Originally created for testing purpose.
*Made by Kritpal Singh.
*For any further query email me @ kritpal.sing@gmail.com
 Originally created for Mediatek SDK 
Utility to get time of Kernel version using /proc/version .
*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
char array[20][40];


int main() {
    FILE *fp;
    char date[40], time[40], month[40], year[40];
    int cnt = 0;
    if ((fp = fopen("/proc/version", "r")) != NULL) {
        char buffer[500];
        fgets(buffer, 499, fp);
        fclose(fp);
//        printf("buf=%s\n", buffer);
        char *token = strtok(buffer, " ");
        while (token != NULL) {
            sprintf(array[cnt], "%s", token);
            cnt++;
            token = strtok(NULL, " ");
        }
//        printf("count=%d\n", cnt);
        sprintf(year, "%s", array[cnt - 1]);
        year[strlen(year) - 1] = '\0';
        sprintf(time, "%s", array[cnt - 3]);
        sprintf(date, "%s", array[cnt - 4]);
        sprintf(month, "%s", array[cnt - 5]);
        int iMonth = 1;
        if (strcmp(month, "Jan")) {
            iMonth = 1;
        } else if (strcmp(month, "Feb")) {
            iMonth = 2;
        } else if (strcmp(month, "Mar")) {
            iMonth = 3;
        } else if (strcmp(month, "Apr")) {
            iMonth = 4;
        } else if (strcmp(month, "May")) {
            iMonth = 5;
        } else if (strcmp(month, "Jun")) {
            iMonth = 6;
        } else if (strcmp(month, "Jul")) {
            iMonth = 7;
        } else if (strcmp(month, "Aug")) {
            iMonth = 8;
        } else if (strcmp(month, "Sep")) {
            iMonth = 9;
        } else if (strcmp(month, "Oct")) {
            iMonth = 10;
        } else if (strcmp(month, "Nov")) {
            iMonth = 11;
        } else if (strcmp(month, "Dec")) {
            iMonth = 12;
        }

        sprintf(buffer, "date -s \"%s-%d-%s %s\"", year, iMonth, date, time);
        printf("final Command=%s\n", buffer);
        system(buffer);
    }

}


