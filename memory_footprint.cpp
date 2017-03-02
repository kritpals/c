/*
*Originally created for testing purpose.
*Made by Kritpal Singh.
*For any further query email me @ kritpal.sing@gmail.com
 Originally created for Mediatek SDK 
Utility to get Memory footprint using PS and meminfo an do a tftp to a particular IP.
*/
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include <fcntl.h>
#include <time.h>

using namespace std;
char PID[20];
char VSZ[20];
char processName[100];
void trimStr(char* str, int cntBytes);
void memoryPrint(int value, char * fileName);
void syncProcData(int value, char *fileName);

main(int argc, char ** argv) {
    while (1) {
        time_t t = time(0); // get time now
        struct tm * now = localtime(& t);

        char fileName[100];
        char memFile[100];
        sprintf(fileName, "Footprint.%d%d%d%d%d.txt", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min);
        sprintf(memFile, "memprint.%d%d%d%d%d.txt", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min);
        //    syncProcData(0, fileName);
        //    for (int i = 0; i < 250; i++) {
        //        sleep(2);
        //        syncProcData(1, fileName);
        //    }
        int i;
        syncProcData(0, fileName);
        memoryPrint(0, memFile);
        for (i = 0; i < 250; i++) {
            printf("Iteration no:%d\r", i + 1);
            fflush(stdout);
            sleep(30);

            //            printf("Iteration no:%d\r", i + 1);
            //            fflush(stdout);
            memoryPrint(1, memFile);
            syncProcData(1, fileName);
        }
        printf("Iteration no:%d\n", i);
        char tftpStr[300];
        if (argc > 1) {
            sprintf(tftpStr, "tftp -pr %s %s", fileName, argv[1]);
            //        //    system("tftp -pr memData.txt 10.10.10.100");
            printf("uploading file = %s\n", tftpStr);
            system(tftpStr);
            sleep(5);
            char IP[40];
            sprintf(IP, "%s", argv[1]);
            sprintf(tftpStr, "tftp -pr %s %s", memFile, IP);
            //    system("tftp -pr memData.txt 10.10.10.100");
            printf("uploading file = %s\n", tftpStr);
            system(tftpStr);

        } else {
            sprintf(tftpStr, "tftp -pr %s 10.10.10.100", fileName);
            //        //    system("tftp -pr memData.txt 10.10.10.100");
            printf("uploading file = %s\n", tftpStr);
            system(tftpStr);
            sleep(5);
            sprintf(tftpStr, "tftp -pr %s 10.10.10.100", memFile);
            //    system("tftp -pr memData.txt 10.10.10.100");
            printf("uploading file = %s\n", tftpStr);
            system(tftpStr);

        }
        //    sprintf(tftpStr, "cat %s", fileName);
        //    system(tftpStr);
        remove(fileName);
        remove(memFile);
    }
}

