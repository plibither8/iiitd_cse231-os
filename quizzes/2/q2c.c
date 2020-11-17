#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

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
  int msqid = msgget(key, MSGQ_PERMS); // Create the queue

  if (msqid == -1) {
    perror("msgget");
    exit(EXIT_FAILURE);
  }

  int end_count = 0;

  while (1) {
    if (msgrcv(msqid, &message, sizeof(message.mtext), 0, 0) == -1) {
      perror("msgrcv");
      exit(1);
    }

    if (!strcmp(message.mtext, "%END%")) {
      if (++end_count == 2) break;
    } else {
      printf("%s ", message.mtext);
    }
  }

  strcpy(message.mtext, "%FIN%");
  msgsnd(msqid, &message, 6, 0);

  if (msgctl(msqid, IPC_RMID, NULL) == -1) {
    perror("msgctl");
    exit(1);
  }

  return 0;
}
