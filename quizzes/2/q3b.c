#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_STR_LEN 500
#define PORT 3000
#define FILENAME "para2.txt"

struct pkt {
  char token[MAX_STR_LEN];
  char file[MAX_STR_LEN];
};

int main() {
  int socket_fd;
  struct sockaddr_in dest_addr;
  struct pkt pkt;
  strcpy(pkt.file, FILENAME);

  socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (socket_fd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // Setting up destination server address struct
  memset(&dest_addr, 0, sizeof(dest_addr));
  dest_addr.sin_port = htons(PORT);
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_addr.s_addr = INADDR_ANY;

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
        token_len--;
      }

      strcpy(pkt.token, token);

      sendto(
        socket_fd,
        (const void *) &pkt,
        sizeof(pkt),
        MSG_CONFIRM,
        (const struct sockaddr *) &dest_addr,
        sizeof(dest_addr)
      );
    }
    while (token = strtok_r(NULL, " ", &saveptr));
  }

  fclose(para_file);
  return 0;
}