void syncProcData(int value, char *fileName) {
    FILE *fp, *writeFp, *readFp;

    int step = 0;
    char processData[500];
    char fileData[500];
    char readData[5000];
    char *ch = NULL;
    int flag = 0;
    char *token = NULL;
    system("ps > ps.txt");
    if (value == 0) {
        if ((writeFp = fopen(fileName, "a")) == NULL) {
            printf("===============ERROR:Not able to write in file===================\n");
        }
    } else if (value == 1) {
        if ((readFp = fopen(fileName, "r")) == NULL) {
            printf("===============ERROR:Not able to write in file===================\n");
        }
        if ((writeFp = fopen("memData_copy.txt", "w")) == NULL) {
            printf("===============ERROR:Not able to write in file===================\n");
        }
    }
    if (fp = fopen("ps.txt", "r")) {
        fgets(processData, sizeof (processData), fp); //to avoid title line
        while (fgets(processData, sizeof (processData), fp)) {
            //            printf("read data = %s\n", processData);
            token = strtok(processData, " ");
            if (token != NULL) {
                while (token != NULL) {
                    if (strlen(token) > 0 && !strstr(token, "admin") && !strstr(token, "SW") && strcmp(token, "S")) { //SW<
                        if (step == 0) {
                            sprintf(PID, "%s", token);
                            //                            printf("pid=%s", PID);
                            trimStr(PID, strlen(PID));
                            step = 1;
                        } else if (step == 1) {
                            sprintf(VSZ, "%s", token);
                            //                            printf(",mem size = %s", VSZ);
                            trimStr(VSZ, strlen(VSZ));
                            step = 2;
                        } else if (step == 2) {
                            if (strlen(token) > 2) {
                                sprintf(processName, "%s", token);
                                //                                printf(",process name = %s\n", processName);
                                step = 3;
                                trimStr(processName, strlen(processName));
                            }
                        }
                    }
                    token = strtok(NULL, " ");
                }
                if (value == 0) {
                    sprintf(fileData, "%s,%s,%s\n", PID, processName, VSZ);
                    //                    printf("Formated Data =%sData size =%d\n", fileData, strlen(fileData));
                    fputs(fileData, writeFp);
                } else if (value == 1) {

                    while ((ch = fgets(readData, sizeof (readData), readFp))) {
                        //                        printf("data read=%s\n", readData);
                        //                        trimStr(readData, strlen(readData));
                        readData[strlen(readData) - 1] = '\0';
                        if (strstr(readData, PID) != NULL) {
                            sprintf(readData + strlen(readData), ",%s\n", VSZ);
                            //                            printf("final data =%s\n", readData);
                            fputs(readData, writeFp);
                            flag = 1;
                            //                            printf("PID found\n");
                            break;
                        }
                    }
                    fseek(readFp, 0, SEEK_SET);
                    if (flag == 0) {
                        //                        printf("process not found.\n");
                        //                        fflush(stdout);
                        //                        sprintf(fileData, "%s,%s,%s\n", PID, processName, VSZ);

                        readData[strlen(readData) ] = '\0';
                        readData[strlen(readData) - 1 ] = '\n';
                        fputs(readData, writeFp);

                    }
                    flag = 0;
                    if (ch == NULL) {
                        //                        printf("process not found.\nNew Process");
                        //                        fflush(stdout);
                        if (strstr(processName, "ps.txt") == NULL) {
                            sprintf(fileData, "%s,%s,%s\n", PID, processName, VSZ);
                            fputs(fileData, writeFp);
                        }
                    }
                }

                //                                sprintf(processData[atoi(PID)], "%s", fileData);
                //                processData[atoi(PID)] = fileData;
            }
            step = 0;
            bzero(processData, 499);
        }
    }
    fclose(fp);
    fclose(writeFp);
    if (value == 1) {
        fclose(readFp);
        //        if ((readFp = fopen(fileName, "r")) == NULL) {
        //            printf("===============ERROR:Not able to write in file===================\n");
        //        }
        //        FILE * copyFp;
        //        if ((copyFp = fopen("memData_copy1.txt", "w")) == NULL) {
        //            printf("===============ERROR:Not able to write in file===================\n");
        //        }
        //        if ((writeFp = fopen("memData_copy.txt", "r")) == NULL) {
        //            printf("===============ERROR:Not able to write in file===================\n");
        //        }
        //        char *ch1,*ch2;
        //        while (ch1=fgets(readData, sizeof (readData), readFp) && ch2=fgets(processData, sizeof (processData), writeFp)) {
        //            char * token1,*token2;
        //            token1=strtok(readData,",");
        //            token2=strtok(processData,",");
        //            if(strstr(readData,processData))
        //        }
        char copyStr[200];
        sprintf(copyStr, "cp memData_copy.txt %s ", fileName);
        //        system("cp memData_copy.txt memData.txt");
        system(copyStr);
        remove("memData_copy.txt");
    }
    remove("ps.txt");

}

