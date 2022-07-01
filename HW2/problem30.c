#include<stdio.h>

int main(){

    int x[4]={4,3,2,1};
    printf("a) %d \n",x[1]);
    printf("b) %p\n",*x);
    printf("c) %p\n",*(x+2));
    printf("d) %p\n",(*x)+2);
    //printf("e) \n",(*x[3]));
    printf("f) %d\n",x[4]);
    printf("g) %p\n",*(&(x[1])+1));

    return 0;
}