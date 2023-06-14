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
#include <sys/stat.h>

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
    int bet_money = 0;
    unsigned char c;

    if((tactsw = open(tact,O_RDWR)) < 0) {
        perror("open");
        exit(1);
    }

    while (true) {
        read(tactsw, &c, sizeof(c));
        if (c)
            break;
    }
    close(tactsw);
    switch(c) {
        case 1:
        bet_money += 100;
        break;
        case 2:
        bet_money += 200;
        break;
        case 3:
        bet_money += 500;
        break;
        default:
        break;
    }

    return bet_money;
}

//덱 드로우 함수
int Draw(char* shape_pt, char* alpha_pt, unsigned char arr[4][13], char* hand) {
    int value = 0;
    char ten[3];
    while (true) {
        srand(time(NULL));
        int shape = rand() % 4;
        srand(time(NULL));
        int card = rand() % 13;
        srand(time(NULL));

        if (arr[shape][card] != ' ') {
            *alpha_pt = arr[shape][card];
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

            int i = 0;
            for (i; i < 16; i++) {
                if (hand[i] == ' ') {
                    if(arr[shape][card] == '0') {
                        sprintf(ten, "%c%d", *shape_pt, 10);
                        hand[i] = ten[0];
                        hand[i + 1] = ten[1];
                        hand[i + 2] = ten[2];
                    }
                    else {
                        hand[i] = *shape_pt;
                        hand[i + 1] = arr[shape][card];
                    }
                    break;
                }
            }
            arr[shape][card] = ' ';

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


//Dot Matrix로 유저의 카드 출력 구현
void CardShow(char shape, char alpha) {
    //dot_matrix에 띄울 문양
    unsigned char mtx[4][8] = {
        {0x66,0xff,0xff,0xff,0xff,0x7e,0x3c,0x18}, //하트
        {0x3c,0x7e,0xff,0xff,0xdb,0x99,0x18,0x18}, //스페이드
        {0x66,0xe7,0xe7,0x18,0x18,0xe7,0xe7,0x66}, //클로버
        {0x18,0x3c,0x7e,0xff,0xff,0x7e,0x3c,0x18} //다이아몬드
        };
    unsigned char mtn[13][8] = {
        {0x18,0x24,0x42,0x42,0x7E,0x42,0x42,0x42}, //A
        {0x3c,0x42,0x02,0x04,0x08,0x10,0x20,0x7e}, //2
        {0x3c,0x02,0x02,0x3c,0x02,0x02,0x02,0x3c}, //3
        {0x0c,0x14,0x24,0x44,0x7e,0x04,0x04,0x04}, //4
        {0x3e,0x20,0x20,0x3c,0x02,0x02,0x02,0x3c}, //5
        {0x3c,0x42,0x40,0x7c,0x42,0x42,0x42,0x3c}, //6
        {0x7e,0x42,0x42,0x42,0x02,0x02,0x02,0x02}, //7
        {0x3c,0x42,0x42,0x3c,0x42,0x42,0x42,0x3c}, //8
        {0x3c,0x42,0x42,0x42,0x3e,0x02,0x42,0x3c}, //9
        {0x9e,0xa1,0xa1,0xa1,0xa1,0xa1,0xa1,0x9e}, //10
        {0x3e,0x08,0x08,0x08,0x08,0x08,0x48,0x30}, //J
        {0x38,0x44,0x82,0x82,0x82,0x8A,0x44,0x3A}, //Q
        {0x42,0x44,0x48,0x70,0x50,0x48,0x44,0x42} //K
        };

    //예외처리
    int dot_mtx;
    if((dot_mtx = open(dot, O_RDWR)) < 0)
    {
        printf("Can't Open\n");
        exit(0);
    }
    int pos = Alpha_dot(alpha);
    //모양에 따라 dot matrix 출력
    switch (shape) {
        case 'S':
            write(dot_mtx, &mtx[1], sizeof(mtx[1])); //스페이드 출력
            usleep(2000000); //3초동안 점등
            write(dot_mtx, &mtn[pos], sizeof(mtn[pos])); usleep(2000000);
            break;
        case 'C':
            write(dot_mtx, &mtx[2], sizeof(mtx[2])); //클로버 출력
            usleep(2000000); //3초동안 점등
            write(dot_mtx, &mtn[pos], sizeof(mtn[pos])); usleep(2000000);
            break;
        case 'D':
            write(dot_mtx, &mtx[3], sizeof(mtx[3])); //다이아몬드 출력
            usleep(2000000); //3초동안 점등
            write(dot_mtx, &mtn[pos], sizeof(mtn[pos])); usleep(2000000);
            break;
        default:
            write(dot_mtx, &mtx[0], sizeof(mtx[0])); //하트 출력
            usleep(2000000); //3초동안 점등
            write(dot_mtx, &mtn[pos], sizeof(mtn[pos])); usleep(2000000);
            break;
    }
    close(dot_mtx);
}

void DealerCardShow(int score, char dealer_hand[16], int hitting) {
    int clcd_d;
    unsigned char buf[32] = "";
    unsigned char guide[16] = "";
    int i;
    bool ten = false;

    for(i = 0; i < 16; i++) {
        buf[i] = dealer_hand[i];
        if(dealer_hand[1] == '1') {
            ten = true;
        }
    }

    if((clcd_d = open(clcd,O_RDWR)) < 0){
        perror("open");
        exit(1);
    }
    switch(hitting) {
        case 0:
        sprintf(guide, "%s", "Card Drawing...");
        strcat(buf, guide);
        write(clcd_d, &buf, strlen(buf));
        close(clcd_d);
        break;
        case 1:
        if(ten) {
            buf[3] = ' ';
            buf[4] = ' ';
            buf[5] = ' ';
        }
        else {
            buf[2] = ' ';
            buf[3] = ' ';
            buf[4] = ' ';
        }
        sprintf(guide, "%d%s", score, " Hit or Stand");
        strcat(buf, guide);
        write(clcd_d, &buf, strlen(buf));
        close(clcd_d);
        break;
        case 2:
        sprintf(guide, "%s", "Betting...      100   200   500");
        write(clcd_d, &guide, strlen(guide));
        close(clcd_d);
        case 3:
        sprintf(guide, "%s", "All in!");
        write(clcd_d, &guide, strlen(guide));
        close(clcd_d);
        default:
        sprintf(guide, "%d %s", score, "Result...");
        strcat(buf, guide);
        write(clcd_d, &buf, strlen(buf));
        close(clcd_d);
        break;
    }
}

bool HitCheck() {
    int tactsw;
    unsigned char c = 0;
    bool check = false;

    if((tactsw = open(tact,O_RDWR)) < 0) {
        perror("open");
        exit(1);
    }

    while (true) {
        read(tactsw, &c, sizeof(c));
        if (c)
            break;
    }
    close(tactsw);
    switch(c) {
        case 1:
        check = true;
        break;
        case 2:
        check = false;
        break;
        default:
        break;
    }

    return check;
}

bool HandCheck(char* arr, int *cnt) {
    int i = 0;
    int count = 0;
    for (i = 0; i < 16; i++) {
        if (arr[i] == 'A') {
            count++;
        }
    }
    if(count > *cnt) {
        *cnt = *cnt + 1;
        return true;
    }
    else {
        return false;
    }
}

void ResultPrint(int score, char hand[16], int check) {
    int clcd_d;
    int i;
    unsigned char buf[32] = "";
    unsigned char guide[16] = "";
    bool ten = false;
    if((clcd_d = open(clcd,O_RDWR)) < 0) {
        perror("open");
        exit(1);
    }

    for(i = 0; i < 16; i++) {
        buf[i] = hand[i];
        if(hand[1] == '1') {
            ten = true;
        }
    }

    switch(check) {
        case 0:
        sprintf(guide, "%d %s", score, "You Win!");
        strcat(buf, guide);
        write(clcd_d, &buf, strlen(buf));
        sleep(3);
        break;
        case 1:
        sprintf(guide, "%s", "Push!");
        strcat(buf, guide);
        write(clcd_d, &buf, strlen(buf));
        sleep(3);
        break;
        case 2:
        sprintf(buf, "%s", "Game Over!");
        write(clcd_d, &buf, strlen(buf));
        sleep(3);
        break;
        case 3:
        if(ten) {
            buf[3] = ' ';
            buf[4] = ' ';
            buf[5] = ' ';
        }
        else {
            buf[2] = ' ';
            buf[3] = ' ';
            buf[4] = ' ';
        }
        sprintf(guide, "%s", "BlackJack!");
        strcat(buf, guide);
        write(clcd_d, &buf, strlen(buf));
        sleep(3);
        break;
        case 4:
        sprintf(guide, "%s", "You Lose!");
        strcat(buf, guide);
        write(clcd_d, &buf, strlen(buf));
        sleep(3);
        break;
        case 5:
        sprintf(buf, "%s", "                Retry?");
        write(clcd_d, &buf, strlen(buf));
        sleep(3);
        break;
        case 6:
        sprintf(buf, "%s", "                Bye");
        write(clcd_d, &buf, strlen(buf));
        sleep(3);
        break;
        case 7:
        if(ten) {
            buf[3] = ' ';
            buf[4] = ' ';
            buf[5] = ' ';
        }
        else {
            buf[2] = ' ';
            buf[3] = ' ';
            buf[4] = ' ';
        }
        sprintf(guide, "%s", "You Busted!");
        strcat(buf, guide);
        write(clcd_d, &buf, strlen(buf));
        sleep(3);
        break;        
        case 8:
        sprintf(buf, "%s", "                               ");
        write(clcd_d, &buf, strlen(buf));
        break;
        case 9:
        sprintf(buf, "%s", "Welcome to      Blackjack!!");
        write(clcd_d, &buf, strlen(buf)); 
        default:
        break;
    }
    close(clcd_d);
}

//게임 종료 or 오버 시 파일 처리
void Finish(bool over, int num) {
    FILE* file;
    int n;
    file = fopen("money.txt", "w");
    char fake[16];
    ResultPrint(n, fake, 8);

    if (over) {
        num = 900;
        fprintf(file, "%ld", num);
    }
    else {
        if(num > 9999) num = 9999;
        fprintf(file, "%ld", num);
    }
    fclose(file);
}

int main(void) {
    int money = 0;
    int bet_money = 0;
    bool user_busted = false;
    char alpha, dealer_alpha, shape, dealer_shape;
    char fake[16];
    FILE* file;

    ResultPrint(money, fake, 9);
    //배팅 금액이 저장된 파일 로드
    file = fopen("money.txt", "r");
    fscanf(file, "%d", &money);
    if(money > 9999) money = 9999;
    fclose(file);

    //게임 실행 반복문
    while (true) {
        //손패 초기화
        char user_hand[16] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
        char dealer_hand[16] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
        int user_score = 0;
        int dealer_score = 0;
        int user_card = 0;
        int dealer_card = 0;
        int user_ace_count = 0;
        int dealer_ace_count = 0;
        bool blackjack = false;
        bool dealer_bust = false;
        if(money > 9999) money = 9999;

        FND_control(money);
        //초기금액 3초간 표시
        //LCD로 배팅금 입력 부분 출력
        //딥 스위치로 배팅금 입력
        DealerCardShow(user_score, dealer_hand, 2);    
        bet_money = Betting(money);
        if (bet_money > money) {
            DealerCardShow(user_score, dealer_hand, 3);
            bet_money = money;
            money = 0;
        }
        else {
            money -= bet_money;
        }
        FND_control(bet_money);

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
        DealerCardShow(user_score, dealer_hand, 0);
        sleep(1);        
        CardShow(shape, alpha);
        sleep(1);

        //두 번째 카드 분배
        user_score += Draw(&shape, &alpha, deck, user_hand);
        dealer_score += Draw(&dealer_shape, &dealer_alpha, deck, dealer_hand);

        //두 번째 카드 출력(플레이어만)
        CardShow(shape, alpha);
        sleep(1);

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
            //블랙잭
            money += bet_money * 2;
            blackjack = true;
        }
        else {
            while (true) {
                DealerCardShow(user_score, dealer_hand, 1);    
                if (HitCheck()) {
                    user_score += Draw(&shape, &alpha, deck, user_hand);
                    CardShow(shape, alpha);
                    if (user_score == 21) {
                        blackjack = true;
                        break;
                    }
                    //힛 or 스탠드 구현
                    if (user_score > 21) {
                        if (HandCheck(user_hand, &user_ace_count)) user_score -= 10;
                        else {
                            user_busted = true;
                            break;
                        }
                    }
                }
                else break;
            }
        }

        if (user_busted) {
            ResultPrint(user_score, dealer_hand, 7);
            FND_control(money);  
            user_busted = false;
        }
        else if (blackjack == false && user_busted == false){
            //딜러가 가진 카드의 합이 16이하인 경우 딜러는 무조건 드로우
            while (dealer_score <= 16) {
                DealerCardShow(user_score, dealer_hand, 0);
                sleep(2);
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
                dealer_bust = false;
                //딜러 버스트 부분 LCD 구현
                ResultPrint(user_score, dealer_hand, 0);
                money += bet_money * 2;
                FND_control(money);
            }
            else if (user_score > dealer_score) {
                //플레이어 승리 부분 LCD 구현
                ResultPrint(user_score, dealer_hand, 0);
                money += bet_money * 2;
                FND_control(money);
            }
            else if (user_score == dealer_score) {
                //무승부 부분 LCD 구현
                ResultPrint(user_score, dealer_hand, 1);
                money += bet_money;
                FND_control(money);
            }
            else {
                //패배
                ResultPrint(user_score, dealer_hand, 4);
                FND_control(money);
            }
        }
        
        if (blackjack) {
            //블랙잭 부분 LCD 구현
            ResultPrint(user_score, dealer_hand, 3);
            FND_control(money);
        }

        if (money == 0) {
            ResultPrint(user_score, dealer_hand, 2);
            //소지금 전부 소진 시
            Finish(true, money);
            break;
        }

        ResultPrint(user_score, dealer_hand, 5);
        //재시작 부분
        if (HitCheck()) {
            continue;
        }
        else {
            //사용자 종료 시 계산된 최종 배팅금을 파일에 저장
            ResultPrint(user_score, dealer_hand, 6);
            Finish(false, money);
            break;
        }
    }
    return 0;
}