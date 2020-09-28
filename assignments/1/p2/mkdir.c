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

void parse_mode(char *raw_mode, flags flag)
{
  mode_t mode = (mode_t)strtol(raw_mode, NULL, 8);
  if (
      (errno == ERANGE && (mode == LONG_MAX || mode == LONG_MIN)) ||
      (mode == 0))
  {
    fprintf(stderr, "mkdir: invalid mode '%s'", raw_mode);
    exit(EXIT_FAILURE);
  }

  flag.mode = mode;
}

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
      else
      {
        char keyword[8];
        memcpy(keyword, arg, 7);
        keyword[7] = '\0';

        if (strcmp(keyword, "--mode=") == 0)
        {
          char *raw_mode = &arg[7];
          parse_mode(raw_mode, flag);
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
      if (strcmp(arg, "-v") == 0) flag.verbose = 1;
      else
      {
        char keyword[4];
        memcpy(keyword, arg, 3);
        keyword[3] = '\0';

        if (strcmp(keyword, "-m=") == 0)
        {
          char *raw_mode = &arg[7];
          parse_mode(raw_mode, flag);
        }
        else
        {
          fprintf(stderr, "mkdir: invalid option -- '%c'\n", arg[1]);
          exit(EXIT_FAILURE);
        }
      }
      continue;
    }

    // This argument is not an option/flag its a path to be cat'ed
    a_paths[path_count] = resolve_path(cwd, arg);
    r_paths[path_count++] = arg;
  }

  if (path_count == 0)
  {
    fprintf(stderr, "mkdir: missing operand\n");
    exit(1);
  }

  for (int i = 0; i < path_count; i++)
  {
    char *a_path = a_paths[i];
    char *r_path = r_paths[i];

    if (mkdir(a_path, flag.mode ? flag.mode : 0755) == -1)
    {
      switch (errno)
      {
        case EACCES:
          fprintf(
            stderr,
            "mkdir: cannot create directory '%s': Permission denied\n",
            r_path
          );
          break;

        case EEXIST:
          fprintf(
            stderr,
            "mkdir: cannot create directory '%s': File exits\n",
            r_path
          );
          break;

        case ENOENT:
          fprintf(
            stderr,
            "mkdir: cannot create directory '%s': No such file or directory\n",
            r_path
          );
      }

      ERR_STATUS = 1;
      continue;
    }

    if (flag.verbose)
      printf("mkdir: created directory '%s'\n", r_path);
  }

  return 0;
}
