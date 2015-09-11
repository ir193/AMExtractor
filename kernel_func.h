#ifndef _KMEM_FUNC_H
#define _KMEM_FUNC_H
#include "kernel_struct.h"

#define DBG(fmt, args...) do { _printk("[KMEM] "fmt"\n", ## args); } while (0)


extern unsigned int pfn_offset;

extern int (*_printk)(const char *fmt, ...);
extern int (*_sock_create_kern)(int family, int type, int proto, struct socket **res);

extern struct resource *_iomem_resource;
extern struct file_operations *_zero_fops;
extern struct file_operations *_ptmx_fops;

extern int (*_sock_setsockopt)(struct socket *sock, int level, int op,
                     char *optval, unsigned int optlen);

extern int (*_kernel_sendmsg)(struct socket *sock, struct msghdr *msg, struct kvec *vec,
                    size_t num, size_t len);

extern void * (*_kmap)(struct page *page);
extern void (*_kunmap)(struct page *page);
extern struct page **_mem_map;
extern int (*_pfn_valid)(unsigned long pfn);
extern void* (*_kmap_atomic_pfn)(unsigned long pfn);
extern void (*__kunmap_atomic)(void *kvaddr);
extern struct mem_section **_mem_section;
#endif