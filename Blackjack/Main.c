#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

void Finish(bool, long);
void DeckGen(int [4][13]);
int Draw(char* , char* , int[4][13]);
bool CheckBlackJack(int);
bool Fight(int, int);

int main(void) {
    long money = 0;
    long bet_money = 0;
    int arr[4][13]; //행 부분은 카드 문양 열 부분은 카드 값
    int play_check = 1;
    int hit_check = 1;
    int user_card, dealer_card;
    char alpha, alphabet, shape, dealer_shape;

    bool win; //승리 여부
    bool loop = true; //기본적으로 반복 게임
    FILE* file;

    //배팅 금액이 저장된 파일 로드
    file = fopen("money.txt", "r");
    fscanf(file, "%ld", &money);
    fclose(file);

    printf("%ld\n", money);

    //게임 실행 부분
    while (true) {
        int user_score = 0;
        int dealer_score = 0;

        DeckGen(&arr); //덱 생성 함수

        //해당 입력을 다이얼 버튼으로 받을 예정
        printf("배팅금 입력(소지액: %ld)\n", money);
        scanf("%ld", &bet_money);

        // 처음 게임 시작 시 카드 분배 구현
        user_card = Draw(&shape, &alpha, arr);
        dealer_card = Draw(&dealer_shape, &alphabet, arr);
        user_score += user_card;
        dealer_score += dealer_card;
        printf("%c, %d\n", shape, user_card);
        user_card = Draw(&shape, &alpha, arr);
        dealer_card = Draw(&dealer_shape, &alphabet, arr);
        printf("%c, %d\n", shape, user_card);
        printf("%c, %d\n", shape, dealer_card);
        user_score += user_card;
        dealer_score += dealer_card;

        //블랙잭 체크 함수
        if (CheckBlackJack(user_score)) {
            printf("%ld\n", user_score);
            printf("BlackJack!!\n");
            money += bet_money * 2;
            continue;
        }
        //푸시 채크 부분
        else if (CheckBlackJack(user_score) && CheckBlackJack(dealer_score)) {
            printf("%ld\n", user_score);
            printf("%ld\n", dealer_score);
            printf("Push\n");
        }
        else {
            while (true) {
                printf("%ld\n", user_score);
                printf("1 | Hit   0 | Stand\n");
                scanf("%d", &hit_check);
                if (hit_check == 1) {
                    user_score += Draw(&shape, &alpha, arr);
                }
                else {
                    break;
                }
                if (user_score > 21) {
                    printf("%ld\n", user_score);
                    printf("Busted!\n");
                    money -= bet_money;
                    break;
                }
            }
        }

        //딜러가 가진 카드의 합이 16이하인 경우 드로우하는 규칙 구현
        while (dealer_score <= 16) {
            dealer_score += Draw(&dealer_shape, &alphabet, arr);
        }

        //승패 계산 부분
        if(dealer_score > 21) {
            printf("Dealer busted\n");
            money += bet_money * 2;
        }
        else if(user_score > dealer_score) {
            printf("You win!\n");
            money += bet_money * 2;
        }
        else if(user_score == dealer_score) {
            printf("Push\n");
            money += bet_money * 2;
        }
        else {
            printf("You lose!\n");
            money -= bet_money;
        }

        //배팅금 전부 소진 시 게임 오버 출력 후 배팅금 5000원 충전 & 종료
        if (money == 0) {
            printf("GameOver!\n");
            Finish(true, money);
            break;
        }

        //사용자 종료 시 계산된 최종 배팅금을 파일에 저장
        printf("재도전? (0입력 시 종료)\n");
        scanf("%d", &play_check);
        if (play_check == 0) {
            printf("GameFinished\n");
            Finish(false, money);
            break;
        }
        else {
            continue;
        }
    }
    return 0;
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
    srand(time(NULL)); //랜덤 정수 생성
    while (true) {
        int shape = rand() % 4;
        int card = rand() % 13;

        if (arr[shape][card] != 0) {
            switch (shape) {
            case 0:
                *shape_pt = 'S';
                break;
            case 1:
                *shape_pt = 'D';
                break;
            case 2:
                *shape_pt = 'C';
                break;
            default:
                *shape_pt = 'H';
                break;
            }
            draw = arr[shape][card];
            arr[shape][card] = 0;
            break;
        }
        else {
            continue;
        }
    }

    if (draw >= 11) {
        switch (draw) {
        case 11:
            *alpha_pt = 'J';
            break;
        case 12:
            *alpha_pt = 'Q';
            break;
        default:
            *alpha_pt = 'K';
            break;
        }
        draw = 10;
    }
    return draw;
}

bool CheckBlackJack(int user_sum) {
    if (user_sum == 21) {
        return true;
    }
    else {
        return false;
    }
}

bool Fight(int user, int dealer) {
    if (user > dealer) {
        return true;
    }
    else {
        return false;
    }
}