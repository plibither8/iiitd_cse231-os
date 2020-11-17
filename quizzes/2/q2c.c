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

  int end_count = 0;

  while (1) {
    if (msgrcv(msqid, &message, sizeof(message.mtext), 0, 0) == -1) {
      perror("msgrcv");
      exit(1);
    }

    if (!strcmp(message.mtext, "%END%")) {
      if (++end_count == 2) break;
    } else {
      printf("%s\n", message.mtext);
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
