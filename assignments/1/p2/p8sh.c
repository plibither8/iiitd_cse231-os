#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wordexp.h>

#define MAX_INP_SIZE 300
#define HISTORY_FILE "./.p8sh_history"

typedef struct WD {
  char current[MAX_INP_SIZE];
  char previous[MAX_INP_SIZE];
} WD;

int echo(int argc, char *argv[])
{
  struct flags
  {
    int no_newline;
    int disable_interpret;
  };

  char *tokens[argc];
  int token_count = 0;
  int parse_options = 1;
  struct flags flag = { 0, 0 };

  for (int i = 0; i < argc; i++)
  {
    char *arg = argv[i];

    // Single dash keyword letter options
    if (parse_options && strlen(arg) > 1 && arg[0] == '-')
    {
      int is_token = 0;
      struct flags buffer = { 0, 0 };
      for (int j = 1; j < strlen(arg); j++)
      {
        char letter = arg[j];
        if (letter == 'n') buffer.no_newline = 1;
        else if (letter == 'e') buffer.disable_interpret = 1;
        else {
          is_token = 1;
          buffer = (struct flags) { 0, 0 };
          break;
        }
      }
      flag.no_newline |= buffer.no_newline;
      flag.disable_interpret |= buffer.disable_interpret;
      if (!is_token)
        continue;
    }

    // This argument is not an option/flag its a path to be cat'ed
    tokens[token_count++] = arg;
    parse_options = 0;
  }

  for (int i = 0; i < token_count; i++)
    printf("%s ", tokens[i]);

  if (!flag.no_newline)
    printf("\n");

  return 0;
}

FILE *open_history(char *mode)
{
  FILE *fd = fopen(HISTORY_FILE, mode);
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
    fd = freopen(HISTORY_FILE, "w", fd);
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

int pwd(char cwd[], int argc, char *argv[])
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
    printf("%s\n", realpath(cwd, NULL));
  else
    printf("%s\n", cwd);

  return 0;
}

void cd_up(char *a_path)
{
  int index = strlen(a_path) - 1;
  while(a_path[index] != '/' && index >= 0)
    index--;
  a_path[index] = '\0';
}

