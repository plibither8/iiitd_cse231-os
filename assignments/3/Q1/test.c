/* Name: Mihir Chaturvedi
   Roll_Number: 2019061 */
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <wait.h>

#define RTNICE_ID 440

void long_computation()
{
  long long sum = 0;
  for (long i = 0; i < 1000000000; i++)
    sum += i;
}

int main(int argc, char **argv)
{
  float duration;
  int rtnice_val = 100;
  int child_pid = fork();
  double cpu_time_used;
  clock_t end, start = clock();

  switch (child_pid)
  {
    case -1: {
      break;
    }
    case 0: {
      int pid = getpid();
      printf("Child process: PID %d\n", pid);
      syscall(RTNICE_ID, pid, rtnice_val);
      long_computation();
      end = clock();
      cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
      printf("Task duration for child: %f seconds\n", cpu_time_used);
      break;
    }
    default: {
      printf("Parent process: PID %d\n", (int)getpid());
      long_computation();
      end = clock();
      cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
      printf("Task duration for parent: %f seconds\n", cpu_time_used);
      wait(NULL);
    }
  }

  return 0;
}
