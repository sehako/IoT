#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

int main(void) {
    long money;
    FILE* file;
    file = fopen("money.txt", "r");
    fscanf(file, "%ld", &money);
    fclose(file);

    return 0;
}