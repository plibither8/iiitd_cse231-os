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

int main() {
  int socket_fd;
  char buf[MAX_STR_LEN];
  struct sockaddr_in dest_addr;

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

  int n, len;
  char *str = "Mihir";

  FILE *para_file = fopen("para1.txt", "r");
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

      sendto(
        socket_fd,
        token,
        token_len + 1,
        MSG_CONFIRM,
        (const struct sockaddr *) &dest_addr,
        sizeof(dest_addr)
      );
    }
    while (token = strtok_r(NULL, " ", &saveptr));
  }

  char *end_signal = "%END%";
  sendto(
    socket_fd,
    end_signal,
    strlen(end_signal) + 1,
    MSG_CONFIRM,
    (const struct sockaddr *) &dest_addr,
    sizeof(dest_addr)
  );

  fclose(para_file);
  return 0;
}
