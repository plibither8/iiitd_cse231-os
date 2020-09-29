#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../global.h"

#define HISTORY_FILE "/.p8sh_history"

WD working_dirs;

FILE *open_history(char *mode)
{
  char history_file[MAX_INP_SIZE];
  strcpy(history_file, working_dirs.start);
  strcat(history_file, HISTORY_FILE);
  FILE *fd = fopen(history_file, mode);
  if (fd == NULL)
  {
    perror("history");
    exit(1);
  }
  return fd;
}

int get_history_count()
{
  FILE *fd = open_history("r");
  int count = 0;
  char ch;
  while(!feof(fd))
  {
    ch = fgetc(fd);
    if (ch == '\n')
      count++;
  }
  fclose(fd);
  return count;
}

void add_to_history(char *cmd)
{
  FILE *fd = open_history("a");
  fprintf(fd, "%s%s", ftell(fd) ? "\n" : "", cmd);
  fclose(fd);
}

int delete_history_offset(int offset)
{
  FILE *fd = open_history("r+");

  fseek(fd, 0, SEEK_END);
  long file_length = ftell(fd);
  fseek(fd, 0, SEEK_SET);

  char new_content[file_length];
  int count = 0, deleted = 0;

  while (!feof(fd))
  {
    char *line = (char *) calloc(MAX_INP_SIZE, sizeof(char));
    fgets(line, MAX_INP_SIZE, fd);
    if (++count == offset)
    {
      deleted = 1;
      continue;
    }
    strcat(new_content, line);
    free(line);
  }

  if (!deleted)
    fprintf(stderr, "p8sh: history: %d: history position out of range\n", offset);
  else
  {
    char history_file[MAX_INP_SIZE];
    strcpy(history_file, working_dirs.start);
    strcat(history_file, HISTORY_FILE);
    fd = freopen(history_file, "w", fd);
    fprintf(fd, "%s", new_content);
  }

  fclose(fd);
  return 0;
}

int history(int argc, char *argv[])
{
  int n_arg = 0;
  int n_arg_found = 0;

  for (int i = 0; i < argc; i++)
  {
    char *arg = argv[i];

    if (n_arg_found)
    {
      fprintf(stderr, "p8sh: history: too many arguments\n");
      return 1;
    }

    if (strcmp(arg, "-c") == 0)
    {
      FILE *fd = open_history("w");
      fclose(fd);
      return 0;
    }

    if (strcmp(arg, "-d") == 0)
    {
      if (i == argc - 1)
      {
        fprintf(stderr, "p8sh: history: -d: option requires an argument\n");
        return 1;
      }

      int del_offset = atoi(argv[i + 1]);
      if (delete_history_offset(del_offset))
        return 1;

      return 0;
    }

    int j = 0;
    while (isdigit(arg[j]))
      j++;

    if (strlen(arg) != j)
    {
      fprintf(stderr, "p8sh: history: %s: numeric argument required\n", arg);
      return 1;
    }

    n_arg = atoi(arg);
    n_arg_found = 1;
  }

  FILE *fd = open_history("r");

  int count = 0;
  int total_lines = get_history_count();
  while (!feof(fd))
  {
    count++;

    char *line = (char *)calloc(MAX_INP_SIZE, sizeof(char));
    fgets(line, MAX_INP_SIZE, fd);

    if (
      !n_arg_found ||
      n_arg_found && count > total_lines - n_arg + 1
    )
    {
      int number_length = floor(log10(count)) + 1;
      int padsize = 6 - number_length;

      for (int i = 0; i < padsize; i++)
        printf(" ");

      printf("%d  ", count);
      printf("%s", line);
    }

    free(line);
  }
  printf("\n");

  fclose(fd);
  return 0;
}
