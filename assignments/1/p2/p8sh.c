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

#define MAX_INP_SIZE 200
#define HISTORY_FILE "./.p8sh_history"

int is_dir(char *path)
{
  DIR *dir = opendir(path);
  if (dir)
  {
    closedir(dir);
    return 1;
  }
  else if (ENOENT == errno)
    return 0;
  return 0;
}

int echo_token(char *token, int interpret_backslash)
{
  if (interpret_backslash)
    printf("%s ", token);
  else
  {
    for (int i = 0; i < strlen(token); i++)
    {
      char c = token[i];
      if (isprint(c))
        putchar(c);
      else {
        switch (c)
        {
          case '\\':
            printf("\\");
            break;
          case '\b':
            printf("\\b");
            break;
          case '\f':
            printf("\\f");
            break;
          case '\n':
            printf("\\n");
            break;
          case '\r':
            printf("\\r");
            break;
          case '\t':
            printf("\\t");
            break;
          case '\v':
            printf("\\v");
            break;
          default:
            printf("%02x", c);
        }
      }
    }
    printf(" ");
  }
}

int echo(int argc, char *argv[])
{
  struct flags
  {
    int no_newline;
    int interpret_backslash;
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
        else if (letter == 'e') buffer.interpret_backslash = 1;
        else {
          is_token = 1;
          buffer = (struct flags) { 0, 0 };
          break;
        }
      }
      flag.no_newline |= buffer.no_newline;
      flag.interpret_backslash |= buffer.interpret_backslash;
      if (!is_token)
        continue;
    }

    // This argument is not an option/flag its a path to be cat'ed
    tokens[token_count++] = arg;
    parse_options = 0;
  }

  for (int i = 0; i < token_count; i++)
  {
    echo_token(tokens[i], flag.interpret_backslash);
  }

  if (!flag.no_newline)
    printf("\n");

  return 0;
}

int get_history_count()
{
  FILE *fd = fopen(HISTORY_FILE, "r");
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
  FILE *fd = fopen(HISTORY_FILE, "a");
  if (ftell(fd) > 0)
    fprintf(fd, "\n");
  fprintf(fd, "%s", cmd);
  fflush(fd);
  fclose(fd);
}

void clear_history()
{
  FILE *fd = fopen(HISTORY_FILE, "w");
  fclose(fd);
  return;
}

int delete_history_offset(int offset)
{
  FILE *fd = fopen(HISTORY_FILE, "r+");

  fseek(fd, 0, SEEK_END);
  long file_length = ftell(fd);
  fseek(fd, 0, SEEK_SET);

  char *new_content = (char *) calloc(file_length, sizeof(char));
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

  free(new_content);
  fclose(fd);
  return 0;
}

void print_history_line_number(int line_number)
{
  int number_length = floor(log10(line_number)) + 1;
  int padsize = 6 - number_length;

  for (int i = 0; i < padsize; i++)
    printf(" ");

  printf("%d  ", line_number);
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
      clear_history();
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

  FILE *fd = fopen(HISTORY_FILE, "r");

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
      print_history_line_number(count);
      printf("%s", line);
    }

    free(line);
  }

  fclose(fd);
  return 0;
}

int pwd(char *cwd, int argc, char *argv[])
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

int cd(char *cwd, int argc, char *argv[])
{
  int print_physical = 0;
  int arg_count = 0;
  char *r_target = (char *) calloc(MAX_INP_SIZE, sizeof(char));
  char *r_target_save = (char *) calloc(MAX_INP_SIZE, sizeof(char));
  char *a_target = strdup(cwd);

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

  if (strcmp(r_target, ".") == 0)
    a_target = a_target;
  else if (strcmp(r_target, "..") == 0)
    cd_up(a_target);
  else
  {
    while (r_target[0] == '.' && r_target[1] == '.' && r_target[2] == '/')
    {
      cd_up(a_target);
      r_target += 3;
    }

    if (strlen(r_target))
    {
      if (r_target[0] != '/')
      {
        strcat(a_target, "/");
        strcat(a_target, r_target);
      }
      else
        strcpy(a_target, r_target);
    }
  }

  if (print_physical)
    a_target = realpath(a_target, NULL);

  if (!is_dir(a_target))
  {
    fprintf(stderr, "p8sh: cd: no such file or directory: %s\n", r_target_save);
    return 1;
  }

  strcpy(cwd, a_target);
  return 0;
}

void print_header()
{
  printf("Welcome to\n\n");
  printf("██████   █████  ███████ ██   ██ \n");
  printf("██   ██ ██   ██ ██      ██   ██ \n");
  printf("██████   █████  ███████ ███████ \n");
  printf("██      ██   ██      ██ ██   ██ \n");
  printf("██       █████  ███████ ██   ██ \n\n");
  printf("Available commands:\n\tcd, pwd, history, echo, ls, cat, date, mkdir, rm, exit\n\n");
}

int fork_and_exec(char *cmd, int argc, char **argv, char *cwd)
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

int check_internal(char *cwd, wordexp_t we)
{
  char *cmd_name = we.we_wordv[0];
  int argc = we.we_wordc - 1;
  char **argv = &we.we_wordv[1];

  if (strcmp(cmd_name, "cd") == 0)
    return cd(cwd, argc, argv);

  if (strcmp(cmd_name, "echo") == 0)
    return echo(argc, argv);

  if (strcmp(cmd_name, "history") == 0)
    return history(argc, argv);

  if (strcmp(cmd_name, "pwd") == 0)
    return pwd(cwd, argc, argv);

  if (strcmp(cmd_name, "exit") == 0)
  {
    int exit_code = argc > 0 ? atoi(argv[0]) : 0;
    exit(exit_code);
  }

  return -1;
}

int check_external(char *cwd, wordexp_t we)
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

int main(int argc, char *argv[])
{
  char current_directory[MAX_INP_SIZE];
  getcwd(current_directory, MAX_INP_SIZE);

  print_header();

  do
  {
    printf("\n%s ❯ ", current_directory);
    char *command = (char *) calloc(MAX_INP_SIZE, sizeof(char));

    fgets(command, MAX_INP_SIZE, stdin);
    command[strlen(command) - 1] = '\0';

    if(command[0] == '\0' || command[0] == '\n')
      continue;

    add_to_history(command);

    wordexp_t we;
    wordexp(command, &we, 0);

    if (!we.we_wordc)
    {
      free(command);
      continue;
    }

    if (
      check_internal(current_directory, we) == -1 &&
      check_external(current_directory, we) == -1
    )
    {
      fprintf(stderr, "p8sh: %s: command not found\n", we.we_wordv[0]);
    }

    free(command);
  }
  while(1);

  return 0;
}
