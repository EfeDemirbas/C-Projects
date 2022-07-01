#include <stdio.h>
#include <string.h>
#define MAXLENGTH 200   // max length of string input

void getString(char *str); // helper prototypes
void printResult(char *str);
int greaterThan(char ch1, char ch2);
void swap(char *str, int index1, int index2);

int main(void) {
    int len;    // length of the entered string
    char str[MAXLENGTH];    // input should be no longer than MAXLENGTH

    // here, any other variables you need
    int index1, index2;

    getString(str);
    len = strlen(str);  // get length of the string, from string.h

    for(int j=1;j<len;j++){
        for(int i=0;i<len-j;i++){
            index1=i;
            index2=i+1;
            if(greaterThan(str[index1],str[index2])){
                swap(str,index1,index2);
            }
        }
    }
    printResult(str);
    return(0);
}


//Helper Functions


void getString(char *str){

    printf("\n\nInput your favourite characters to sort: ");
    scanf("%s",str);  
    
    return;
}

void printResult(char *str){

    printf("\nHere is the sorted version of your favourite caharacters: %s\n\n",str);

    return;
}

int greaterThan(char ch1, char ch2){

    if(ch1>ch2){
        return 1;
    }
    else{
        return 0;
    }
}

void swap(char *str, int index1, int index2){

    char a=str[index2];
    str[index2]=str[index1];
    str[index1]=a;

    return;
}