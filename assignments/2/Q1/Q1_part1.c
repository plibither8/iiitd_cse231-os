/* Name: Mihir Chaturvedi
   Roll_Number: 2019061 */
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int num = 10;

int main(int argc, char *argv[])
{
  switch(fork())
  {
    case -1:
    {
      perror("Fork");
      exit(1);
    }
    case 0:
    {
      while(--num > -90);
      break;
    }
    default:
    {
      while(++num < 100);
      wait(NULL);
    }
  }

  printf("%d\n", num);
  return 0;
}
