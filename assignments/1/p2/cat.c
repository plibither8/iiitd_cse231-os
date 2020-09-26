#include <dirent.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>

typedef struct flags
{
  int show_end;
  int number;
} flags;

int ERR_STATUS = 0;

char *inputString()
{
  int ch;
  size_t len = 0;
  size_t size = 20;

  char *str;
  str = realloc(NULL, sizeof(char) * size);
  if (!str)
    return str;

  while (ch = fgetc(stdin))
  {
    if (ch == EOF || ch == '\n')
      break;

    str[len++] = ch;
    if (len == size)
    {
      str = realloc(str, sizeof(char) * (size += 16));
      if (!str)
        return str;
    }
  }
  str[len++] = '\0';

  return realloc(str, sizeof(char) * len);
}

void print_line_number(int line_number)
{
  int number_length = floor(log10(line_number)) + 1;
  int padsize = 6 - number_length;

  for (int i = 0; i < padsize; i++)
    printf(" ");

  printf("%d  ", line_number);
}

void stdin_loop(int *line_number)
{
  while(1)
  {
    char *string = inputString();
    print_line_number((intptr_t) ++(*line_number));
    printf("%s$\n", string);

    if (feof(stdin))
      break;
  }
}

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
      if (strcmp(arg, "--show-ends") == 0) flag.show_end = 1;
      else if (strcmp(arg, "--number") == 0) flag.number = 1;
      else
      {
        fprintf(stderr, "cat: unrecognized option '%s'\n", arg);
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
        if (letter == 'E') flag.show_end = 1;
        else if (letter == 'n') flag.number = 1;
        else
        {
          fprintf(stderr, "cat: invalid option -- '%c'\n", letter);
          exit(EXIT_FAILURE);
        }
      }
      continue;
    }

    // This argument is not an option/flag its a path to be cat'ed
    paths[path_count++] = arg;
  }

  int line_number = 0;
  int new_line = 1;

  // Copy standard input to standard output
  if (path_count == 0)
  {
    stdin_loop(&line_number);
    return 0;
  }

  for (int i = 0; i < path_count; i++)
  {
    char *path = paths[i];

    if (strcmp(path, "-") == 0)
    {
      stdin_loop(&line_number);
      continue;
    }

    FILE *fd = fopen(path, "r");
    if (fd == NULL)
    {
      printf("cat: %s: No such file or directory\n", path);
      ERR_STATUS = 1;
      continue;
    }

    int c;

    while ((c = fgetc(fd)) && !feof(fd))
    {
      if (new_line && flag.number)
        print_line_number(++line_number);

      new_line = 0;

      if (c == '\n')
      {
        new_line = 1;
        if (flag.show_end)
          printf("$");
      }

      printf("%c", c);
    }

    fclose(fd);
  }

  return ERR_STATUS;
}
