#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int exit_wrp(int argc, char *argv[])
{
  int exit_code = 0;

  if (argc > 0)
  {
    char *arg = argv[0];
    int j = 0;
    while (isdigit(arg[j]))
      j++;

    if (strlen(arg) != j)
    {
      fprintf(stderr, "p8sh: exit: %s: numeric argument required\n", arg);
      return 1;
    }

    exit_code = atoi(arg);
  }

  if (argc > 1)
  {
    fprintf(stderr, "p8sh: exit: too many arguments\n");
    return 1;
  }

  exit(exit_code);
}
