#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <asm/ioctls.h>
#include <time.h>

#define tact "/dev/tactsw"

int main()
{
    unsigned char c;
    int tactsw;

    if((tactsw = open(tact, O_RDWR)) < 0) {  
        perror("open");
        exit(1);
    }
    
    while (1)
    {
        read(tactsw, &c, sizeof(c));
        if (c)
            break;
    }
    switch(c) {
        case 1:
        printf("1\n");
        break;
        case 2:
        printf("2\n");
        break;
        case 3:
        printf("3\n");
        break;
        default:
        printf("Bye\n");
    }
    printf("%c %d\n", c);
    close(tactsw);
}