
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define dip "/dev/dipsw"

int main()
{
    int dip_d;
    unsigned char c;

    if((dip_d = open(dip,O_RDWR)) < 0) {
        perror("open");
        exit(1);
    }

    while(1) {
        while(1) {
            read(dip_d,&c,sizeof(c));
            if(c != 0) {
                printf("%d\n",c);
            }
            if(c)
                break;
        }

        close(dip_d);
        return 0;
    }
}