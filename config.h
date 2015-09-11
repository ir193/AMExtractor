#ifndef _KMEM_CONFIG_H
#define _KMEM_CONFIG_H


/******************************/
//#define CONFIG_NEXUS5
#define CONFIG_SM_G3508I
/******************************/





#ifdef CONFIG_NEXUS4
    #define SPARSE_MEM
    #define STRUCT_PAGE_SIZE 32
    #define USE_SYNC_PTMX
#endif


#ifdef CONFIG_NEXUS5
    #define FLAT_MEM
    #define STRUCT_PAGE_SIZE 32
    #define USE_SYNC_PTMX
#endif

#ifdef CONFIG_I9500
    #define FLAT_MEM
    #define STRUCT_PAGE_SIZE 36
    #define USE_SYNC_PTMX
#endif

#ifdef CONFIG_SM_G3508I
    #define FLAT_MEM
    #define CONFIG_IOMEM 0xC4C04668
    #define CONFIG_MEMMAP 0xC4D6C9E0
    #define STRUCT_PAGE_SIZE 32
    #define USE_SEEK_ZERO
#endif


#if !defined(FLAT_MEM) && !defined(SPARSE_MEM)
    #error "Unsupported Memory Module!"
#endif

#endif