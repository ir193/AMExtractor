#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "config.h"
#include "kmem.h"
#include "kernel_struct.h"
#include "kernel_pfn.h"
#include "kallsyms.h"


ssize_t _hello_world(void * file, long long a, long long b, int datasync);
ssize_t _tcp_dump(void * file, long long a, long long b, int datasync);
ssize_t _test(void * file, long long a, long long b, int datasync);
void setup();
void trigger();
void kmem_dump(int offset);

ssize_t (*hook_func)(void * file, long long a, long long b, int datasync);


#define DBG(fmt, args...) do { _printk("[KMEM] "fmt"\n", ## args); } while (0)


int main(int argc, char const *argv[])
{
    unsigned int addr;
    char buf[300];

    if(argc >= 2){
        if(strcmp(argv[1], "-r")==0){
            if(argc < 3){
                printf("No address given to read from /dev/kmem");
                exit(1);
            }
            addr = strtoul(argv[2], NULL, 16);
            kmem_dump(addr);
            exit(0);
        }
        else if(strcmp(argv[1], "-d")==0){
            hook_func = &_tcp_dump;
            setup();
            trigger();
        }
        else if(strcmp(argv[1], "-t")==0){
            hook_func = &_test;
            setup();
            trigger();
        }
        else{
            printf("Unkown argument");
        }
    }
    else {
        hook_func = &_hello_world;
        setup();
        trigger();
    }

    return 0;
}

unsigned int pfn_offset;
int (*_printk)(const char *fmt, ...);
int (*_sock_create_kern)(int family, int type, int proto, struct socket **res);

struct resource *_iomem_resource;
struct file_operations *_fops;

int (*_sock_setsockopt)(struct socket *sock, int level, int op,
                     char *optval, unsigned int optlen);
int (*_kernel_sendmsg)(struct socket *sock, struct msghdr *msg, struct kvec *vec,
                    size_t num, size_t len);
void* (*_kmap)(struct page *page);
void (*_kunmap)(struct page *page);
int (*_pfn_valid)(unsigned long pfn);

void* (*_kmap_atomic_pfn)(unsigned long pfn);
void (*__kunmap_atomic)(void *kvaddr);
void (*_copy_page)(void *to, const void *from);

unsigned long (*_get_zeroed_page)(gfp_t gfp_mask);
struct page **_mem_map;
struct mem_section **_mem_section;


void setup(){
    char buf[80];
    int offset;

    if(!kallsyms_exist()){
        perror("Error open /proc/kallsyms");
        exit(1);
    }
    //_zero_fops = (struct file_operations *) 0xc061fb50;

    pfn_offset = (unsigned int) detect_kernel_phys_parameters();


    _printk    = kallsyms_get_symbol_address("printk");
#ifdef CONFIG_IOMEM
    _iomem_resource = CONFIG_IOMEM;
#else
    _iomem_resource = kallsyms_get_symbol_address("iomem_resource");
#endif
    _kmap      = kallsyms_get_symbol_address("kmap");
    _kunmap    = kallsyms_get_symbol_address("kunmap");
    //_pfn_valid = kallsyms_get_symbol_address("pfn_valid");
    //_kmap_atomic_pfn = kallsyms_get_symbol_address("kmap_atomic_pfn");
    //__kunmap_atomic = kallsyms_get_symbol_address("__kunmap_atomic");
    _copy_page = kallsyms_get_symbol_address("copy_page");

    _get_zeroed_page = kallsyms_get_symbol_address("get_zeroed_page");

    _sock_create_kern = kallsyms_get_symbol_address("sock_create_kern");
    _sock_setsockopt = kallsyms_get_symbol_address("sock_setsockopt");
    _kernel_sendmsg = kallsyms_get_symbol_address("kernel_sendmsg");


#ifdef FLAT_MEM
    #ifdef CONFIG_MEMMAP
    _mem_map   = CONFIG_MEMMAP;
    #else
    _mem_map   = kallsyms_get_symbol_address("mem_map");
    #endif
    //_mem_map   = (void*) 0xc122afc0;
#endif
#ifdef SPARSE_MEM
    _mem_section = kallsyms_get_symbol_address("mem_section");
#endif

#ifdef USE_SEEK_ZERO
    _fops = kallsyms_get_symbol_address("zero_fops");
    offset = ((unsigned long) &(((struct file_operations *) 0)->write));
#endif
#ifdef USE_SYNC_PTMX
    _fops = kallsyms_get_symbol_address("ptmx_fops");
    offset = ((unsigned long) &(((struct file_operations *) 0)->fsync));
#endif

    int wr_addr = (int)_fops + offset;

    int kmem = open("/dev/kmem",O_RDWR);
    if (kmem<0){
        perror("open /dev/kmem error\n");
        exit(1);
    }

    printf("before write to kmem: ");
    rkm(kmem, wr_addr, (void *)buf, 80);
    printmem(buf, 10);

    unsigned long int addr = (unsigned long int)hook_func;
    printf("write to kmem\n");
    wkm(kmem, wr_addr, &addr, sizeof(addr));

    printf("after write to kmem: ");
    rkm(kmem, wr_addr, (void *)buf, 80);
    printmem(buf, 10);

}

