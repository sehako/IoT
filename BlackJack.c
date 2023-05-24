#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

void Finish(bool, long);
void DeckGen(int arr[4][13]);
int Draw(char* , char* , int arr[4][13]);
int Betting(); //배팅을 딥 스위치로 구현할 함수
void CardShow(char* , char* , int); //뽑은 카드를 Dot Matrix와 디지털 패드로 구현할 함수

int main(void) {
    long money = 0;
    long bet_money = 0;
    int arr[4][13]; //행 부분은 카드 문양 열 부분은 카드 값
    int play_check = 1;
    int hit_check = 1;
    bool user_busted = false;
    int first_dealer_card;
    char first_dealer_alpha, first_dealer_shape;
    int user_card, dealer_card;
    char alpha, dealer_alpha, shape, dealer_shape;
    int user_score;
    int dealer_score;
    FILE* file;

    //배팅 금액이 저장된 파일 로드
    file = fopen("money.txt", "r");
    fscanf(file, "%ld", &money);
    fclose(file);

    while(true) { //게임 실행 반복문
        user_score = 0;
        dealer_score = 0;

        //딥 스위치로 배팅금 입력

        DeckGen(&arr); //덱 생성
        //첫 번째 카드 분배
        user_card = Draw(&shape, &alpha, arr);
        dealer_card = Draw(&dealer_shape, &dealer_alpha, arr);
        user_score += user_card;
        dealer_score += dealer_card;
        CardShow(&shape, &alpha, &user_card);
        sleep(3);
        first_dealer_card = dealer_card;
        first_dealer_alpha = dealer_alpha;
        first_dealer_shape = dealer_shape;
        
        //두 번째 카드 분배
        user_card = Draw(&shape, &alpha, arr);
        dealer_card = Draw(&dealer_shape, &dealer_alpha, arr);
        CardShow(&shape, &alpha, user_card);
        sleep(3);
        CardShow(&dealer_shape, &dealer_alpha, dealer_card);
        user_score += user_card;
        dealer_score += dealer_card;

        if(user_score == 21) {
            //블랙잭 구현 부분
            money += bet_money * 2;
            continue;
        }
        else {
            while(true) {
                //힛 or 스탠드 구현
                //int or char hit: 딥 스위치로 입력받을 변수
                if(user_score > 21) {
                    user_busted = true;
                    break;
                }

                if(true) {
                    user_card = Draw(&shape, &alpha, arr);
                    CardShow(&shape, &alpha, user_card, &user_score);
                }
                else {
                    break;
                }
            }
        }
        if(user_busted) {
            user_busted = false;
        }
        else {
            CardShow(&first_dealer_shape, &first_dealer_alpha, first_dealer_card);
            
            //딜러가 가진 카드의 합이 16이하인 경우 딜러는 무조건 드로우
            while(dealer_score <= 16) {
                dealer_card = Draw(&dealer_shape, &dealer_alpha, arr);
                CardShow(&dealer_shape, &dealer_alpha, dealer_card);
                sleep(3);
            }

            //승패 계산
            if(dealer_score > 21) {
                //딜러 버스트 부분 LCD 구현
                money += bet_money * 2;
            }
            else if(user_score > dealer_score) {
                //플레이어 승리 부분 LCD 구현
                money += bet_money * 2;
            }
            else if(user_score == dealer_score) {
                //무승부 부분 LCD 구현
                money += bet_money;
            }
            else {
                //패배
                printf("You lose!\n");
            }
        }
        if(money == 0) {
            //소지금 전부 소진 시
            Finish(true, money);
            break;
        }

        //재시작 부분
        if (true) {
            //사용자 종료 시 계산된 최종 배팅금을 파일에 저장
            continue;
        }
        else {
            Finish(false, money);
            break;
        }
    }
    return 0;
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

//덱 드로우 함수
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

//게임 종료 or 오버 시 파일 처리
void Finish(bool over, long num) {
    FILE* file;
    file = fopen("money.txt", "w");

    if (over) {
        num = 5000;
        fprintf(file, "%ld", num);
        printf("GameOver!\n");
    }
    else {
        fprintf(file, "%ld", num);
        printf("GameFinished\n");
    }
    fclose(file);
}

void CardShow(char* shape, char* alpha, int card) {

}