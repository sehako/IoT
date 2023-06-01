#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <asm/ioctls.h>
#include <time.h>
#include <stdbool.h>
#include<sys/stat.h> 

#define clcd "/dev/clcd"
#define tact "/dev/tactsw"
#define dot "/dev/dot"
#define fnd "/dev/fnd"

// 세그먼트 제어 함수
int FND_control(int money){
 	unsigned char FND_DATA_TBL[]={~0x3f,~0x06,~0x5b,~0x4f,~0x66,~0x6d,~0x7d,~0x07,~0x7f,~0x67,~0x00};

 	int fnd_fd = 0;
    unsigned char fnd_num[4];

    int user_money[4] = {0, 0, 0, 0};

    user_money[0] = money / 1000;
    user_money[1] = (money % 1000) / 100;
    user_money[2] = (money % 100) / 10;
    user_money[3] = money % 10;

    fnd_num[0] = FND_DATA_TBL[user_money[0]];
    fnd_num[1] = FND_DATA_TBL[user_money[1]];
    fnd_num[2] = FND_DATA_TBL[user_money[2]];
    fnd_num[3] = FND_DATA_TBL[user_money[3]];
 	// money 배열의 원소들을 순서에 맞게 넣어주기

    fnd_fd = open(fnd, O_RDWR);
 	if(fnd_fd <0){ printf("fnd error\n"); } // 예외처리

    write(fnd_fd, &fnd_num, sizeof(fnd_num)); // 출력
    sleep(3);

    close(fnd_fd);
}


int Betting(int money) {
    int tactsw;
    int clcd_d;
    int bet_money = 0;
    char guide[32] = "";
    unsigned char c;

    if((tactsw = open(tact,O_RDWR)) < 0) {
        perror("open");
        exit(1);
    }

    if((clcd_d = open(clcd,O_RDWR)) < 0) {
        perror("open");
        exit(1);
    }

    sprintf(guide, "%s", "Betting...");
    write(clcd_d, &guide, strlen(guide));
    close(clcd_d);
    while(true) {
        switch(c) {
        case 0:
        case 1:
        bet_money += 100;
        FND_control(bet_money);
        break;
        case 2:
        bet_money += 200;
        FND_control(bet_money);
        break;
        case 3:
        bet_money += 500;
        FND_control(bet_money);
        break;
        }
    }


    close(tactsw);
    return bet_money;
}

int Alpha_dot(char alphabet) {
    int alpha_pos = 0;
    switch (alphabet) {
    case 'A': alpha_pos = 0; break;
    case '2': alpha_pos = 1; break;
    case '3': alpha_pos = 2; break;
    case '4': alpha_pos = 3; break;
    case '5': alpha_pos = 4; break;
    case '6': alpha_pos = 5; break;
    case '7': alpha_pos = 6; break;
    case '8': alpha_pos = 7; break;
    case '9': alpha_pos = 8; break;
    case '0': alpha_pos = 9; break;
    case 'J': alpha_pos = 10; break;
    case 'Q': alpha_pos = 11; break;
    case 'K': alpha_pos = 12; break;
    }
    return alpha_pos;
}

