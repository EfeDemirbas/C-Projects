#include<stdio.h>

int main(){

    int i, k=6;
    i=3*(5>1);
    printf("%d\n",i);
    i=3*(5>1)+(k=2);
    printf("%d %d\n",i,k);
    i=3*(5>1)+(k=2)+(k==6);
    printf("%d\n",i);

    return 0;
}