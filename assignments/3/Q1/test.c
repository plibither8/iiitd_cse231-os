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
  for (int i = 0; i < 1000000000; i++)
    sum += i;
}

int main(int argc, char **argv)
{
  struct timeval start_time, stop_time;
  gettimeofday(&start_time, NULL);

  int rtnice_val = 100;
  int child_pid = fork();
  syscall(RTNICE_ID, child_pid, rtnice_val);

  switch (child_pid)
  {
    case -1: {
      break;
    }
    case 0: {
      printf("Child process: PID %d\n", child_pid);
      long_computation();
      break;
    }
    default: {
      printf("Parent process: PID %d\n", (int)getpid());
      long_computation();
      wait(NULL);
    }
  }

  gettimeofday(&stop_time, NULL);
  printf("Task duration for PID %d: lu%", (int)getpid(), stop_time.tv_usec - start_time.tv_usec);

  return 0;
}
