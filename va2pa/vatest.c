#include "types.h"
#include "stat.h"
#include "user.h"

int gv = 7;                             //global variable

int main(void){
    int lv = 5;                         //local variable
    char *mv = malloc(7);                 //malloc variable
    int invalid = 0x7fffffff;           //invalid variable
    int pa;

    //va2pa tesst

    printf(1, "\n\nGlobal variable: ");       
    pa = va2pa((int)&gv);
    printf(1, "\nPA: 0x%x", pa);

    printf(1, "\n\nLocal variable: ");
    pa = va2pa((int)&lv);
    printf(1, "\nPA: 0x%x", pa);

    printf(1, "\n\nMalloc variable: ");
    pa = va2pa((int)mv);
    printf(1, "\nPA: 0x%x", pa);

    printf(1, "\n\nInvalid variable: ");
    pa = va2pa((int)invalid);
    printf(1, "\nPA: 0x%x (%d)", pa, pa);

    //fork test
    printf(1, "\n\n--Fork test--");

    int pid = fork();
    if(pid == 0){
        printf(1, "\n\nChild process: ");
        pa = va2pa((int)&gv);
        printf(1, "\nPA: 0x%x", pa);
        exit();
    }
    else{
        wait();
        printf(1, "\n\nParent process: ");
        pa = va2pa((int)&gv);
        printf(1, "\nPA: 0x%x\n", pa);
    }

    free(mv);
    exit();
}