int cd(WD *w_dirs, int argc, char *argv[])
{
  int print_physical = 0;
  int arg_count = 0;
  char *r_target = (char *) calloc(MAX_INP_SIZE, sizeof(char));
  char *r_target_save = (char *) calloc(MAX_INP_SIZE, sizeof(char));
  char a_target[MAX_INP_SIZE];
  strcpy(a_target, w_dirs->current);

  for (int i = 0; i < argc; i++)
  {
    if (arg_count > 0)
    {
      fprintf(stderr, "p8sh: cd: too many arguments\n");
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
    strcpy(a_target, w_dirs->previous);
  else
  {
    while (1)
    {
      if (
        strcmp(r_target, "..") == 0 ||
        r_target[0] == '.' && r_target[1] == '.' && r_target[2] == '/'
      )
      {
        cd_up(a_target);
        r_target += strlen(r_target) == 2 ? 2 : 3;
      }
      else if (
        strcmp(r_target, ".") == 0 ||
        r_target[0] == '.' && r_target[1] == '/'
      )
      {
        r_target += strlen(r_target) == 1 ? 1 : 2;
      }
      else break;
    }
  }

  if (strlen(r_target))
  {
    if (a_target[strlen(a_target) - 1] != '/')
      strcat(a_target, "/");
    strcat(a_target, r_target);
  }

  if (print_physical)
    strcpy(a_target, realpath(a_target, NULL));

  DIR *dir = opendir(a_target);
  if (!dir)
  {
    fprintf(stderr, "p8sh: cd: no such file or directory: %s\n", a_target);
    closedir(dir);
    return 1;
  }

  strcpy(w_dirs->previous, w_dirs->current);
  strcpy(w_dirs->current, a_target);

  closedir(dir);
  return 0;
}

int fork_and_exec(char cmd[], 
  int argc, char *argv[], char *cwd)
{
  char **n_argv = malloc((argc + 2) * sizeof(*n_argv));
  memmove(&n_argv[1], argv, sizeof(*n_argv) * argc);
  n_argv[0] = cwd;

  int child_pid = fork();
  if (child_pid < 0)
  {
    fprintf(stderr, "ERR! Could not fork parent process\n");
    exit(EXIT_FAILURE);
  }
  else if (child_pid == 0)
  {
    int err = execvp(cmd, n_argv) == -1;

    char *errmsg = (char *) calloc(200, sizeof(char));
    snprintf(errmsg, 200, "%s", n_argv[1]);
    perror(errmsg);

    free(errmsg);
    exit(err);
  }
  else {
    wait(NULL);
    return 0;
  }
}

int run_internal(WD *w_dirs, wordexp_t we)
{
  char *cmd_name = we.we_wordv[0];
  char **argv = &we.we_wordv[1];
  int argc = we.we_wordc - 1;

  if (strcmp(cmd_name, "cd") == 0)
    return cd(w_dirs, argc, argv);

  if (strcmp(cmd_name, "echo") == 0)
    return echo(argc, argv);

  if (strcmp(cmd_name, "history") == 0)
    return history(argc, argv);

  if (strcmp(cmd_name, "pwd") == 0)
    return pwd(w_dirs->current, argc, argv);

  if (strcmp(cmd_name, "exit") == 0)
  {
    int exit_code = argc > 0 ? atoi(argv[0]) : 0;
    exit(exit_code);
  }

  return -1;
}

int run_external(char cwd[], wordexp_t we)
{
  char *cmd_name = we.we_wordv[0];
  int argc = we.we_wordc;
  char **argv = we.we_wordv;
  char *cwdcpy = strdup(cwd);

  if (strcmp(cmd_name, "ls") == 0)
  {
    fork_and_exec("./bin/ls", argc, argv, cwdcpy);
    return 0;
  }

  if (strcmp(cmd_name, "cat") == 0)
  {
    fork_and_exec("./bin/cat", argc, argv, cwdcpy);
    return 0;
  }

  if (strcmp(cmd_name, "mkdir") == 0)
  {
    fork_and_exec("./bin/mkdir", argc, argv, cwdcpy);
    return 0;
  }

  if (strcmp(cmd_name, "rm") == 0)
  {
    fork_and_exec("./bin/rm", argc, argv, cwdcpy);
    return 0;
  }

  if (strcmp(cmd_name, "date") == 0)
  {
    fork_and_exec("./bin/date", argc, argv, cwdcpy);
    return 0;
  }

  return -1;
}

void print_header()
{
  printf("Welcome to\n\n");
  printf("██████   █████  ███████ ██   ██\n");
  printf("██   ██ ██   ██ ██      ██   ██\n");
  printf("██████   █████  ███████ ███████\n");
  printf("██      ██   ██      ██ ██   ██\n");
  printf("██       █████  ███████ ██   ██\n\n");
  printf("Available commands:\n\tcd, pwd, history, echo, ls, cat, date, mkdir, rm, exit\n\n");
}

int main(int argc, char *argv[])
{
  WD working_dirs;
  getcwd(working_dirs.current, MAX_INP_SIZE);
  strcpy(working_dirs.previous, working_dirs.current);

  print_header();

  do
  {
    printf("\n%s ❯ ", working_dirs.current);
    char command[MAX_INP_SIZE];

    fgets(command, MAX_INP_SIZE, stdin);
    command[strlen(command) - 1] = '\0';

    if(command[0] == '\0' || command[0] == '\n')
      continue;

    add_to_history(command);

    wordexp_t we;
    int we_ret = wordexp(command, &we, 0);

    if (we_ret)
    {
      wordfree(&we);
      continue;
    }

    if (!we.we_wordc)
    {
      wordfree(&we);
      continue;
    }

    if (
      run_internal(&working_dirs, we) == -1 &&
      run_external(working_dirs.current, we) == -1
    )
    {
      fprintf(stderr, "p8sh: %s: command not found\n", we.we_wordv[0]);
    }

    wordfree(&we);
  }
  while(1);

  return 0;
}
