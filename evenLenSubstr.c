/*
Longest Even Length Substring
For given string ‘str’ of digits, find length of the longest substring of ‘str’, such that the length of the substring is 2k digits and sum of left k digits is equal to the sum of right k digits.
 

Input:

The first line of input contains an integer T denoting the number of test cases. The description of T test cases follows.
Each test case contains a string string of length N.

Output:

Print length of the longest substring of length 2k such that sum of left k elements is equal to right k elements and if there is no such substring print 0.

*/
#include <stdio.h>
#include <string.h>
int maxLength(char *str){
    return strlen(str)%2==0?strlen(str):strlen(str)-1;
}
int sum(int index,char *str,int len){
    int sum=0,i;
    //printf("index=%d,len=%d\n",index,len);
    for(i=0;i<len;i++){
        sum=sum+*(str + index +i);
    }
    return sum;
}
int findLength(char *str){
    int mLen=maxLength(str);
    
    while(mLen>0){
        int j;
   // printf("max Length=%d\n",mLen);
        for(j=0;j<strlen(str)-mLen+1;j++){
           // printf("J=%d,mLen=%d,index=%d,sumLeft=%d,sumRight=%d\n",j,mLen,j+mLen/2,sum(j,str,mLen/2),sum(j+mLen/2,str,mLen/2));
            if(sum(j,str,mLen/2)==sum(j+mLen/2,str,mLen/2)){
                return mLen;
            }
        }
        mLen-=2;
    }
    return 0;
}
int main() {
    int t,n;
    char str[100];
    scanf("%d",&t);
    while(t--){
        scanf("%s",str);
        //sprintf(str,"%d",n);
        printf("%d\n",findLength(str));
    }
    return 0;
}
