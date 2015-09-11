#ifndef _KMEM_KERNEL_H
#define _KMEM_KERNEL_H

#include <linux/net.h>
#include <netinet/in.h>

#include "config.h"

struct page{
    char pad[STRUCT_PAGE_SIZE];
};

typedef unsigned int resource_size_t;
typedef unsigned long mm_segment_t;

#define PAGE_SHIFT              12
#define PAGE_SIZE               (1UL << PAGE_SHIFT)
#define KERNEL_DS 0x00000000

struct resource {
        resource_size_t start;
        resource_size_t end;
        const char *name;
        unsigned long flags;
        struct resource *parent, *sibling, *child;
};

struct file_operations {
    struct module *owner;
    loff_t *llseek;
    ssize_t *read;
    ssize_t *write;
    ssize_t *aio_read;
    ssize_t *aio_write;
    int *readdir;
    unsigned int *poll;
    long *unlocked_ioctl;
    long *compat_ioctl;
    int *mmap;
    int *open;
    int *flush;
    int *release;
    int *fsync ;
    int *aio_fsync;
    int *fasync;
    int *lock;
    /* ... */
};


#define kmemcheck_bitfield_begin(name)  \
        int name##_begin[0];
#define kmemcheck_bitfield_end(name)    \
        int name##_end[0];

struct socket {
        socket_state            state;
        kmemcheck_bitfield_begin(type);
        short                   type;
        kmemcheck_bitfield_end(type);
        unsigned long           flags;
        void                    *wq;
        void                    *file;
        void                    *sk;
        const struct proto_ops  *ops;
};

struct proto_ops {
        int             family;
        struct module   *owner;
        int             (*release)   (struct socket *sock);
        int             (*bind)      (struct socket *sock,
                                      struct sockaddr *myaddr,
                                      int sockaddr_len);
        int             (*connect)   (struct socket *sock,
                                      struct sockaddr *vaddr,
                                      int sockaddr_len, int flags);
        int             (*socketpair)(struct socket *sock1,
                                      struct socket *sock2);
        int             (*accept)    (struct socket *sock,
                                      struct socket *newsock, int flags);
        int             (*getname)   (struct socket *sock,
                                      struct sockaddr *addr,
                                      int *sockaddr_len, int peer);
        unsigned int    (*poll)      (void *file, struct socket *sock,
                                      struct poll_table_struct *wait);
        int             (*ioctl)     (struct socket *sock, unsigned int cmd,
                                      unsigned long arg);
#ifdef CONFIG_COMPAT
        int             (*compat_ioctl) (struct socket *sock, unsigned int cmd,
                                      unsigned long arg);
#endif
        int             (*listen)    (struct socket *sock, int len);
        int             (*shutdown)  (struct socket *sock, int flags);
        int             (*setsockopt)(struct socket *sock, int level,
                                      int optname, char __user *optval, unsigned int optlen);
        int             (*getsockopt)(struct socket *sock, int level,
                                      int optname, char __user *optval, int __user *optlen);

/*.....*/

    };

struct kvec {
        void *iov_base; /* and that should *never* hold a userland pointer */
        size_t iov_len;
};


struct iov_iter {
        int type;
        size_t iov_offset;
        size_t count;
        union {
                const struct iovec *iov;
                const struct kvec *kvec;
                const struct bio_vec *bvec;
        };
        unsigned long nr_segs;
};






register unsigned long current_stack_pointer asm ("sp");
#define THREAD_SIZE_ORDER       1
#define THREAD_SIZE             (PAGE_SIZE << THREAD_SIZE_ORDER)

struct thread_info {
        unsigned long           flags;          /* low level flags */
        int                     preempt_count;  /* 0 => preemptable, <0 => bug */
        mm_segment_t            addr_limit;     /* address limit */
        
        /*...*/
};

static inline struct thread_info *current_thread_info(void)
{
        return (struct thread_info *)
                (current_stack_pointer & ~(THREAD_SIZE - 1));
}

#define get_fs() (current_thread_info()->addr_limit)

static inline void set_fs(mm_segment_t fs)
{
        current_thread_info()->addr_limit = fs;
}


typedef unsigned gfp_t;

struct mem_section {
 unsigned long section_mem_map;
 unsigned long *pageblock_flags;
};

#define min(x, y) ({                            \
        typeof(x) _min1 = (x);                  \
        typeof(y) _min2 = (y);                  \
        (void) (&_min1 == &_min2);              \
        _min1 < _min2 ? _min1 : _min2; })



#endif