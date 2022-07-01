#include<stdio.h>

int main(){
    int demirinpipisi;
    while(1){
    printf("\nDemirin pipi bouyunu girin: ");
    scanf("%d",&demirinpipisi);
    if (demirinpipisi>10)
    {
        printf("Hadi lan ordan tekrar gir..");
    }
    else{
        break;
    }}

    demirinpipisi=demirinpipisi-2;
    printf("Dogru degere yakinsin, gercek uzunlugu %dcm.",demirinpipisi);
    
}