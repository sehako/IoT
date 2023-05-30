#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

#define clcd "/dev/clcd"
#define dip "/dev/dipsw"
#define dot "/dev/dot"
#define fnd "/dev/fnd"

long Betting(long); //배팅을 딥 스위치로 구현할 함수
int Draw(char*, char*, char(*)[13], char*);
void CardShow(char, char); //뽑은 카드를 Dot Matrix로 보여줄 함수
void DealerCardShow(char, char);
bool HandCheck(char*);
void ResultPrint(int);
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

    //fnd에 숫자를 띄우기위한 파일 로드 후 배열저장
    int user_money[4] = {0};
    file = fopen("money.txt", "r");
    fscanf(file, "%1d%1d%1d%1d",&user_money[0], &user_money[1], &user_money[2], &user_money[3]);
    fclose(file);

    //게임 실행 반복문
    while (true) {
        //손패 초기화
        char user_hand[10] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
        char dealer_hand[10] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
        user_score = 0;
        dealer_score = 0;

        //초기금액 3초간 표시
        FND_control(user_money,3);
        //LCD로 배팅금 입력 부분 출력
        //딥 스위치로 배팅금 입력
        // bet_money = Betting(money);
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
        DealerCardShow(dealer_shape, dealer_alpha);        
        CardShow(shape, alpha);

        //두 번째 카드 분배
        user_score += Draw(&shape, &alpha, deck, user_hand);
        dealer_score += Draw(&dealer_shape, &dealer_alpha, deck, dealer_hand);

        //두 번째 카드 출력(플레이어만)
        CardShow(shape, alpha);

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
                    if (HandCheck(dealer_hand)) dealer_score -= 10;
                    else {
                        dealer_bust = true;
                        break;
                    }
                }
            }

            //승패 계산
            if (dealer_bust) {
                //딜러 버스트 부분 LCD 구현
                ResultPrint(0);
                money += bet_money * 2;
            }
            else if (user_score > dealer_score) {
                //플레이어 승리 부분 LCD 구현
                ResultPrint(0);
                money += bet_money * 2;
            }
            else if (user_score == dealer_score) {
                //무승부 부분 LCD 구현
                ResultPrint(1);
                money += bet_money;
            }
            else {
                //패배
                ResultPrint(3);
                printf("You lose!\n");
            }
        }
        if (money == 0) {
            ResultPrint(2);
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

long Betting(long money) {
    long bet_money;
    int dip_d, clcd_d;
    unsigned char c;
    char buf[10] = "\nBetting...";
    if((dip_d = open(dip,O_RDWR)) < 0) {
        perror("open");
        exit(1);
    }
    if((clcd_d = open(clcd,O_RDWR)) < 0){
        perror("open");
        exit(1);
    }


    while(true) {
        while(true) {
            read(dip_d,&c,sizeof(c));
            if(c) break;
        }

        printf("%d\n",c);
        close(dip_d);
        close(clcd_d);
    }
    return bet_money;
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
    ;}

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
            usleep(1000000); //1초동안 점등
            switch (alpha) {
                case 'A': write(dot_mtx, &mtn[0], sizeof(mtn[0])); usleep(100000); break;
                case '2': write(dot_mtx, &mtn[1], sizeof(mtn[1])); usleep(100000); break;
                case '3': write(dot_mtx, &mtn[2], sizeof(mtn[2])); usleep(100000); break;
                case '4': write(dot_mtx, &mtn[3], sizeof(mtn[3])); usleep(100000); break;
                case '5': write(dot_mtx, &mtn[4], sizeof(mtn[4])); usleep(100000); break;
                case '6': write(dot_mtx, &mtn[5], sizeof(mtn[5])); usleep(100000); break;
                case '7': write(dot_mtx, &mtn[6], sizeof(mtn[6])); usleep(100000); break;
                case '8': write(dot_mtx, &mtn[7], sizeof(mtn[7])); usleep(100000); break;
                case '9': write(dot_mtx, &mtn[8], sizeof(mtn[8])); usleep(100000); break;
                case '0': write(dot_mtx, &mtn[9], sizeof(mtn[9])); usleep(100000); break;
                case 'J': write(dot_mtx, &mtn[10], sizeof(mtn[10])); usleep(100000); break;
                case 'Q': write(dot_mtx, &mtn[11], sizeof(mtn[11])); usleep(100000); break;
                case 'K': write(dot_mtx, &mtn[12], sizeof(mtn[12])); usleep(100000); break;
            }
            break;
        case 'C':
            write(dot_mtx, &mtx[2], sizeof(mtx[2])); //클로버 출력
            usleep(1000000); //1초동안 점등
            switch (alpha) {
                case 'A': write(dot_mtx, &mtn[0], sizeof(mtn[0])); usleep(100000); break;
                case '2': write(dot_mtx, &mtn[1], sizeof(mtn[1])); usleep(100000); break;
                case '3': write(dot_mtx, &mtn[2], sizeof(mtn[2])); usleep(100000); break;
                case '4': write(dot_mtx, &mtn[3], sizeof(mtn[3])); usleep(100000); break;
                case '5': write(dot_mtx, &mtn[4], sizeof(mtn[4])); usleep(100000); break;
                case '6': write(dot_mtx, &mtn[5], sizeof(mtn[5])); usleep(100000); break;
                case '7': write(dot_mtx, &mtn[6], sizeof(mtn[6])); usleep(100000); break;
                case '8': write(dot_mtx, &mtn[7], sizeof(mtn[7])); usleep(100000); break;
                case '9': write(dot_mtx, &mtn[8], sizeof(mtn[8])); usleep(100000); break;
                case '0': write(dot_mtx, &mtn[9], sizeof(mtn[9])); usleep(100000); break;
                case 'J': write(dot_mtx, &mtn[10], sizeof(mtn[10])); usleep(100000); break;
                case 'Q': write(dot_mtx, &mtn[11], sizeof(mtn[11])); usleep(100000); break;
                case 'K': write(dot_mtx, &mtn[12], sizeof(mtn[12])); usleep(100000); break;
            }
            break;
        case 'D':
            write(dot_mtx, &mtx[3], sizeof(mtx[3])); //다이아몬드 출력
            usleep(1000000); //1초동안 점등
            switch (alpha) {
                case 'A': write(dot_mtx, &mtn[0], sizeof(mtn[0])); usleep(100000); break;
                case '2': write(dot_mtx, &mtn[1], sizeof(mtn[1])); usleep(100000); break;
                case '3': write(dot_mtx, &mtn[2], sizeof(mtn[2])); usleep(100000); break;
                case '4': write(dot_mtx, &mtn[3], sizeof(mtn[3])); usleep(100000); break;
                case '5': write(dot_mtx, &mtn[4], sizeof(mtn[4])); usleep(100000); break;
                case '6': write(dot_mtx, &mtn[5], sizeof(mtn[5])); usleep(100000); break;
                case '7': write(dot_mtx, &mtn[6], sizeof(mtn[6])); usleep(100000); break;
                case '8': write(dot_mtx, &mtn[7], sizeof(mtn[7])); usleep(100000); break;
                case '9': write(dot_mtx, &mtn[8], sizeof(mtn[8])); usleep(100000); break;
                case '0': write(dot_mtx, &mtn[9], sizeof(mtn[9])); usleep(100000); break;
                case 'J': write(dot_mtx, &mtn[10], sizeof(mtn[10])); usleep(100000); break;
                case 'Q': write(dot_mtx, &mtn[11], sizeof(mtn[11])); usleep(100000); break;
                case 'K': write(dot_mtx, &mtn[12], sizeof(mtn[12])); usleep(100000); break;
            }
            break;
        default:
            write(dot_mtx, &mtx[0], sizeof(mtx[0])); //하트 출력
            usleep(1000000); //1초동안 점등
            switch (alpha) {
                case 'A': write(dot_mtx, &mtn[0], sizeof(mtn[0])); usleep(100000); break;
                case '2': write(dot_mtx, &mtn[1], sizeof(mtn[1])); usleep(100000); break;
                case '3': write(dot_mtx, &mtn[2], sizeof(mtn[2])); usleep(100000); break;
                case '4': write(dot_mtx, &mtn[3], sizeof(mtn[3])); usleep(100000); break;
                case '5': write(dot_mtx, &mtn[4], sizeof(mtn[4])); usleep(100000); break;
                case '6': write(dot_mtx, &mtn[5], sizeof(mtn[5])); usleep(100000); break;
                case '7': write(dot_mtx, &mtn[6], sizeof(mtn[6])); usleep(100000); break;
                case '8': write(dot_mtx, &mtn[7], sizeof(mtn[7])); usleep(100000); break;
                case '9': write(dot_mtx, &mtn[8], sizeof(mtn[8])); usleep(100000); break;
                case '': write(dot_mtx, &mtn[9], sizeof(mtn[9])); usleep(100000); break;
                case 'J': write(dot_mtx, &mtn[10], sizeof(mtn[10])); usleep(100000); break;
                case 'Q': write(dot_mtx, &mtn[11], sizeof(mtn[11])); usleep(100000); break;
                case 'K': write(dot_mtx, &mtn[12], sizeof(mtn[12])); usleep(100000); break;
            }
            break;
    }
    close(dot_mtx);
}

