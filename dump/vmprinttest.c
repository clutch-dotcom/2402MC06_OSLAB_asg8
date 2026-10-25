#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  printf(1, "=== INITIAL PAGE TABLE DUMP ===\n");
  vmprint();

  printf(1, "\n=== GROWING HEAP BY 8 PAGES: sbrk(8 * 4096) ===\n");
  char *ptr = sbrk(8 * 4096);
  if(ptr == (char*)-1){
    printf(1, "sbrk failed\n");
    exit();
  }
  vmprint();

  printf(1, "\n=== SHRINKING HEAP BY 1 PAGE: sbrk(-4096) ===\n");
  sbrk(-4096);
  vmprint();

  exit();
}
