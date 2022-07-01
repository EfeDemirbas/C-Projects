#include<stdio.h>
int main(){

    unsigned char a=0x0D, b=0x03, c;
    c = ~a; printf("a) 0x%x\n",c);
    c = a & b; printf("b) 0x0%x\n",c);
    c = a | b; printf("c) 0x0%x\n",c);
    c = a ^ b; printf("d) 0x0%x\n",c);
    c = a >> 3; printf("e) 0x0%x\n",c);
    c = a << 3; printf("f) 0x%x\n",c);
    c &= b; printf("g) 0x0%c\n",c);

    return 0;
}