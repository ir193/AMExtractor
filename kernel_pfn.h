#ifndef _KMEM_PFN_H
#define _KMEM_PFN_H
#include "kernel_struct.h"

#ifdef SPARSE_MEM

struct mem_section *__nr_to_section(unsigned long nr);

struct mem_section *__pfn_to_section(unsigned long pfn);

struct page *__section_mem_map_addr(struct mem_section *section);

struct page* _pfn_to_page(resource_size_t pfn);

#else

struct page* _pfn_to_page(resource_size_t pfn);

#endif


#endif