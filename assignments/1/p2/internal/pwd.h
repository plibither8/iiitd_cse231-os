#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../global.h"

WD working_dirs;

int pwd(int argc, char *argv[])
{
  int print_physical = 0;

  for (int i = 0; i < argc; i++)
  {
    char *arg = argv[i];
    // Single dash keyword letter options
    if (strlen(arg) > 1 && arg[0] == '-')
    {
      for (int j = 1; j < strlen(arg); j++)
      {
        char letter = arg[j];
        if (letter == 'P') print_physical = 1;
        else if (letter == 'L') print_physical = 0;
        else
        {
          fprintf(stderr, "p8sh: pwd: -%c: invalid option\n", letter);
          return 1;
        }
      }
    }
  }

  if (print_physical)
    printf("%s\n", realpath(working_dirs.current, NULL));
  else
    printf("%s\n", working_dirs.current);

  return 0;
}
