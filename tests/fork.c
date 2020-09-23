#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  printf("Hello, PID: %d\n", (int)getpid());
  int child_process = fork();
  printf("Poop PID: %d\n", getpid());
  if(child_process < 0) 
  {
    fprintf(stderr, "There seems to be an error in creating a child process\n");
    exit(1);
  }
  else if (child_process == 0)
  {
    printf("Yayy, I'm the child :D, PID: %d\n", (int)getpid());
  }
  else
  {
    printf("Hmm, still the parent here, PID: %d\n", (int)getpid());
  }
  return 0;
}
