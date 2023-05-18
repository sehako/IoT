#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

void Wallet(bool, long, long*);
void Finish(bool, long);
void DeckGen(int [4][13]);
int Draw(char* , char* , int[4][13]);

int main(void) {
    long money;
    long bet_money = 0;
    int arr[4][13]; //행 부분은 카드 문양 열 부분은 카드 값
    char alpha, alphabet, shape, dealer_shape;

    bool win; //승리 여부
    bool loop = true; //기본적으로 반복 게임
    FILE* file;

    //배팅 금액이 저장된 파일 로드
    file = fopen("money.txt", "r");
    fscanf(file, "%ld", &money);
    fclose(file);

    //게임 실행 부분
    while (loop) {
        DeckGen(&arr); //덱 생성 함수
        int user_card = Draw(&shape, &alpha, arr);
        int dealer_card = Draw(&dealer_shape, &alphabet, arr);

        printf("%c %c\n", shape, dealer_shape);
        printf("%d %d\n", user_card, dealer_card);
        printf("%c %c\n", alpha, alphabet);

        // deck_gen(deck); 덱 생성 함수
        //해당 입력을 다이얼 버튼으로 받을 예정
        scanf("%ld", &bet_money);

        //배팅 금액 계산 함수
        Wallet(false, bet_money, &money);

        //배팅금 전부 소진 시 게임 오버 출력 후 배팅금 5000원 충전 & 종료
        if (money == 0) {
            printf("GameOver!\n");
            Finish(true, money);
            break;
        }

        //사용자 종료 시 계산된 최종 배팅금을 파일에 저장
        if (bet_money == 0) {
            Finish(false, money);
            break;
        }
    }

    //전체 덱 출력 테스트
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 13; j++) {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
    return 0;
}

// 승패에 따른 배팅금 처리
void Wallet(bool game, long betting, long* result) {
    if (game) {
        betting *= 2;
        *result += betting;
    }
    else {
        *result -= betting;
    }
}

//게임 종료 or 오버 시 파일 처리
void Finish(bool over, long num) {
    FILE* file;
    file = fopen("money.txt", "w");

    if (over) {
        num = 5000;
        fprintf(file, "%ld", num);
    }
    else {
        fprintf(file, "%ld", num);
    }
    fclose(file);
}

//덱 생성 함수
void DeckGen(int arr[4][13]) {
    for (int i = 0; i < 4; i++) {
        int count = 1;
        for (int j = 0; j < 13; j++) {
            arr[i][j] = count++;
        }
    }
}

//카드 드로우 함수 구현
int Draw(char* shape_pt, char* alpha_pt, int arr[4][13]) {
    int draw = 0;
    srand(time(NULL));

    while (draw < 10) {
        int shape = rand() % 4;
        int card = rand() % 13;

        if (arr[shape][card] != 0) {
            if (arr[shape][card] >= 11) {
                draw = 10;
                arr[shape][card] = 0;
            }
            else {
                draw = arr[shape][card];
                arr[shape][card] = 0;
                break;
            }
        }
        else {
            continue;
        }
    }
    return draw;
}