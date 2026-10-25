#ifndef MEMINFO_H
#define MEMINFO_H

#include "types.h"

struct meminfo {
  uint total_pages;      // Total physical pages managed by kalloc
  uint free_pages;       // Current number of free physical pages
  uint proc_user_pages;  // User address space pages (p->sz / PGSIZE)
  uint proc_pt_pages;    // Pages used by process's page table hierarchy
};

#endif
