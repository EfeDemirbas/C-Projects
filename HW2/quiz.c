#include<stdio.h>

typedef struct{
    int id;
    char name[100];
} record;

int myfun();
record myfun2(record f);
void getname(record *rp);
void getid(record *rp);



int main(){
    record r;
    r.id=19;
    r.name[0]='E';
    r.name[1]='f';
    r.name[2]='e';
    r.name[3]=0;
    //or//
    sprintf(r.name,"%s","Efe");
    printf("%s",r.name);
    r.id=myfun();
    r = myfun2(r);
    //by using a pointer//
    record *rp;
    rp = &r;
    rp->id = 19;
    rp->name[0]='E';
    sprintf(rp->name,"%s","Efe");
    scanf("%s",rp->name);
    //using a pointer, get a name using a function//
    getname(rp);
    getid(rp);
}

//write a function that gets an ID from a user//

int myfun(){
    int i;
    printf("type in id");
    scanf("%d",&i);
    return i;
}


record myfun2(record f){
    scanf("%d",&f.id);
    return f;
}

void getname(record *rp){
    scanf("%s",rp->name);
}

void getid(record *rp){
    scanf("%d",&rp->id);
}
