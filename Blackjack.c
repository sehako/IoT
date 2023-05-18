#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

void finish(long);

int main(void) {
    long money;
    long bet_money;
    bool win;
    FILE* file;

    file = fopen("money.txt", "r"); //배팅 금액이 저장된 파일을 불러    옴
    fscanf(file, "%ld", &money);
    fclose(file);

    scanf("%ld", &bet_money);

    // 게임 실행 코드 구현 예정

    if(win) {
        bet_money *= 2;
        money += bet_money;
    }
    else {
        money -= bet_money;
    }

    finish(money);
    return 0;
}

void finish(long num) {
    FILE* file;
    file = fopen("money.txt", "w"); 
    fprintf(file, "%ld", num);
    fclose(file);
}