// 승패 여부에 따라서 잔고의 100의 자리수를 조절해주는 함수
/*
void calculate_user_money(rps_state){
	if (rps_state == 1) {	// 승리했을경우 배팅금액 더해주기
		user_money[1] += led_count;
	}
	else if (rps_state == -1) {	// 패배했을경우 배팅금액 빼주기
		user_money[1] -= led_count;
	}
	else { printf("unknown Error");	}
} */

// fnd에 한 자리수의 양수로 출력할 수 있도록, user_money 배열 조정해주는 함수
void adjust_user_money(int money[]){
	if (money[1] >= 10) {  // 100의 자리수가 10보다 커질때
		money[0]++; // 1000의 자리수 올려주기
		money[1] %= 10; // 나머지값을 100의 자릿수에 대입
	}
	else if (money[1] < 0) { // 100의 자리수가 0보다 작아질때
		money[0]--; // 1000의 자릿수 내려주기
		money[1] = (10 + money[1]); // 100의 자릿수 조정
	}
	else{ /*pass*/ }
}

// 세그먼트 제어 함수
int FND_control(long money){
	unsigned char FND_DATA_TBL[]={
        	0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,
        	0x83,0xC6,0xA1,0x86,0x8E,0xC0,0xF9,0xA4,0xB0,0x99,0x89
	};

	int fnd_fd = 0;
    unsigned char fnd_num[4];

	// money 배열의 원소들을 순서에 맞게 넣어주기
    fnd_num[0] = FND_DATA_TBL[money[0]];
    fnd_num[1] = FND_DATA_TBL[money[1]];
    fnd_num[2] = FND_DATA_TBL[money[2]];
    fnd_num[3] = FND_DATA_TBL[money[3]];

    fnd_fd = open(fnd, O_RDWR);
	if(fnd_fd <0){ printf("fnd error\n"); } // 예외처리

    write(fnd_fd, &fnd_num, sizeof(fnd_num)); // 출력
    sleep(time_sleep); // 점등시간 조절

    close(fnd_fd);
}

