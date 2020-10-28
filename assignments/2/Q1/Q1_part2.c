/* Name: Mihir Chaturvedi
   Roll_Number: 2019061 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int num = 10;

void *decrementer(void *args)
{
  while(--num > -90);
  printf("%d\n", num);
}

int main(int argc, char *argv[])
{
  pthread_t child;
  pthread_create(&child, NULL, decrementer, NULL);

  while(++num < 100);
  pthread_join(child, NULL);
  printf("%d\n", num);

  return 0;
}
