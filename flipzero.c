/*
Maximize Number of 1's
Given a binary array and an integer m, find the maximum number of consecutive 1's produced by flipping 0's. You can flip maximum m zeroes.

Input:
The first line contains an integer 'T' denoting the total number of test cases. In each test cases, the first line contains an integer 'N' denoting the size of array. The second line contains N space-separated integers A1, A2, ..., AN denoting the elements of the array.  Third line consists of an integer m that is maximum number of flips allowed.
Output:
Output the maximum numbers of consecutive 1's. (Indices begin from 0).
Constraints:
1<=T<=31
1<=N<=100
1<=m<=N
Example:
Input:
1
11
1 0 0 1 1 0 1 0 1 1 1
2

Output:
8
*/
#include <stdio.h>
#include<iostream>
using namespace std;
int findBestWindow(char A[],int len,int m){
int wL = 0, wR = 0,bestWL=0,bestWR=0; 
int nZero = 0;
int bestWindowWidth = -1;

while(wR < len){	
	//expand to the right, update '0' count:
	if (nZero <= m){
		wR++;
		if (A[wR] == '0') nZero++; 
	}

	//shrink from left, update '0' count:
	if (nZero > m){ 
		if (A[wL] == '0') nZero--;
		wL++;
	}
	
	//update best window:
	if (wR - wL > bestWindowWidth){
		bestWindowWidth = wR - wL;
		bestWR = wR;
		bestWL = wL;
	}	
}
printf("%d\n",bestWindowWidth+1);
}
int main(){
    int t,n,i,m;
    char arr[100]={0};
    scanf("%d",&t);
    while(t--){
        scanf("%d",&n);
       // printf("n=%d\n",n);
        for(i=0;i<=n;i++){
            scanf("%d",&arr[i]);
        }
        //std::cin>>arr;
        scanf("%d",&m);
       // printf("%s,M=%d\n",arr,m);
        findBestWindow(arr,n,m);
    }
    
    
}
