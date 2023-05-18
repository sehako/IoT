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
    int arr[4][13]; //�� �κ��� ī�� ���� �� �κ��� ī�� ��
    char alpha, alphabet, shape, dealer_shape;

    bool win; //�¸� ����
    bool loop = true; //�⺻������ �ݺ� ����
    FILE* file;

    //���� �ݾ��� ����� ���� �ε�
    file = fopen("money.txt", "r");
    fscanf(file, "%ld", &money);
    fclose(file);

    //���� ���� �κ�
    while (loop) {
        DeckGen(&arr); //�� ���� �Լ�
        int user_card = Draw(&shape, &alpha, arr);
        int dealer_card = Draw(&dealer_shape, &alphabet, arr);

        printf("%c %c\n", shape, dealer_shape);
        printf("%d %d\n", user_card, dealer_card);
        printf("%c %c\n", alpha, alphabet);

        // deck_gen(deck); �� ���� �Լ�
        //�ش� �Է��� ���̾� ��ư���� ���� ����
        scanf("%ld", &bet_money);

        //���� �ݾ� ��� �Լ�
        Wallet(false, bet_money, &money);

        //���ñ� ���� ���� �� ���� ���� ��� �� ���ñ� 5000�� ���� & ����
        if (money == 0) {
            printf("GameOver!\n");
            Finish(true, money);
            break;
        }

        //����� ���� �� ���� ���� ���ñ��� ���Ͽ� ����
        if (bet_money == 0) {
            Finish(false, money);
            break;
        }
    }

    //��ü �� ��� �׽�Ʈ
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 13; j++) {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
    return 0;
}

// ���п� ���� ���ñ� ó��
void Wallet(bool game, long betting, long* result) {
    if (game) {
        betting *= 2;
        *result += betting;
    }
    else {
        *result -= betting;
    }
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