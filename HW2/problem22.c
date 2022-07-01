#include<stdio.h>

int main(){

    int a=2, b=3, c;
    float d=1.0, e=3.5, f;
    
    f=a/b;
    printf("\na) %f\n",f);
    f=((float) a)/b;
    printf("\nb) %f\n",f);
    f=(float)(a/b);
    printf("\nc) %f\n",f);
    c=e/d;
    printf("\nd) %d\n",c);
    c=(int)(e/d);
    printf("\ne) %d\n",c);
    f=((int) e)/d;
    printf("\nf) %f\n",f);

    return 0;
}