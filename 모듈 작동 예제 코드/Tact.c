
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define tact "/dev/tactsw"

int main()
{
    unsigned char c;
    int tactsw;
    if((tactsw = open(tact,O_RDWR)) < 0) {  
    perror("open");
    exit(1);
    }
    
    while(1) {
        read(tactsw, &c, sizeof(c));
        printf("%c %d", c, c);
        close(tactsw);
    }
}