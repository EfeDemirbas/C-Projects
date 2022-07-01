#include <stdio.h>

int main(){
    unsigned char index;
    index = 33;
    printf("Decimal | ASCII Character\n------------------------\n");
    for(index=33;index<128;index++){
        if(index<100){
            printf("   %d   |  %c   \n",index,index);
        }
        else{
            printf("   %d  |  %c   \n",index,index);
        }
    }
    return 0;
}