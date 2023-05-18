#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

void Wallet(bool, long, long*);
void Finish(bool, long);

int main(void) {
    long money;
    long bet_money;
    bool win; //승리 여부
    bool loop = true; //기본적으로 반복 게임
    FILE* file;

    //배팅 금액이 저장된 파일 로드
    file = fopen("money.txt", "r"); 
    fscanf(file, "%ld", &money);
    fclose(file);

    //게임 실행 부분
    while(loop) {
        // deck_gen(deck); 덱 생성 함수
        //해당 입력을 다이얼 버튼으로 받을 예정
        scanf("%ld", &bet_money);

        //배팅 금액 계산 함수
        wallet(win, bet_money, &money);

        //배팅금 전부 소진 시 게임 오버 출력 후 배팅금 5000원 충전 & 종료
        if(money == 0) {
            printf("GameOver!\n");
            finish(true, money);
            break;
        }

        //사용자 종료 시 계산된 최종 배팅금을 파일에 저장
        if(bet_money == 0) {
            finish(false, money);
            break;
        }
    }
    return 0;
}

void Wallet(bool game, long betting, long* result) {
        if(game) {
            betting *= 2;
            *result += betting;
        }
        else {
            *result -= betting;
        }
}

void Finish(bool over, long num) {
    FILE* file;
    file = fopen("money.txt", "w"); 

    if(over) {
        num = 5000;
        fprintf(file, "%ld", num);
    }
    else {
        fprintf(file, "%ld", num);
    }
    fclose(file);
}