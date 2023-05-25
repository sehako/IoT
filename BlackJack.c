#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

void ResetHand(char[10], char[10]);
int Betting(); //배팅을 딥 스위치로 구현할 함수
int Draw(char* , char* , int arr[4][13]);
void AceCheck(char*, int*, int*);
void CardShow(char* , char* , int); //뽑은 카드를 Dot Matrix와 디지털 패드로 구현할 함수
bool HandCheck(char*);
void Finish(bool, long);

int main(void) {
    long money = 0;
    long bet_money = 0;
    bool user_busted = false;
    int user_card, dealer_card, first_dealer_card, user_score, dealer_score, user_count, dealer_count;
    char alpha, dealer_alpha, shape, dealer_shape, first_dealer_alpha, first_dealer_shape;
    char user_hand[10] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
    char dealer_hand[10] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
    FILE* file;

    //배팅 금액이 저장된 파일 로드
    file = fopen("money.txt", "r");
    fscanf(file, "%ld", &money);
    fclose(file);

    while(true) { //게임 실행 반복문
        user_score = 0;
        dealer_score = 0;
        user_count = 0;
        dealer_count = 0;

        //딥 스위치로 배팅금 입력

        int arr[4][13] = {{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},
        {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}
        }; //행 부분은 카드 문양 열 부분은 카드 값

        //첫 번째 카드 분배
        user_card = Draw(&shape, &alpha, arr);
        dealer_card = Draw(&dealer_shape, &dealer_alpha, arr);
        AceCheck(&alpha, user_hand, &user_count);
        AceCheck(&dealer_alpha, dealer_hand, &dealer_count);
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
        AceCheck(&alpha, user_hand, &user_count);
        AceCheck(&dealer_alpha, dealer_hand, &dealer_count);
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
                    if(HandCheck(user_hand)) user_score -= 10;
                    else {
                        user_busted = true;
                        break;
                    } 
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
            continue;
        }
        else {
            //사용자 종료 시 계산된 최종 배팅금을 파일에 저장
            Finish(false, money);
            break;
        }
    }
    return 0;
}

//손패 초기화
void ResetHand(char arr[10], char ar[10]) {
    for(int i = 0; i < 10; i++) {
        arr[i] = ' ';
        ar[i] = ' ';
    }
}

//덱 드로우 함수
int Draw(char* shape_pt, char* alpha_pt, int arr[4][13]) {
    int draw = 0;
    srand(time(NULL)); //랜덤 정수 생성
    while (true) {
        int shape = rand() % 4;
        int card = rand() % 13;

        if (arr[shape][card] != 0) { //0값이 아닌 배열이 나올때까지 반복
            switch (shape) {
            case 0:
                *shape_pt = '♠';
                break;
            case 1:
                *shape_pt = '◆';
                break;
            case 2:
                *shape_pt = '♣';
                break;
            default:
                *shape_pt = '♥';
                break;
            }
            draw = arr[shape][card];
            arr[shape][card] = 0; //뽑힌 배열값을 0으로 변경(중복 방지)
            break;
        }
        else {
            continue;
        }
    }

    switch (draw) {
        case 0:
            *alpha_pt = 'A';
            draw = 11;
            break;
        case 11:
            *alpha_pt = 'J';
            draw = 10;
            break;
        case 12:
            *alpha_pt = 'Q';
            draw = 10;
            break;
        default:
            *alpha_pt = 'K';
            draw = 10;
            break;
    }
    return draw;
}

void AceCheck(char* alphabet, int* arr, int* arr_count) {
    if(*alphabet == 'A') {
        arr[*arr_count] = 'A';
        *arr_count++;
        *alphabet = ' ';
    }
}

//Dot Matrix로 카드 구현
void CardShow(char* shape, char* alpha, int card) {
    switch(*alpha) {
        case 'K':
        *alpha = ' ';
        break;
        case 'Q':
        *alpha = ' ';
        break;
        case 'J':
        *alpha = ' ';
        break;
    }
}

bool HandCheck(char* arr) {
    for(int i = 0; i < 10; i++) {
        if(arr[i] == 'A') {
            arr[i] = ' ';
            return true;
        }
        else return false;
    }
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