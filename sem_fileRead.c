/*
*Originally created for testing purpose.
*Made by Kritpal Singh.
*For any further query email me @ kritpal.sing@gmail.com
 Originally created for Openwrt SDK 
 Reading multiple files using semaphore.
 Test to check accuracy of semaphores.
 */
#include<sys/msg.h>
#include<sys/ipc.h>
#include<fcntl.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/sem.h>
#include<unistd.h>
#include<stdlib.h>
#include<time.h>
#include<utime.h>
//#include<linux/dirent.h>
#include<grp.h>
#include<pwd.h>
#include<signal.h>
#include<sys/resource.h>
#include<dirent.h>
char ch1,ch2;
main()
{
struct sembuf v;
int id;
char i=0;
id=semget(1, 2, IPC_CREAT | IPC_EXCL | 0600);
int fd1,fd2,fd3;
fd1=open("file1",O_RDONLY);
fd2=open("file2",O_RDONLY);
fd3=open("file3",O_CREAT|O_WRONLY);
if(fork())
{

	
	printf("Going to read file1\n");
	while(read(fd1,&ch1,1)!=0){
		v.sem_num=1;
		v.sem_op=-1;
		v.sem_flg=0;
		semop(id,&v,1);
		v.sem_num=0;
		v.sem_op=-1;
		v.sem_flg=0;
		semop(id,&v,1);
		printf("fork ch=%c\n",ch1);
		write(fd3,&ch1,1);
		v.sem_num=1;
		v.sem_op=1;
		v.sem_flg=0;
		semop(id,&v,1);
		v.sem_num=0;
		v.sem_op=1;
		v.sem_flg=0;
		semop(id,&v,1);

	}
}
else
{
	printf("Going to read file2\n");
	while(read(fd2,&ch2,1)!=0){
		v.sem_num=1;
		v.sem_op=-1;
		v.sem_flg=0;
		semop(id,&v,1);
		v.sem_num=0;
		v.sem_op=-1;
		v.sem_flg=0;
		semop(id,&v,1);
		printf("child ch=%c\n",ch2);
		write(fd3,&ch2,1);
		v.sem_num=1;
		v.sem_op=1;
		v.sem_flg=0;
		semop(id,&v,1);
		v.sem_num=0;
		v.sem_op=1;
		v.sem_flg=0;
		semop(id,&v,1);

	}	
}

}

