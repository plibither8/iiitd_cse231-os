#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct flags
{
  int verbose;
  mode_t mode;
} flags;

int ERR_STATUS = 0;

int main(int argc, char *argv[])
{
  char *paths[argc];
  int path_count = 0;
  flags flag = { 0, 0775 };

  for (int i = 1; i < argc; i++)
  {
    char *arg = argv[i];

    // Double dash keyword options
    if (strlen(arg) > 2 && arg[0] == '-' && arg[1] == '-')
    {
      if (strcmp(arg, "--verbose") == 0) flag.verbose = 1;
      else
      {
        char keyword[8];
        memcpy(keyword, arg, 7);
        keyword[7] = '\0';

        if (strcmp(keyword, "--mode=") == 0)
        {
          char *raw_mode = &arg[7];
          mode_t mode = (mode_t) strtol(raw_mode, NULL, 10);
          if (
            (errno == ERANGE && (mode == LONG_MAX || mode == LONG_MIN)) ||
            (mode == 0)
          )
          {
            fprintf(stderr, "mkdir: invalid mode '%s'", raw_mode);
            exit(EXIT_FAILURE);
          }

          flag.mode = mode;
        }
        else {
          fprintf(stderr, "mkdir: unrecognized option '%s'\n", arg);
          exit(EXIT_FAILURE);
        }
      }
      continue;
    }

    // Single dash keyword letter options
    if (strlen(arg) > 1 && arg[0] == '-')
    {
      for (int j = 1; j < strlen(arg); j++)
      {
        char letter = arg[j];
        if (letter == 'V') flag.verbose = 1;
        else if (letter == 'p') flag.mode = 1;
        else
        {
          fprintf(stderr, "mkdir: invalid option -- '%c'\n", letter);
          exit(EXIT_FAILURE);
        }
      }
      continue;
    }

    // This argument is not an option/flag its a path to be cat'ed
    paths[path_count++] = arg;
  }

  umask(flag.mode);

  if (path_count == 0)
  {
    fprintf(stderr, "mkdir: missing operand\n");
    exit(1);
  }

  for (int i = 0; i < path_count; i++)
  {
    char *path = paths[i];

    if (mkdir(path, flag.mode) == -1)
    {
      switch (errno)
      {
        case EACCES:
          fprintf(
            stderr,
            "mkdir: cannot create directory '%s': Permission denied\n",
            path
          );
          break;

        case EEXIST:
          fprintf(
            stderr,
            "mkdir: cannot create directory '%s': File exits\n",
            path
          );
          break;

        case ENOENT:
          fprintf(
            stderr,
            "mkdir: cannot create directory '%s': No such file or directory\n",
            path
          );
      }

      ERR_STATUS = 1;
      continue;
    }

    if (flag.verbose)
      printf("mkdir: created directory '%s'\n", path);
  }

  return 0;
}