void trigger(){
    printf("Trigger hook!!!\n");
    if(hook_func == &_hello_world){
        printf("A simple hello world\n");
    }
    else{
        printf("do the real dump\n");
    }
#ifdef USE_SYNC_PTMX
    int fd = open("/dev/ptmx", O_WRONLY);
    //int ret = write(fd, "trigger", 7);
    fsync(fd);
#endif

#ifdef USE_SEEK_ZERO
    int fd = open("/dev/zero", O_WRONLY);
    int ret = write(fd, "trigger", 7);
#endif
}

void kmem_dump(int addr){
    unsigned int buf[300];

    int kmem = open("/dev/kmem", O_RDWR);
    if (kmem<0){
        perror("open file error\n");
        exit(1);
    }
    rkm(kmem, addr, (void *)buf, 256);
    printmem(buf, 256 );
    printf("first  byte: 0x%08x\n", buf[0]);
    printf("second byte: 0x%08x\n", buf[1]);
    printf("third  byte: 0x%08x\n", buf[2]);
    printf("fourth byte: 0x%08x\n", buf[3]);

    int fd = open("/data/local/tmp/vm", O_CREAT | O_WRONLY);
    if(fd < 0){
        perror("Open Error\n");
    }
    write(fd, buf, 256);
    printf("dump some bytes into /data/local/tmp/vm\n");
    close(fd);
}

ssize_t _hello_world(void * file, long long a, long long b, int datasync){
    _printk("Hello kmem...\n");
    return 0;
}




ssize_t _test(void * file, long long a, long long b, int datasync){
    //struct page *mem_map = *_mem_map;

    struct resource *res;
    int err = 0;
    struct page *p;
    resource_size_t i, is;
    resource_size_t p_last = 0;
    void *v;
    int j = 0;

    DBG("Test map page");
    //printf("test");
    //DBG("mem_map addr: %p", mem_map);
    DBG("sizeof page %d", sizeof(struct page));
    DBG("%p", _iomem_resource);
    DBG("pfn_offset %p", pfn_offset);

    for (res = _iomem_resource->child; res ; res = res->sibling) {
        
        if (strcmp(res->name, "System RAM")){
            DBG("skip %s", res->name);
            continue;
        }
        for (i = res->start; i <= res->end; i += is) {

        // XXX: TODO
            j ++;
            p = _pfn_to_page(i >> PAGE_SHIFT);

           
            is = min((size_t) PAGE_SIZE, (size_t) (res->end - i + 1));
            if (is < PAGE_SIZE) {
                DBG("Padding partial page: vaddr %p size: %lu", (void *) i, (unsigned long) is);
            }
            else{
                v = _kmap(p);
                //s = _write_vaddr_tcp(v, is);
                _kunmap(p);
            }
        }
        
    }
    DBG("totol: %d", j); 
}


