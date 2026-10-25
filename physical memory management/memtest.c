#include "types.h"
#include "stat.h"
#include "user.h"
#include "meminfo.h"

void
print_info(const char *msg)
{
  struct meminfo mi;
  if(getmeminfo(&mi) < 0){
    printf(1, "getmeminfo failed\n");
    exit();
  }
  printf(1, "[%s]\n", msg);
  printf(1, "  Total Pages : %d\n", mi.total_pages);
  printf(1, "  Free Pages  : %d\n", mi.free_pages);
  printf(1, "  User Pages  : %d\n", mi.proc_user_pages);
  printf(1, "  PT Pages    : %d\n\n", mi.proc_pt_pages);
}

int
main(void)
{
  struct meminfo before, after;

  printf(1, "=== PHYSICAL MEMORY STATISTICS TEST ===\n\n");

  // Step 1: Initial statistics
  print_info("Initial State");

  // Step 2: Grow memory using sbrk()
  printf(1, "Allocating 4 physical pages via sbrk(16384)...\n");
  char *ptr = sbrk(4 * 4096);
  if(ptr == (char*)-1){
    printf(1, "sbrk failed\n");
    exit();
  }

  // Touch the allocated memory to force page allocation if lazy allocation is enabled
  for(int i = 0; i < 4 * 4096; i += 4096){
    ptr[i] = 1;
  }
  print_info("After Memory Allocation");

  // Step 3: Record state right before forking
  if(getmeminfo(&before) < 0){
    printf(1, "getmeminfo failed\n");
    exit();
  }

  // Step 4: Fork a child process
  int pid = fork();
  if(pid < 0){
    printf(1, "fork failed\n");
    exit();
  }

  if(pid == 0){
    // Inside Child Process
    print_info("Child Process Execution");
    exit();
  } else {
    // Inside Parent Process: Wait for child to exit and be reaped
    wait();

    // Step 5: Check statistics after child terminates and is reaped
    if(getmeminfo(&after) < 0){
      printf(1, "getmeminfo failed\n");
      exit();
    }

    print_info("After Child Exited & Reaped");

    // Step 6: Verify memory leak status
    if(before.free_pages == after.free_pages){
      printf(1, "SUCCESS: No memory leak detected! Free pages returned to %d.\n", after.free_pages);
    } else {
      printf(1, "FAILURE: Memory leak detected! Before fork: %d free pages, After reap: %d free pages.\n",
             before.free_pages, after.free_pages);
    }
  }

  exit();
}
