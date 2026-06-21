#include "utils.h"
#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static int32_t read_all(int fd, char *buf, int n) {
  int rv = 0;

  // more reads expected
  while (n > 0) {
    rv = read(fd, buf, n);

    if (rv <= 0) {
      return -1;
    }

    // prepate for next read
    assert(n >= rv);
    n -= rv;

    // offset the pointer
    buf += rv;
  }

  return 0;
}

static int32_t write_all(int fd, const char *buf, size_t n) {
  int rv = 0;

  // more writes expeccted
  while (n > 0) {
    rv = write(fd, buf, n);

    if (rv <= 0) {
      fprintf(stderr, "error write_all");
      return -1;
    }

    assert(n >= rv);
    n -= rv;

    buf += rv;
  }

  return 0;
}

int32_t one_request(int fd) {
  debugger;
  uint32_t header_buf;
  // this should be replaced by read all
  int rv = read_all(fd, (char *)&header_buf, HEADER_LEN);

  if (rv != 0) {
    fprintf(stderr, "Corrupt Header, skipping payload");
    return -1;
    close(fd);
  }

  char buf[PAYLOAD_LEN];
  // this should be readall
  rv = read_all(fd, (char *)&buf, header_buf);
  if (rv != 0) {
    fprintf(stderr, "corrupt payload, skipping");
    close(fd);
  }

  if (strncmp(buf, "PING", header_buf) != 0) {
    fprintf(stderr, "unknown cmd, skipping reply");
    return -1;
  }

  char payload[PAYLOAD_LEN] = {0}, *msg = "PONG";
  uint32_t header_size = strnlen(msg, PAYLOAD_LEN);

  // no endianess here for now
  memcpy(payload, &header_size, HEADER_LEN);
  memcpy(payload + HEADER_LEN, msg, header_size);

  if (write_all(fd, (const char *)payload, PAYLOAD_LEN) != 0) {
    fprintf(stderr, "couldnt push data to client, terminating..");
    return -1;
  }

  return 0;
}

int main() {

  int fd, status;

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

  const int yes = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(&yes));

  // Bind the part to machine address
  if (bind(fd, res->ai_addr, res->ai_addrlen) == -1) {
    // close(fd);
    perror("server: bind\n");
  }

  if (listen(fd, 1000) != 0) {
    perror("err listen: ");
  };
  // TODO: deserialise byte stream
  //
  // TODO: event-loop()
  while (true) {
    struct sockaddr_storage their_addr;
    socklen_t addrlen = sizeof(their_addr);
    int connfd = accept(fd, (struct sockaddr *)&their_addr, &addrlen);

    if (connfd < 0) {
      continue;
    }

    // serve the client
    int32_t err = one_request(connfd);
  }

  return 0;
}
