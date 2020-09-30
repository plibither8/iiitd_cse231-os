#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wordexp.h>

#include "global.h"
#include "internal/cd.h"
#include "internal/echo.h"
#include "internal/history.h"
#include "internal/pwd.h"
#include "internal/exit.h"

int fork_and_exec(char cmd[], int argc, char *argv[])
{
  int child_pid = fork();
  if (child_pid < 0)
  {
    fprintf(stderr, RED "ERR! Could not fork parent process\n" RESET);
    exit(EXIT_FAILURE);
  }
  else if (child_pid == 0)
  {
    char abs_cmd[MAX_INP_SIZE];
    strcpy(abs_cmd, working_dirs.start);
    strcat(abs_cmd, cmd);

    int err = execvp(abs_cmd, argv) == -1;

    char *errmsg = (char *) calloc(200, sizeof(char));
    snprintf(errmsg, 200, "%s", argv[0]);
    perror(errmsg);

    free(errmsg);
    _exit(err);
  }
  else {
    int exit_status;
    waitpid(child_pid, &exit_status, 0);
    return exit_status != 0;
  }
}

int check_run_command(wordexp_t we)
{
  char *cmd_name = we.we_wordv[0];
  int argc = we.we_wordc;
  char **argv = we.we_wordv;

  if (strcmp(cmd_name, "cd") == 0)
    return cd(argc - 1, &argv[1]);

  if (strcmp(cmd_name, "echo") == 0)
    return echo(argc - 1, &argv[1]);

  if (strcmp(cmd_name, "history") == 0)
    return history(argc - 1, &argv[1]);

  if (strcmp(cmd_name, "pwd") == 0)
    return pwd(argc - 1, &argv[1]);

  if (strcmp(cmd_name, "exit") == 0)
    return exit_wrp(argc - 1, &argv[1]);

  if (strcmp(cmd_name, "ls") == 0)
    return fork_and_exec("/bin/ls", argc, argv);

  if (strcmp(cmd_name, "cat") == 0)
    return fork_and_exec("/bin/cat", argc, argv);

  if (strcmp(cmd_name, "mkdir") == 0)
    return fork_and_exec("/bin/mkdir", argc, argv);

  if (strcmp(cmd_name, "rm") == 0)
    return fork_and_exec("/bin/rm", argc, argv);

  if (strcmp(cmd_name, "date") == 0)
    return fork_and_exec("/bin/date", argc, argv);

  return -1;
}

void print_header()
{
  printf("\e[1;1H\e[2J");
  printf(BLU "Welcome to\n\n" RESET);
  printf(GRN "██████   █████  ███████ ██   ██\n");
  printf("██   ██ ██   ██ ██      ██   ██\n");
  printf("██████   █████  ███████ ███████\n");
  printf("██      ██   ██      ██ ██   ██\n");
  printf("██       █████  ███████ ██   ██\n\n" RESET);
  printf(
    BLU "Available commands:" RESET
    YEL "\n\tcd, pwd, history, echo, ls, cat, date, mkdir, rm, exit\n\n"
    RESET
  );
}

int main(int argc, char *argv[])
{
  // Initial setup
  getcwd(working_dirs.current, MAX_INP_SIZE);
  strcpy(working_dirs.previous, working_dirs.current);
  strcpy(working_dirs.start, working_dirs.current);

  print_header();

  int ERR = 0;
  do
  {
    printf(BLU "\n%s%s\np8sh ❯  " RESET, working_dirs.current, ERR == 0 ? GRN : RED);
    char command[MAX_INP_SIZE];

    fgets(command, MAX_INP_SIZE, stdin);
    command[strlen(command) - 1] = '\0';

    if(command[0] == '\0' || command[0] == '\n')
      continue;

    add_to_history(command);

    wordexp_t we;
    int we_ret = wordexp(command, &we, 0);

    if (we_ret || !we.we_wordc)
    {
      wordfree(&we);
      continue;
    }

    if ((ERR = check_run_command(we)) == -1)
      fprintf(stderr, RED "p8sh: %s: command not found\n%s", we.we_wordv[0], RESET);

    wordfree(&we);
  }
  while(1);

  return 0;
}
