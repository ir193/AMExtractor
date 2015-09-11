#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

void printmem(void *m, int sz);
int rkm(int fd, int offset, void *buf, int size);
int wkm(int fd, int offset, void *buf, int size);

void printmem(void *m, int sz){
    unsigned char *p = (unsigned char *)m;
    int i;

    for (i = 0; i < sz; i++){
        printf("%02x ", p[i]);
        if((i+1) % 16 == 0){
            printf("\n");
        }
    }

    printf("\n");
}

/* read data from kmem */
int rkm(int fd, int offset, void *buf, int size)
{
        if (lseek(fd, offset, 0) != offset){
            perror("seek error\n");
            exit(2);
        }
        if (read(fd, buf, size) != size){
            perror("read error");
            exit(2);
        }
        return size;
}

/* write data to kmem */
int wkm(int fd, int offset, void *buf, int size)
{
        if (lseek(fd, offset, 0) != offset){
            perror("seek error\n");
            exit(2);
        }
        if (write(fd, buf, size) != size){
            perror("write error\n");
            exit(2);
        }
        return size;
}