void DealerCardShow(char shape, char alpha) {
    int clcd_d;
    unsigned char buf[16];
    unsigned char temp[2];

    strcat(temp, shape);
    strcat(temp, alpha);
    strcat(buf, temp);

    if((clcd_d = open(clcd,O_RDWR)) < 0){
        perror("open");
        exit(1);
    }
    scanf("%s", buf);
    write(clcd_d, &buf, strlen(buf));
    close(clcd_d);
}

bool HandCheck(char* arr) {
    int i = 0;
    for (i = 0; i < 10; i++) {
        if (arr[i] == 'A') {
            arr[i] = ' ';
            return true;
        }
        else return false;
    }
}


void ResultPrint(int check) {
    int clcd_d;
    unsigned char buf[16] = "";
    if((clcd_d = open(clcd,O_RDWR)) < 0) {
        perror("open");
        exit(1);
    }

    switch(check) {
        case 0:
        write(clcd_d, &buf, strlen(buf));
        buf = "You Win!";
        break;
        case 1:
        buf = "Push!";
        write(clcd_d, &buf, strlen(buf));
        break;
        case 2:
        buf = "Game Over!";
        write(clcd_d, &buf, strlen(buf));
        default:
        buf = "You Lose!";
        write(clcd_d, &buf, strlen(buf));
        break;
    }

    close(clcd_d);
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