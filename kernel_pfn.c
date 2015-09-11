#include "kernel_struct.h"
#include "kernel_func.h"


#define SECTIONS_PER_ROOT       (PAGE_SIZE / sizeof (struct mem_section))
#define SECTION_NR_TO_ROOT(sec) ((sec) / SECTIONS_PER_ROOT)
#define SECTION_ROOT_MASK       (SECTIONS_PER_ROOT - 1)

#define SECTION_MARKED_PRESENT  (1UL<<0)
#define SECTION_HAS_MEM_MAP     (1UL<<1)
#define SECTION_MAP_LAST_BIT    (1UL<<2)
#define SECTION_MAP_MASK        (~(SECTION_MAP_LAST_BIT-1))
#define SECTION_NID_SHIFT       2

#define PFN_SECTION_SHIFT       (SECTION_SIZE_BITS - PAGE_SHIFT)

#define pfn_to_section_nr(pfn) ((pfn) >> PFN_SECTION_SHIFT)


#ifdef SPARSE_MEM

static struct mem_section **mem_section;

struct mem_section *__nr_to_section(unsigned long nr)
{
 if (!_mem_section[((nr) / (((1UL) << 12) / sizeof (struct mem_section)))])
  return ((void *)0);
 return &_mem_section[((nr) / (((1UL) << 12) / sizeof (struct mem_section)))][nr & ((((1UL) << 12) / sizeof (struct mem_section)) - 1)];
}

struct mem_section *__pfn_to_section(unsigned long pfn)
{
 return __nr_to_section(((pfn) >> (28 - 12)));
}

struct page *__section_mem_map_addr(struct mem_section *section)
{
 unsigned long map = section->section_mem_map;
 map &= (~((1UL<<2)-1));
 return (struct page *)map;
}

struct page* _pfn_to_page(resource_size_t pfn){
    unsigned long __pfn = pfn;
    struct mem_section *__sec = __pfn_to_section(__pfn);
    //_printk("section :%p", __sec);
    //return NULL;
    return __section_mem_map_addr(__sec) + __pfn;
}

#endif 

#ifdef FLAT_MEM

struct page* _pfn_to_page(resource_size_t pfn){
    struct page *mem_map = *_mem_map;
    return (mem_map + (pfn - (pfn_offset >> PAGE_SHIFT)));
}

#endif
/*
struct mem_section *__nr_to_section(unsigned long nr){
        if (!mem_section[SECTION_NR_TO_ROOT(nr)])
                return NULL;
        return &mem_section[SECTION_NR_TO_ROOT(nr)][nr & SECTION_ROOT_MASK];
}

struct page *__section_mem_map_addr(struct mem_section *section)
{
        unsigned long map = section->section_mem_map;
        map &= SECTION_MAP_MASK;
        return (struct page *)map;
}


/*
struct page* _pfn_to_page(resource_size_t pfn){
    //0xc024f348: ldr r0, [r3]
    //0xc024f34c: add r0, r0, r4, lsl #5
    struct page *result;
    struct page *mem_map = *_mem_map;
    /*
    asm("mov r0, %1\n\t"
        "mov r4, %2\n\t"
        "add r0, r0, r4, lsl #5\n\t"
        "mov %0, r0"
        : "=r" (result)
        : "r" (mem_map), "r" (pfn)
        : "r0", "r4", "memory"
        );
        /
    asm("mov %[result], %[value], ror #1" : [result] "=r" (result) : [value] "r" (pfn));
    return result;
}

*/