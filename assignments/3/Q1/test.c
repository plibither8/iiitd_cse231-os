/* Name: Mihir Chaturvedi
   Roll_Number: 2019061 */
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/kernel.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <omp.h>
#include "rtnice.c"

void long_computation()
{
  long long sum = 0;
  for (long i = 0; i < 1000000000; i++)
    sum++;
}

int main(int argc, char **argv)
{
  int rtnice_step, fork_count, i;

  double duration;
  double end, start;

  printf("rtnice value step-size: ");
  scanf("%d", &rtnice_step);
  printf("Number of forks/children: ");
  scanf("%d", &fork_count);

  printf("With rtnice=0:\n");
  for (i = 0; i < fork_count; i++)
  {
    if (!fork()) {
      int pid = getpid();

      if (syscall(RTNICE_ID, pid, 0)) return -1;

      start = omp_get_wtime();
      long_computation();
      end = omp_get_wtime();
      duration = end - start;

      printf("Child process %d; rtnice=%d; duration=%lf seconds\n", i + 1, 0, duration);
      exit(0);
    }
  }

  while(i > 0)
  {
    wait(NULL);
    i--;
  }

  printf("With increasing rtnice:\n");
  for (i = 0; i < fork_count; i++)
  {
    if (!fork()) {
      int pid = getpid();
      int rtnice_val = i * rtnice_step;

      if (rtnice(pid, rtnice_val) < 0) return -1;

      start = omp_get_wtime();
      long_computation();
      end = omp_get_wtime();
      duration = end - start;

      printf("Child process %d; rtnice=%d; duration=%lf seconds\n", i + 1, rtnice_val, duration);
      exit(0);
    }
  }

  while(i > 0)
  {
    wait(NULL);
    i--;
  }

  return 0;
}
