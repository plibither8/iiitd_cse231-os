#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define MAX_STR_SIZE 500

int main(int argc, char** argv) {
  int first_pipe_fds[2];
  int second_pipe_fds[2];

  if (pipe(first_pipe_fds) < 0 || pipe(second_pipe_fds) < 0) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  char input[MAX_STR_SIZE];
  scanf("%[^\n]%*c", input);

  pid_t pid = fork();

  switch (pid) {
    case -1: {
      perror("fork");
      exit(EXIT_FAILURE);
    }

    // Child process
    case 0: {
      char upper[MAX_STR_SIZE];
      char buf[MAX_STR_SIZE];
      int pos = 0;

      close(first_pipe_fds[1]);
      read(first_pipe_fds[0], buf, MAX_STR_SIZE);

      int i = 0;
      for (; i < strlen(buf); i++) {
        upper[i] = toupper((unsigned char) buf[i]);
      }
      upper[i] = '\0';

      close(first_pipe_fds[0]);

      close(second_pipe_fds[0]);
      write(second_pipe_fds[1], upper, sizeof(upper));
      close(second_pipe_fds[1]);

      break;
    }

    // Parent process
    default: {
      close(first_pipe_fds[0]);
      write(first_pipe_fds[1], input, strlen(input));
      close(first_pipe_fds[1]);

      wait(NULL);

      char output[MAX_STR_SIZE];

      close(second_pipe_fds[1]);
      read(second_pipe_fds[0], output, MAX_STR_SIZE);
      close(second_pipe_fds[0]);

      printf("%s", output);
    }
  }

  return 0;
}
