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

char *resolve_path(char *cwd, char *arg)
{
  char *new_path = (char *)calloc(200, sizeof(char));
  if (arg[0] == '/')
    strcpy(new_path, arg);
  else
  {
    strcpy(new_path, cwd);
    strcat(new_path, "/");
    strcat(new_path, arg);
  }
  return new_path;
}

int main(int argc, char *argv[])
{
  char *cwd = argv[0];

  char *a_paths[argc - 1];
  char *r_paths[argc - 1];
  int path_count = 0;
  flags flag = { 0, 0 };

  for (int i = 2; i < argc; i++)
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
    a_paths[path_count] = resolve_path(cwd, arg);
    r_paths[path_count++] = arg;
  }

  if (path_count == 0 && !flag.force)
  {
    fprintf(stderr, "rm: missing operand\n");
    exit(1);
  }

  for (int i = 0; i < path_count; i++)
  {
    char *a_path = a_paths[0];
    char *r_path = r_paths[0];
    if (unlink(a_path) == -1)
    {
      switch (errno)
      {
        case EACCES:
          fprintf(
            stderr,
            "rm: cannot remove '%s': Permission denied\n",
            r_path
          );
          break;

        case EISDIR:
          fprintf(
            stderr,
            "rm: cannot remove '%s': Is a directory\n",
            r_path
          );
          break;

        case ENOENT:
          if (!flag.force)
            fprintf(
              stderr,
              "rm: cannot remove '%s': No such file or directory\n",
              r_path
            );
      }

      ERR_STATUS = 1;
      continue;
    }

    if (flag.verbose)
      printf("Removed '%s'\n", r_path);
  }

  return 0;
}
