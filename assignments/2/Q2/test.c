#define _GNU_SOURCE

#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

#define SYS_SH_TASK_INFO 440

int main(int argc, char **argv)
{
  char *filename = "task-info.txt";
  int pid = 1;

  syscall(SYS_SH_TASK_INFO, pid, filename);

  return 0;
}
