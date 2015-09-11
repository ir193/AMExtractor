#ifndef _KMEM_TCP_H
#define _KMEM_TCP_H

void _test_page();
int _setup_tcp();
void _cleanup_tcp();
ssize_t _write_vaddr_tcp(void * v, size_t is);


#endif