#ifndef _KMEM_KMEM_H
#define _KMEM_KMEM_H

void printmem(void *m, int sz);
int rkm(int fd, int offset, void *buf, int size);
int wkm(int fd, int offset, void *buf, int size);

#endif