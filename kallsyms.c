#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "kallsyms.h"

bool
kallsyms_exist(void)
{
  struct stat st;
  int fd;
  char buf = '0';
  if (stat("/proc/kallsyms", &st) < 0) {
    return false;
  }

  if(st.st_mode & S_IROTH) {
    fd = open("/proc/sys/kernel/kptr_restrict", O_RDWR);
    write(fd, &buf, 1);
    //printf("%s\n", buf);
    return kallsyms_get_symbol_address("_stext") != 0;
  }

  return false;
}

void* kallsyms_get_symbol_address(const char *symbol_name)
{
  FILE *fp;
  char function[BUFSIZ];
  char symbol;
  void *address;
  int ret;

  fp = fopen("/proc/kallsyms", "r");
  if (!fp) {
    printf("Failed to open /proc/kallsyms due to %s.", strerror(errno));
    exit(1);
    return 0;
  }

  while(!feof(fp)) {
    ret = fscanf(fp, "%p %c %s", &address, &symbol, function);
    if (ret != 3) {
      break;
    }

    if (!strcmp(function, symbol_name)) {
      fclose(fp);
      return address;
    }
  }
  fclose(fp);

  printf("addr %s not found\n", symbol_name);
  //exit(1);
  return NULL;
}


void* detect_kernel_phys_parameters(void)
{
  FILE *fp;
  void *system_ram_address;
  char name[BUFSIZ];
  void *start_address, *end_address;
  int ret;

  system_ram_address = NULL;

  fp = fopen("/proc/iomem", "r");
  if (!fp) {
    printf("Failed to open /proc/iomem due to %s.\n", strerror(errno));
    return NULL;
  }

  while ((ret = fscanf(fp, "%p-%p : %[^\n]", &start_address, &end_address, name)) != EOF) {
    if (!strcmp(name, "System RAM")) {
      system_ram_address = start_address;
      continue;
    }
    if (!strncmp(name, "Kernel", 6)) {
      break;
    }
  }
  fclose(fp);

  return system_ram_address;
}