void _do_dump();
ssize_t _tcp_dump(void * file, long long a, long long b, int datasync){
    _setup_tcp();
    _do_dump();
    _cleanup_tcp();
    return 0;
}


#define LIME_MAGIC 0x4C694D45 //LiME
typedef struct {
    unsigned int magic;
    unsigned int version;
    unsigned long long s_addr;
    unsigned long long e_addr;
    unsigned char reserved[8];
} __attribute__ ((__packed__)) lime_mem_range_header;

static char *_zero_page;
static lime_mem_range_header *_header_page;

static void _write_range(struct resource * res);
static int _write_lime_header(struct resource * res);
static ssize_t _write_padding(size_t s);


void _do_dump(){
    //struct page *mem_map = *_mem_map;

    struct resource *res;
    int err = 0;

    resource_size_t p_last = 0;
    DBG("begin dump");
    //DBG("mem_map addr: %p", mem_map);
    DBG("sizeof page %d", sizeof(struct page));
    
    DBG("alloc page");
    _header_page = (lime_mem_range_header *) _get_zeroed_page(0xd0); 
    _zero_page = (char *) _get_zeroed_page(0xd0); 
    DBG("alloc page done");

    for (res = _iomem_resource->child; res ; res = res->sibling) {
        if (strcmp(res->name, "System RAM")){
            DBG("skip %s", res->name);
            continue;
        }
        else{
            DBG("RAM from %lx to %lx", (long) res->start, (long) res->end);
        }   

        err = _write_lime_header(res);
        if(err){
            DBG("Error writing _header_page 0x%lx - 0x%lx", (long) res->start, (long) res->end);
        }
        
        _write_range(res);
        
        p_last = res->end;
    }
}

static void _write_range(struct resource * res) {
    int j = 0;
    struct page *p;
    resource_size_t i, is;
    void *v;
    ssize_t s;
    struct page *mem_map = *_mem_map;

    for (i = res->start; i <= res->end; i += is) {

        // XXX: TODO
        p = _pfn_to_page(i >> PAGE_SHIFT);

        is = min((size_t) PAGE_SIZE, (size_t) (res->end - i + 1));
        if (is < PAGE_SIZE) {
            DBG("Padding partial page: vaddr %p size: %lu", (void *) i, (unsigned long) is);
            _write_padding(is);
        }
        else{
            v = _kmap(p);
            s = _write_vaddr_tcp(v, is);
            _kunmap(p);
            //_printk("kmem pfn %d one page: %p valid: %d\n",i, p, _pfn_valid(i));

            if (s < 0) {
                DBG("Error writing page: vaddr %p ret: %zd.  Null padding.", v, s);
                _write_padding(is);
            } else if (s != is) {
                DBG("Short Read %zu instead of %lu.  Null padding.", s, (unsigned long) is);
                _write_padding(is - s);
            }

        }
       
    }
}


static int _write_lime_header(struct resource * res) {
    ssize_t s;

    memset(_header_page, 0, sizeof(lime_mem_range_header));
    _header_page->magic = LIME_MAGIC;
    _header_page->version = 1;
    _header_page->s_addr = res->start;
    _header_page->e_addr = res->end;
    
    s = _write_vaddr_tcp(_header_page, sizeof(lime_mem_range_header));
    
    if (s != sizeof(lime_mem_range_header)) {
        DBG("Error sending _header_page %zd", s);
        return (int) s;
    }               

    return 0;
}

static ssize_t _write_padding(size_t s) {
    size_t i = 0;
    ssize_t r;

    while(s -= i) {

        i = min((size_t) PAGE_SIZE, s);
        r = _write_vaddr_tcp(_zero_page, i);

        if (r != i) {
            DBG("Error sending zero page: %zd", r);
            return r;
        }
    }

    return 0;
}
