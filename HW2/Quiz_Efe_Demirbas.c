#include<stdio.h>
#include<math.h>

/*unsigned char specialsum(a,b){

unsigned char c;
if((a>128&&b>127)||(a>127&&b>128)){
c=a+b-256;
}
else{
c=a+b;
}

return c;
}

int main(){

unsigned char a=175, b=100, c;
c=specialsum(a,b);
printf("%d",c);

return 0;
}*/

/*int main(){

unsigned char q[5] = {2, 12, 'j', 128, 10}; 
unsigned char a; int b; 

a = *(q+1);
printf("a)%d\n",a);

b = ((int)q[3]) << 2;
printf("b)%d\n",b);

a = q[q[0]]+1;
printf("c)%c\n",a);

b = q[3]*2;
printf("d)%d\n",b);

b = q[4]/q[5];
printf("e)%d\n",b);

return 0;
}*/

/*int main(){

char i=97;

while (i<=122)
{
printf("%c\n",i);
i++;
}


return 0;
}*/


/*typedef struct{
    int id;
    char brand[50];
    float miles[10];
}car;


car funbyvalue(car a){
    for (int i = 0; i < 9; i++)
    {
        a.miles[i+1]=((float) pow(2,i+1))*a.miles[0];
    }
    return a;
}

void funbypoint(car *a){
    for (int i = 0; i < 9; i++)
    {
        a->miles[i+1]=((float) pow(2,i+1))*a->miles[0];
    }
    
}


int main(){
    car example;
    example.id=1;
    example.brand[0]=0;
    example.miles[0]=1;
    example=funbyvalue(example);
    printf("%f",example.miles[9]);
    for (char i = 0; i < 10; i++)
    {
        example.miles[i]=0.0;
    }
    car example2;
    example2.miles[0]=1;
    car *example2p;
    example2p=&example2;
    funbypoint(example2p);
    /*scanf("%s",examplep->brand);
    printf("%s",example.brand);
    printf("\nPointer %f",example2p->miles[9]);

    return 0;
}*/




int main()
{
    char a[100];
    int b;
    printf("Please enter a string and a number between 1 and 8 inclusive\n");
    scanf("%s",&a);
    scanf("%d",&b);
    
    if((b<1)||(b>8)){
        for (;!((b>=1) && (b<=8));)
        {
            printf("The number you entered is out of range please try again.\n");
            scanf("%d",&b);
        }
    }
    for (int i = 0; i < 100; i++){
        if(a[i]==0){
            break;
        }
        a[i]=a[i]-b;
    }
    printf("\n%s",a);
    
    return 0;
}
