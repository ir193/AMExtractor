#include "kernel_struct.h"
#include "kernel_func.h"



#define LIME_MAGIC 0x4C694D45 //LiME
typedef struct {
    unsigned int magic;
    unsigned int version;
    unsigned long long s_addr;
    unsigned long long e_addr;
    unsigned char reserved[8];
} __attribute__ ((__packed__)) lime_mem_range_header;



extern char *zero_page;
extern lime_mem_range_header *header_page;

static void _write_range(struct resource * res);
static int _write_lime_header(struct resource * res);
static ssize_t _write_padding(size_t s);
static _do_dump();

void _begin_dump(){

    zero_page = _get_zeroed_page(0xd0); 
    //_setup_tcp();
    //_do_dump();
    //_cleanup_tcp();
}

static _do_dump(){
    struct page *mem_map = *_mem_map;

    struct resource *res;
    int err = 0;

    resource_size_t p_last = 0;
    DBG("begin dump");
    DBG("mem_map addr: %p", mem_map);
    DBG("sizeof page %d", sizeof(struct page));
    
    DBG("alloc page");
    header_page = (lime_mem_range_header *) _get_zeroed_page(0xd0); 
    zero_page = (char *) _get_zeroed_page(0xd0); 
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
            DBG("Error writing header_page 0x%lx - 0x%lx", (long) res->start, (long) res->end);
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
        p = (mem_map + ((i >> PAGE_SHIFT) - (pfn_offset >> PAGE_SHIFT)));

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

    memset(header_page, 0, sizeof(lime_mem_range_header));
    header_page->magic = LIME_MAGIC;
    header_page->version = 1;
    header_page->s_addr = res->start;
    header_page->e_addr = res->end;
    
    s = _write_vaddr_tcp(header_page, sizeof(lime_mem_range_header));
    
    if (s != sizeof(lime_mem_range_header)) {
        DBG("Error sending header_page %zd", s);
        return (int) s;
    }               

    return 0;
}

static ssize_t _write_padding(size_t s) {
    size_t i = 0;
    ssize_t r;

    while(s -= i) {

        i = min((size_t) PAGE_SIZE, s);
        r = _write_vaddr_tcp(zero_page, i);

        if (r != i) {
            DBG("Error sending zero page: %zd", r);
            return r;
        }
    }

    return 0;
}
