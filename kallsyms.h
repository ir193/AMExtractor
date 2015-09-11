#ifndef _KMEM_KALLSYMS_H
#define _KMEM_KALLSYMS_H

#include <stdbool.h>

bool  kallsyms_exist(void);
void *kallsyms_get_symbol_address(const char *symbol_name);

void* detect_kernel_phys_parameters(void);
#endif /* _KMEM_KALLSYMS_H */