#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

void finish(long);

int main(void) {
    long money;
    FILE* file;

    file = fopen("money.txt", "r"); //배팅 금액이 저장된 파일을 불러옴
    fscanf(file, "%ld", &money);
    fclose(file);

    finish(money);
    
    return 0;
}

void finish(long num) {
    FILE* file;
    file = fopen("money.txt", "w"); //배팅 금액이 저장된 파일을 불러옴
    fprintf(file, "%ld", num);
    fclose(file);
}