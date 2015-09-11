#ifndef _KMEM_CONFIG_H
#define _KMEM_CONFIG_H


/******************************/
//#define CONFIG_NEXUS5
//#define DEVICE "NEXUS5"
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

    #define STRUCT_PAGE_SIZE 32
    #define USE_SEEK_ZERO
#endif

#ifndef DEVICE
    #error "NO device config defined, please check README"
#endif


#if !defined(FLAT_MEM) && !defined(SPARSE_MEM)
    #error "Unsupported Memory Module!"
#endif


#endif