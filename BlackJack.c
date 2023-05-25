#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>


void ResetHand(char*, char*);
int Betting(); //배팅을 딥 스위치로 구현할 함수
int Draw(char*, char*, char(*)[13], char*);
void CardShow(char*, char*); //뽑은 카드를 Dot Matrix와 디지털 패드로 구현할 함수
bool HandCheck(char*);
void Finish(bool, long);

int main(void) {
    long money = 0;
    long bet_money = 0;
    bool user_busted = false;
    int user_card, dealer_card, first_dealer_card, user_score, dealer_score, user_count, dealer_count;
    char alpha, dealer_alpha, shape, dealer_shape, first_dealer_alpha, first_dealer_shape;
    FILE* file;

    //배팅 금액이 저장된 파일 로드
    file = fopen("money.txt", "r");
    fscanf(file, "%ld", &money);
    fclose(file);

    //게임 실행 반복문
    while (true) {
        //손패 초기화
        char user_hand[10] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
        char dealer_hand[10] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
        user_score = 0;
        dealer_score = 0;

        //딥 스위치로 배팅금 입력
        printf("배팅금 입력 %ld\n", money);
        scanf("%ld", &bet_money);
        money -= bet_money;

        //행 부분은 카드 문양 열 부분은 카드 값
        char deck[4][13] = { {'A', '2', '3', '4', '5', '6', '7', '8', '9', '10', 'Q', 'K', 'J'},
        {'A', '2', '3', '4', '5', '6', '7', '8', '9', '10', 'Q', 'K', 'J'},
        {'A', '2', '3', '4', '5', '6', '7', '8', '9', '10', 'Q', 'K', 'J'},
        {'A', '2', '3', '4', '5', '6', '7', '8', '9', '10', 'Q', 'K', 'J'} };

        //첫 번째 카드 분배
        user_score += Draw(&shape, &alpha, deck, user_hand);
        dealer_score += Draw(&dealer_shape, &dealer_alpha, deck, dealer_hand);

        //딜러의 첫 번째 카드 저장 플레이어와 승부시 출력할 용도
        first_dealer_alpha = dealer_alpha;
        first_dealer_shape = dealer_shape;

        //두 번째 카드 분배
        user_score += Draw(&shape, &alpha, deck, user_hand);
        dealer_score += Draw(&dealer_shape, &dealer_alpha, deck, dealer_hand);

        //초반 시작 부분에서 에이스 두 장 뽑으면 자동으로 11, 1로 취급
        if (user_score > 21) {
            HandCheck(user_hand);
            user_score -= 10;
        }
        if (dealer_score > 21) {
            HandCheck(dealer_hand);
            user_score -= 10;
        }

        if (user_score == 21) {
            //블랙잭 구현 부분
            money += bet_money * 2;
            continue;
        }
        else {
            while (true) {
                int check = 0;
                //힛 or 스탠드 구현
                scanf("%d", &check);
                if (user_score > 21) {
                    if (HandCheck(user_hand)) user_score -= 10;
                    else {
                        user_busted = true;
                        break;
                    }
                }
                if (check == 1) {
                    user_score += Draw(&shape, &alpha, deck, user_hand);
                }
                else break;
            }
        }
        if (user_busted) user_busted = false;
        else {
            bool dealer_bust = false;

            //딜러가 가진 카드의 합이 16이하인 경우 딜러는 무조건 드로우
            while (dealer_score <= 16) {
                dealer_score += Draw(&dealer_shape, &dealer_alpha, deck, dealer_hand);
                if (dealer_score > 21) {
                    if (HandCheck(dealer_hand)) {
                        dealer_score -= 10;
                    }
                    else {
                        dealer_bust = true;
                        break;
                    }
                }
            }

            //승패 계산
            if (dealer_bust) {
                //딜러 버스트 부분 LCD 구현
                money += bet_money * 2;
            }
            else if (user_score > dealer_score) {
                //플레이어 승리 부분 LCD 구현
                money += bet_money * 2;
            }
            else if (user_score == dealer_score) {
                //무승부 부분 LCD 구현
                money += bet_money;
            }
            else {
                //패배
                printf("You lose!\n");
            }
        }
        if (money == 0) {
            //소지금 전부 소진 시
            Finish(true, money);
            break;
        }

        //재시작 부분
        if (true) continue;
        else {
            //사용자 종료 시 계산된 최종 배팅금을 파일에 저장
            Finish(false, money);
            break;
        }
    }
    return 0;
}

//덱 드로우 함수
int Draw(char* shape_pt, char* alpha_pt, char(*arr)[13], char* hand) {
    int value = 0;
    while (true) {
        srand(time(NULL));
        int shape = rand() % 4;
        srand(time(NULL));
        int card = rand() % 13;

        if (arr[shape][card] != ' ') {
            *alpha_pt = arr[shape][card];
            for (int i = 0; i < 10; i++) {
                if (hand[i] == ' ') {
                    hand[i] = arr[shape][card];
                    break;
                }
            }
            arr[shape][card] = ' ';
            switch (shape) {
            case 0:
                *shape_pt = 'S';
                break;
            case 1:
                *shape_pt = 'C';
                break;
            case 2:
                *shape_pt = 'D';
                break;
            default:
                *shape_pt = 'H';
                break;
            }
            switch (card) {
            case 0:
                value = 11;
                break;
            case 10:
                value = 10;
                break;
            case 11:
                value = 10;
                break;
            case 12:
                value = 10;
                break;
            default:
                value = card + 1;
                break;
            }
            break;
        }
    }
    return value;
}

//Dot Matrix로 카드 구현
void CardShow(char* shape, char* alpha) {
}

bool HandCheck(char* arr) {
    for (int i = 0; i < 10; i++) {
        if (arr[i] == 'A') {
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