void memoryPrint(int value, char * fileName) {
    FILE *memFp, *writeFp, *readFp;
    char memFileData[200];
    char *token;
    char processData[200];
    char readData[5000];
    char fileData[200];
    char memTotal[20];
    char memFree[20];
    char kernelStack[20];
    int step = 0;
    if ((memFp = fopen("/proc/meminfo", "r")) == NULL) {
        printf("===============ERROR:Not able to open meminfo file===================\n");
    }
    if (value == 0) {
        if ((writeFp = fopen(fileName, "a")) == NULL) {
            printf("===============ERROR:Not able to write in file===================\n");
        }
    } else if (value == 1) {
        if ((readFp = fopen(fileName, "r")) == NULL) {
            printf("===============ERROR:Not able to write in file===================\n");
        }
        if ((writeFp = fopen("memPrint_copy.txt", "w")) == NULL) {
            printf("===============ERROR:Not able to write in file===================\n");
        }
    }
    while (fgets(memFileData, sizeof (memFileData), memFp)) {
        if (strstr(memFileData, "MemTotal:") || strstr(memFileData, "MemFree:") || strstr(memFileData, "KernelStack:")) {
            //            printf(" got %s\n", memFileData);
            token = strtok(memFileData, ":");
            token = strtok(NULL, " ");
            //            token[strlen(token) - 3] = '\0';
            trimStr(token, strlen(token));
            if (step == 0) {
                sprintf(memTotal, "%s", token);
                step = 1;
            } else if (step == 1) {
                sprintf(memFree, "%s", token);
                step = 2;
            } else if (step == 2) {
                sprintf(kernelStack, "%s", token);
                step = 3;
            }
        }
    }
    if (value == 0) {
        sprintf(fileData, "MemTotal,%s\nMemFree,%s\nKernelStack,%s\n", memTotal, memFree, kernelStack);
        //        printf("file Data=%s", fileData);
        fputs(fileData, writeFp);
    } else if (value == 1) {
        while (fgets(readData, sizeof (readData), readFp)) {
            readData[strlen(readData) - 1] = '\0';
            if (strstr(readData, "MemTotal")) {
                sprintf(readData + strlen(readData), ",%s\n", memTotal);
                fputs(readData, writeFp);
            } else if (strstr(readData, "MemFree")) {
                sprintf(readData + strlen(readData), ",%s\n", memFree);
                fputs(readData, writeFp);
            } else if (strstr(readData, "KernelStack")) {
                sprintf(readData + strlen(readData), ",%s\n", kernelStack);
                fputs(readData, writeFp);
            }

        }

    }
    fclose(memFp);
    fclose(writeFp);
    if (value == 1) {
        fclose(readFp);
        sprintf(processData, "cp memPrint_copy.txt %s", fileName);
        system(processData);
        remove("memPrint_copy.txt");
    }
}

void trimStr(char* str, int cntBytes) {
    int iTrim, iTrim2;
    for (iTrim = 0; iTrim <= cntBytes; iTrim++) {
        if (str[iTrim] == ' ') {
            for (iTrim2 = iTrim + 1; iTrim2 <= cntBytes; iTrim2++) {
                str[iTrim2 - 1] = str[iTrim2];
            }
            str[iTrim2 - 1] = '\0';
            iTrim--;
        } else if (str[iTrim] == '\r') {
            for (iTrim2 = iTrim + 1; iTrim2 <= cntBytes; iTrim2++) {
                str[iTrim2 - 1] = str[iTrim2];
            }
            str[iTrim2 - 1] = '\0';
            iTrim--;
        } else if (str[iTrim] == '\n') {
            for (iTrim2 = iTrim + 1; iTrim2 <= cntBytes; iTrim2++) {
                str[iTrim2 - 1] = str[iTrim2];
            }
            str[iTrim2 - 1] = '\0';
            iTrim--;
        } else if (str[iTrim] == '\t') {
            for (iTrim2 = iTrim + 1; iTrim2 <= cntBytes; iTrim2++) {
                str[iTrim2 - 1] = str[iTrim2];
            }
            str[iTrim2 - 1] = '\0';
            iTrim--;
        }
    }
    str[cntBytes] = '\0';
}


