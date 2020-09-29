#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

typedef struct flags
{
  int verbose;
  int force;
} flags;

int ERR_STATUS = 0;

int main(int argc, char *argv[])
{
  char *paths[argc];
  int path_count = 0;
  flags flag = { 0, 0 };

  for (int i = 1; i < argc; i++)
  {
    char *arg = argv[i];

    // Double dash keyword options
    if (strlen(arg) > 2 && arg[0] == '-' && arg[1] == '-')
    {
      if (strcmp(arg, "--verbose") == 0) flag.verbose = 1;
      else if (strcmp(arg, "--force") == 0) flag.force = 1;
      else
      {
        fprintf(stderr, "rm: unrecognized option '%s'\n", arg);
        exit(EXIT_FAILURE);
      }
      continue;
    }

    // Single dash keyword letter options
    if (strlen(arg) > 1 && arg[0] == '-')
    {
      for (int j = 1; j < strlen(arg); j++)
      {
        char letter = arg[j];
        if (letter == 'v') flag.verbose = 1;
        else if (letter == 'f') flag.force = 1;
        else
        {
          fprintf(stderr, "rm: invalid option -- '%c'\n", letter);
          exit(EXIT_FAILURE);
        }
      }
      continue;
    }

    // This argument is not an option/flag its a path to be cat'ed
    paths[path_count++] = arg;
  }

  if (path_count == 0 && !flag.force)
  {
    fprintf(stderr, "rm: missing operand\n");
    exit(1);
  }

  for (int i = 0; i < path_count; i++)
  {
    char *path = paths[i];
    if (unlink(path) == -1)
    {
      switch (errno)
      {
        case EACCES:
          fprintf(
            stderr,
            "rm: cannot remove '%s': Permission denied\n",
            path
          );
          break;

        case EISDIR:
          fprintf(
            stderr,
            "rm: cannot remove '%s': Is a directory\n",
            path
          );
          break;

        case ENOENT:
          if (!flag.force)
            fprintf(
              stderr,
              "rm: cannot remove '%s': No such file or directory\n",
              path
            );
      }

      ERR_STATUS = 1;
      continue;
    }

    if (flag.verbose)
      printf("Removed '%s'\n", path);
  }

  return 0;
}
