#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../global.h"

WD working_dirs;

void cd_free(char *a, char *b)
{
  free(a);
  free(b);
}

int parse_cd(char *a_target, char *r_target)
{
  char *copy = strdup(r_target);
  char *ptr = copy;
  for (int i = 0; copy[i]; i++)
  {
    *ptr++ = r_target[i];
    if (r_target[i] == '/')
    {
      i++;
      while (r_target[i] == '/') i++;
      i--;
    }
  }
  *ptr = '\0';
  strcpy(r_target, copy);

  if (r_target[0] == '/')
  {
    strcpy(a_target, r_target);
    r_target[0] = '\0';
  }
  else
  {
    if (a_target[strlen(a_target) - 1] != '/')
      strcat(a_target, "/");
    strcat(a_target, r_target);
  }

  if (strcmp(a_target, "/") == 0)
    return 0;

  char new_path[MAX_INP_SIZE] = "/";

  char *saveptr;
  char *delim = "/";
  char *token = strtok_r(a_target, delim, &saveptr);

  do
  {
    if (strcmp(token, ".") == 0) continue;
    else if (strcmp(token, "..") == 0)
    {
      if (strcmp(new_path, "/") == 0) continue;
      int index = strlen(new_path) - 1;
      while (new_path[index] != '/' && index >= 0) index--;
      new_path[index] = '\0';
    }
    else
    {
      if (new_path[strlen(new_path) - 1] != '/')
        strcat(new_path, "/");
      strcat(new_path, token);
    }

    if (!strlen(new_path))
      strcpy(new_path, "/");

    struct stat statbuf;
    if (stat(new_path, &statbuf) != 0) return 1;
    if (!S_ISDIR(statbuf.st_mode)) return 2;
  }
  while (token = strtok_r(NULL, delim, &saveptr));

  strcpy(a_target, new_path);
  return 0;
}

int cd(int argc, char *argv[])
{
  int print_physical = 0;
  int arg_count = 0;
  char *r_target = (char *) calloc(MAX_INP_SIZE, sizeof(char));
  char *r_target_save = (char *) calloc(MAX_INP_SIZE, sizeof(char));
  char a_target[MAX_INP_SIZE];
  strcpy(a_target, working_dirs.current);

  for (int i = 0; i < argc; i++)
  {
    if (arg_count > 0)
    {
      fprintf(stderr, "p8sh: cd: too many arguments\n");
      cd_free(r_target, r_target_save);
      return 1;
    }

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
          fprintf(stderr, "p8sh: cd: -'%c': invalid option\n", letter);
          cd_free(r_target, r_target_save);
          return 1;
        }
      }
      continue;
    }

    r_target = strdup(arg);
    r_target_save = strdup(arg);
    arg_count++;
  }

  if (strcmp(r_target, "-") == 0)
    strcpy(a_target, working_dirs.previous);
  else
  {
    int err;
    if (err = parse_cd(a_target, r_target))
    {
      switch (err)
      {
        case 1:
          fprintf(stderr, "p8sh: cd: no such file or directory: %s\n", r_target_save);
          break;
        case 2:
          fprintf(stderr, "p8sh: cd: not a directory: %s\n", r_target_save);
      }
      cd_free(r_target, r_target_save);
      return 1;
    }
  }

  if (print_physical)
    strcpy(a_target, realpath(a_target, NULL));

  struct stat statbuf;
  if (stat(a_target, &statbuf) != 0)
  {
    fprintf(stderr, "p8sh: cd: no such file or directory: %s\n", r_target_save);
    cd_free(r_target, r_target_save);
    return 1;
  }
  if (!S_ISDIR(statbuf.st_mode))
  {
    fprintf(stderr, "p8sh: cd: not a directory: %s\n", r_target_save);
    cd_free(r_target, r_target_save);
    return 1;
  }

  strcpy(working_dirs.previous, working_dirs.current);
  strcpy(working_dirs.current, a_target);
  chdir(working_dirs.current);

  cd_free(r_target, r_target_save);
  return 0;
}
