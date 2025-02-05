#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define MAX_STR_LEN 500
#define MSGQ_PERMS 0644
#define FILENAME "para1.txt"

struct message {
  long mtype;
  char mtext[MAX_STR_LEN];
  char mfile[MAX_STR_LEN];
};

int main(int argc, char** argv) {
  struct message message;
  message.mtype = 1;

  key_t key = 31415; // Random, unique key
  int msqid = msgget(key, MSGQ_PERMS | IPC_CREAT); // Create the queue

  if (msqid == -1) {
    perror("msgget");
    exit(EXIT_FAILURE);
  }

  FILE *para_file = fopen(FILENAME, "r");
  char line[MAX_STR_LEN];
  int file_length;

  fseek(para_file, 0, SEEK_END);
  file_length = ftell(para_file);
  fseek(para_file, 0, SEEK_SET);

  while (fgets(line, file_length + 1, para_file)) {
    char *saveptr;
    char *token = strtok_r(line, " ", &saveptr);
    int token_len;

    do {
      token_len = strlen(token);

      if (token[token_len - 1] == '\n') {
        token[token_len - 1] = '\0';
      }

      strcpy(message.mtext, token);
      strcpy(message.mfile, FILENAME);
      if (msgsnd(msqid, &message, sizeof(message), 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
      }
    }
    while (token = strtok_r(NULL, " ", &saveptr));
  }

  fclose(para_file);
  return 0;
}
