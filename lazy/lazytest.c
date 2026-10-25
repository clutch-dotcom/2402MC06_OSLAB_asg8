#include "types.h"
#include "stat.h"
#include "user.h"
#include "meminfo.h"

void
print_free(const char *label)
{
  struct meminfo mi;
  if(getmeminfo(&mi) == 0){
    printf(1, "%s: Free Pages = %d\n", label, mi.free_pages);
  }
}

int
main(void)
{
  printf(1, "=== LAZY ALLOCATION TEST ===\n\n");

  print_free("1. Before sbrk(32MB)");

  // 32 MB = 32 * 1024 * 1024 bytes
  int sz32mb = 32 * 1024 * 1024;
  char *p = sbrk(sz32mb);
  if(p == (char*)-1){
    printf(1, "sbrk failed\n");
    exit();
  }

  print_free("2. Immediately after sbrk(32MB)");

  // Touch 3 widely spaced pages
  printf(1, "Touching 3 pages in 32MB range...\n");
  p[0] = 'A';
  p[16 * 1024 * 1024] = 'B';
  p[31 * 1024 * 1024] = 'C';

  print_free("3. After touching 3 pages");

  // Test system call on an untouched page buffer
  printf(1, "Testing system call on untouched page...\n");
  char *sys_buf = p + 20 * 1024 * 1024;
  int pipefd[2];
  if(pipe(pipefd) == 0){
    write(pipefd[1], "hello", 5);
    read(pipefd[0], sys_buf, 5);
    printf(1, "System call buffer read: %s\n", sys_buf);
    close(pipefd[0]);
    close(pipefd[1]);
  }

  // Test fork, exit and memory release
  int pid = fork();
  if(pid < 0){
    printf(1, "fork failed\n");
    exit();
  }

  if(pid == 0){
    printf(1, "Child process active...\n");
    exit();
  } else {
    wait();
    print_free("4. After child fork/exit/reap");
  }

  printf(1, "\nSUCCESS: Lazy allocation test complete.\n");
  exit();
}
