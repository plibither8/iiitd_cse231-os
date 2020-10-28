#define _GNU_SOURCE

#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <errno.h>

#define SYS_SH_TASK_INFO 440

int main(int argc, char **argv)
{
  char *filename = "task-info.txt";
  int pid = -1;

  int res = syscall(SYS_SH_TASK_INFO, pid, filename);

  if (res < 0) {
    perror("sh_task_info");
    return errno;
  }

  return 0;
}
