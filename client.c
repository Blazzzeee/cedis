#include "utils.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {

  int fd, status, rv;

  struct addrinfo hints, *res;
  struct sockaddr_in bind_addr;

  memset(&hints, 0, sizeof(hints));

  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  // TODO: Support IPv6 later
  hints.ai_family = AF_INET;

  socklen_t sin_size;

  if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
    // TODO:Iterate through this linked list
    fprintf(stderr, "gai error getaddrinfo: %s", gai_strerror(status));
  }

  if ((fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
    fprintf(stderr, "err socket: cannot request fd");
  }

  if (connect(fd, res->ai_addr, res->ai_addrlen) == -1) {
    perror("connect: ");
  }

  uint32_t header;
  char *msg, payload[PAYLOAD_LEN] = {0};
  msg = "PING";

  header = strnlen(msg, PAYLOAD_LEN);

  if (header + HEADER_LEN > PAYLOAD_LEN) {
    fprintf(stderr, "payload exceeds 4096 bytes, skipping ...");
    return 1;
  }

  // this doesnt have endianess
  memcpy(payload, &header, HEADER_LEN);
  // the msg should be given an offset of 4 bytes
  memcpy(payload + HEADER_LEN, msg, header);

  if ((rv = write(fd, payload, PAYLOAD_LEN) < 0)) {
    perror("write");
  }

  char reply[PAYLOAD_LEN];
  rv = read(fd, reply, PAYLOAD_LEN);
  debugger;

  return 0;
}
