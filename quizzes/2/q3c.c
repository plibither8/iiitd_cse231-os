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

struct pkt {
  char token[MAX_STR_LEN];
  char file[MAX_STR_LEN];
};

int main() {
  int socket_fd;
  struct sockaddr_in server_addr, client_addr;
  struct pkt pkt;

  socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
  if (socket_fd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // Setting up server and client address struct
  memset(&server_addr, 0, sizeof(server_addr));
  memset(&client_addr, 0, sizeof(client_addr));
  server_addr.sin_port = htons(PORT);
  server_addr.sin_family  = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;

  int bind_res =
    bind(socket_fd, (const struct sockaddr *) &server_addr, sizeof(server_addr));
  if (bind_res < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  int token_len;
  int client_addr_len = sizeof(client_addr);
  int end_count = 0;

  while (1) {
    token_len = recvfrom(
      socket_fd,
      &pkt,
      sizeof(pkt),
      MSG_WAITALL,
      (struct sockaddr *) &client_addr,
      &client_addr_len
    );

    if (!token_len) continue;

    printf("%s: %s\n", pkt.file, pkt.token);
  }

  close(socket_fd);
  return 0;
}
