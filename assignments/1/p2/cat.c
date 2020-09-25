#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char *argv[])
{
  char *paths[argc];
  int path_count = 0;

  int flag_all = 0;
  int flag_list = 0;

  for (int i = 1; i < argc; i++)
  {
    char *arg = argv[i];

    // Check if the argument is a flag
    if (arg[0] == '-' && strcmp(arg, "-") != 0)
    {
      if (strcmp(arg, "-a") == 0 || strcmp(arg, "--all") == 0)
      {
        flag_all = 1;
        continue;
      }
      else if (strcmp(arg, "-1") == 0)
      {
        flag_list = 1;
        continue;
      }
      else
      {
        fprintf(stderr, "ls: unrecognized option '%s'\n", arg);
        exit(1);
      }
    }

    // This argument is not an option/flag its a path to be ls'ed
    paths[path_count++] = arg;
  }

  return 0;
}