//덱 드로우 함수
int Draw(char* shape_pt, char* alpha_pt, unsigned char arr[4][13], char* hand) {
    int value = 0;
    while (true) {
        srand(time(NULL));
        int shape = rand() % 4;
        srand(time(NULL));
        int card = rand() % 13;

        if (arr[shape][card] != ' ') {
            *alpha_pt = arr[shape][card];
            int i = 0;
            for (i = 0; i < 10; i++) {
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

//Dot Matrix로 유저의 카드 출력 구현
void CardShow(char shape, char alpha) {
    //dot_matrix에 띄울 문양
    unsigned char mtx[4][8] = {
        {0x00,0x66,0xFF,0xFF,0x7E,0x3C,0x18,0x00}, //하트
        {0x00,0x08,0x1C,0x3E,0x7F,0x3E,0x1C,0x08}, //스페이드
        {0x18,0x00,0x18,0x3C,0x3C,0x18,0x00,0x00}, //클로버
        {0x00,0x08,0x1C,0x3E,0x1C,0x08,0x00,0x00} //다이아몬드
        };
    unsigned char mtn[13][8] = {
        {0x18,0x24,0x42,0x42,0x7E,0x42,0x42,0x42}, //A
        {0x18,0x24,0x24,0x04,0x08,0x10,0x3c,0x00}, //2
        {0x18,0x24,0x04,0x18,0x04,0x24,0x18,0x00}, //3
        {0x08,0x18,0x28,0x48,0xfc,0x08,0x08,0x00}, //4
        {0x2c,0x20,0x20,0x18,0x04,0x24,0x18,0x00}, //5
        {0x18,0x24,0x20,0x38,0x24,0x24,0x18,0x00}, //6
        {0x3c,0x04,0x04,0x08,0x10,0x10,0x10,0x00}, //7
        {0x18,0x24,0x24,0x18,0x24,0x24,0x18,0x00}, //8
        {0x18,0x24,0x24,0x1c,0x04,0x04,0x18,0x00}, //9
        {0x26,0x69,0xa9,0x29,0x29,0x29,0x76,0x00}, //10
        {0x1C,0x08,0x08,0x08,0x08,0x48,0x48,0x30},  //J
        {0x38,0x44,0x82,0x82,0x82,0x8A,0x44,0x3A},  //Q
        {0x44,0x48,0x50,0x60,0x50,0x48,0x44,0x44}  //K
        };

    //예외처리
    int dot_mtx;
    if((dot_mtx = open(dot, O_RDWR)) < 0)
    {
        printf("Can't Open\n");
        exit(0);
    }

    //모양에 따라 dot matrix 출력
    switch (shape) {
        case 'S':
            write(dot_mtx, &mtx[1], sizeof(mtx[1])); //스페이드 출력
            usleep(3000000); //1초동안 점등
            write(dot_mtx, &mtn[0], sizeof(mtn[alpha_dot(alpha)])); usleep(300000);
            break;
        case 'C':
            write(dot_mtx, &mtx[2], sizeof(mtx[2])); //클로버 출력
            usleep(3000000); //1초동안 점등
            write(dot_mtx, &mtn[0], sizeof(mtn[alpha_dot(alpha)])); usleep(300000);
            break;
        case 'D':
            write(dot_mtx, &mtx[3], sizeof(mtx[3])); //다이아몬드 출력
            usleep(3000000); //1초동안 점등
            write(dot_mtx, &mtn[0], sizeof(mtn[alpha_dot(alpha)])); usleep(300000);
            break;
        default:
            write(dot_mtx, &mtx[0], sizeof(mtx[0])); //하트 출력
            usleep(3000000); //1초동안 점등
            write(dot_mtx, &mtn[0], sizeof(mtn[alpha_dot(alpha)])); usleep(300000);
            break;
    }
    close(dot_mtx);
}

void DealerCardShow(char dealer_hand[10], int hitting) {
    int clcd_d;
    unsigned char buf[32];
    unsigned char guide[16];
    unsigned char temp[2];
    int i;

    if((clcd_d = open(clcd,O_RDWR)) < 0){
        perror("open");
        exit(1);
    }
    
    sprintf(buf, "%s", dealer_hand);
    switch(hitting) {
        case 0:
        for(i = 0; i < 2; i++) {
        temp[i] = dealer_hand[i];
        }
        sprintf(buf, "%s", temp);
        sprintf(guide, "%s", "Card Drawing");
        strcat(buf, guide);
        write(clcd_d, &buf, strlen(buf));
        break;
        case 1:
        for(i = 0; i < 2; i++) {
        temp[i] = dealer_hand[i];
        }
        sprintf(buf, "%s", temp);
        sprintf(guide, "%s", "Hit or Stand");
        strcat(buf, guide);
        write(clcd_d, &buf, strlen(buf));
        break;
        default:
        sprintf(buf, "%s", dealer_hand);
        sprintf(guide, "%s", "Result...");
        strcat(buf, guide);
        write(clcd_d, &buf, strlen(buf));
        break;
    }
    close(clcd_d);
}

bool HandCheck(char* arr, int *cnt) {
    int i = 0;
    int count = 0;
    for (i = 0; i < 10; i++) {
        if (arr[i] == 'A') {
            count++;
        }
    }
    if(count > *cnt) {
        *cnt++;
        return true;
    }
    else {
        return false;
    }
}

void ResultPrint(char hand[10], int check) {
    int clcd_d;
    unsigned char buf[32] = "";
    sprintf(buf, "%s", hand);
    unsigned char guide[16] = "";
    if((clcd_d = open(clcd,O_RDWR)) < 0) {
        perror("open");
        exit(1);
    }

    switch(check) {
        case 0:
        sprintf(guide, "%s", "\nYou Win!");
        strcat(buf, guide);
        write(clcd_d, &buf, strlen(buf));
        break;
        case 1:
        sprintf(guide, "%s", "\nPush!");
        strcat(buf, guide);
        write(clcd_d, &buf, strlen(buf));
        break;
        case 2:
        sprintf(guide, "%s", "\nGame Over!");
        strcat(buf, guide);
        write(clcd_d, &buf, strlen(buf));
        case 3:
        sprintf(guide, "%s", "\nBlackJack!");
        strcat(buf, guide);
        write(clcd_d, &buf, strlen(buf));
        default:
        sprintf(guide, "%s", "\nYou Lose!");
        strcat(buf, guide);
        write(clcd_d, &buf, strlen(buf));
        break;
    }
    close(clcd_d);
}

//게임 종료 or 오버 시 파일 처리
void Finish(bool over, int num) {
    FILE* file;
    file = fopen("money.txt", "w");

    if (over) {
        num = 5000;
        fprintf(file, "%ld", num);
        printf("GameOver!\n");
    }
    else {
        if(num > 9999) num = 9999;
        fprintf(file, "%ld", num);
        printf("GameFinished\n");
    }
    fclose(file);
}

int main(void) {
    int money = 0;
    int bet_money = 0;
    bool user_busted = false;
    char alpha, dealer_alpha, shape, dealer_shape;
    FILE* file;

    //배팅 금액이 저장된 파일 로드
    file = fopen("money.txt", "r");
    fscanf(file, "%d", &money);
    if(money > 9999) money = 9999;
    fclose(file);

    //게임 실행 반복문
    while (true) {
        //손패 초기화
        char user_hand[10] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
        char dealer_hand[10] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
        int user_score = 0;
        int dealer_score = 0;
        int user_card = 0;
        int dealer_card = 0;
        int user_ace_count = 0;
        int dealer_ace_count = 0;
        if(money > 9999) money = 9999;

        FND_control(money);
        //초기금액 3초간 표시
        //LCD로 배팅금 입력 부분 출력
        //딥 스위치로 배팅금 입력
        bet_money = Betting(money);
        money -= bet_money;

        //행 부분은 카드 문양 열 부분은 카드 값
        unsigned char deck[4][13] = { 
            {'A', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'Q', 'K', 'J'},
            {'A', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'Q', 'K', 'J'},
            {'A', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'Q', 'K', 'J'},
            {'A', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'Q', 'K', 'J'} };
    

        //첫 번째 카드 분배
        user_score += Draw(&shape, &alpha, deck, user_hand);
        dealer_score += Draw(&dealer_shape, &dealer_alpha, deck, dealer_hand);


        //첫 번째 카드 출력(딜러는 LCD, 플레이어는 Dot Matrix)
        DealerCardShow(dealer_hand, 0);        
        CardShow(shape, alpha);

        //두 번째 카드 분배
        user_score += Draw(&shape, &alpha, deck, user_hand);
        dealer_score += Draw(&dealer_shape, &dealer_alpha, deck, dealer_hand);

        //두 번째 카드 출력(플레이어만)
        CardShow(shape, alpha);

        //초반 시작 부분에서 에이스 두 장 뽑으면 자동으로 11, 1로 취급
        if (user_score > 21) {
            HandCheck(user_hand, &user_ace_count);
            user_score -= 10;
        }
        if (dealer_score > 21) {
            HandCheck(dealer_hand, &dealer_ace_count);
            user_score -= 10;
        }

        if (user_score == 21) {
            ResultPrint(dealer_hand, 3);
            money += bet_money * 2;
            continue;
        }
        else {
            while (true) {
                
                //힛 or 스탠드 구현
                if (user_score > 21) {
                    if (HandCheck(user_hand, &user_ace_count)) user_score -= 10;
                    else {
                        user_busted = true;
                        break;
                    }
                }
                if (true) {
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
                    if (HandCheck(dealer_hand, &dealer_ace_count)) dealer_score -= 10;
                    else {
                        dealer_bust = true;
                        break;
                    }
                }
            }

            //승패 계산
            if (dealer_bust) {
                //딜러 버스트 부분 LCD 구현
                ResultPrint(dealer_hand, 0);
                money += bet_money * 2;
            }
            else if (user_score > dealer_score) {
                //플레이어 승리 부분 LCD 구현
                ResultPrint(dealer_hand, 0);
                money += bet_money * 2;
            }
            else if (user_score == dealer_score) {
                //무승부 부분 LCD 구현
                ResultPrint(dealer_hand, 1);
                money += bet_money;
            }
            else {
                //패배
                ResultPrint(dealer_hand, 3);
                printf("You lose!\n");
            }
        }
        if (money == 0) {
            ResultPrint(dealer_hand, 2);
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

