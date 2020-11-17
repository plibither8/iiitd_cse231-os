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

struct message {
  long mtype;
  char mtext[MAX_STR_LEN];
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

  FILE *para_file = fopen("para1.txt", "r");
  char line[MAX_STR_LEN];
  int file_length;

  fseek(para_file, 0, SEEK_END);
  file_length = ftell(para_file);
  fseek(para_file, 0, SEEK_SET);

  printf("%d\n", file_length);

  while (fgets(line, file_length + 1, para_file)) {
    char *saveptr;
    char *token = strtok_r(line, " ", &saveptr);

    do {
      strcpy(message.mtext, token);
      printf("%s\n", token);
      if (msgsnd(msqid, &message, strlen(token) + 1, 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
      }
    }
    while (token = strtok_r(NULL, " ", &saveptr));
  }

  strcpy(message.mtext, "%END%");
  if (msgsnd(msqid, &message, 6, 0) == -1) {
    perror("msgsnd");
    exit(EXIT_FAILURE);
  }

  return 0;
}
