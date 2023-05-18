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
    int arr[4][13]; //�� �κ��� ī�� ���� �� �κ��� ī�� ��
    int play_check = 1;
    int hit_check = 1;
    int user_card, dealer_card;
    char alpha, alphabet, shape, dealer_shape;

    bool win; //�¸� ����
    bool loop = true; //�⺻������ �ݺ� ����
    FILE* file;

    //���� �ݾ��� ����� ���� �ε�
    file = fopen("money.txt", "r");
    fscanf(file, "%ld", &money);
    fclose(file);

    printf("%ld\n", money);

    //���� ���� �κ�
    while (true) {
        int user_score = 0;
        int dealer_score = 0;

        DeckGen(&arr); //�� ���� �Լ�

        //�ش� �Է��� ���̾� ��ư���� ���� ����
        printf("���ñ� �Է�(������: %ld)\n", money);
        scanf("%ld", &bet_money);

        // ó�� ���� ���� �� ī�� �й� ����
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

        //���� üũ �Լ�
        if (CheckBlackJack(user_score)) {
            printf("%ld\n", user_score);
            printf("BlackJack!!\n");
            money += bet_money * 2;
            continue;
        }
        //Ǫ�� äũ �κ�
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

        //������ ���� ī���� ���� 16������ ��� ��ο��ϴ� ��Ģ ����
        while (dealer_score <= 16) {
            dealer_score += Draw(&dealer_shape, &alphabet, arr);
        }

        //���� ��� �κ�
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

        //���ñ� ���� ���� �� ���� ���� ��� �� ���ñ� 5000�� ���� & ����
        if (money == 0) {
            printf("GameOver!\n");
            Finish(true, money);
            break;
        }

        //����� ���� �� ���� ���� ���ñ��� ���Ͽ� ����
        printf("�絵��? (0�Է� �� ����)\n");
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

//���� ���� or ���� �� ���� ó��
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

//�� ���� �Լ�
void DeckGen(int arr[4][13]) {
    for (int i = 0; i < 4; i++) {
        int count = 1;
        for (int j = 0; j < 13; j++) {
            arr[i][j] = count++;
        }
    }
}

//ī�� ��ο� �Լ� ����
int Draw(char* shape_pt, char* alpha_pt, int arr[4][13]) {
    int draw = 0;
    srand(time(NULL)); //���� ���� ����
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