#include <stdlib.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include "kernel_struct.h"
#include "kernel_func.h"

static struct socket *control;
static struct socket *accept_fd;


int _setup_tcp() {
    struct sockaddr_in saddr;
    int r;
    
    int buffsize = PAGE_SIZE;

    mm_segment_t fs;

    r = _sock_create_kern(AF_INET, SOCK_STREAM, IPPROTO_TCP, &control);
    if (r < 0) {
        DBG("Error creating control socket");
        return r;
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    int port = 31415;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    fs = get_fs();
    set_fs(0x00000000);
    DBG("call _sock_setsockopt %08x\n", _sock_setsockopt);
    _sock_setsockopt(control, SOL_SOCKET, SO_SNDBUF, (void *) &buffsize, sizeof (int));

    set_fs(fs);

    DBG("bind");
    r = control->ops->bind(control,(struct sockaddr*) &saddr,sizeof(saddr));

    if (r < 0) {
        DBG("Error binding control socket");
        return r;
    }

    DBG("listen");
    r = control->ops->listen(control,1);
    if (r) {
        DBG("Error listening on socket");
        return r;
    }

    r = _sock_create_kern(PF_INET, SOCK_STREAM, IPPROTO_TCP, &accept_fd);
    if (r < 0) {
        DBG("Error creating accept socket");
        return r;
    }

    DBG("accept");
    r = accept_fd->ops->accept(control,accept_fd,0);
    if (r < 0) {
        DBG("Error accepting socket");
        return r;
    }

}

ssize_t _write_vaddr_tcp(void * v, size_t is) {
    ssize_t s;
    struct kvec iov;
    struct msghdr msg;

    memset(&iov, 0, sizeof(struct iovec));
    memset(&msg, 0, sizeof(struct msghdr));

    iov.iov_base = v;   
    iov.iov_len = is;
    
    s = _kernel_sendmsg(accept_fd, &msg, &iov, 1, is);

    return s;
}

void _cleanup_tcp() {
    if (accept_fd && accept_fd->ops) {
        accept_fd->ops->shutdown(accept_fd, 0);
        accept_fd->ops->release(accept_fd);
    }
    
    if (control && control->ops) {
        control->ops->shutdown(control, 0);
        control->ops->release(control);
    }
}