#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT "35000"

int main() {

  int fd, status;

  struct addrinfo hints, *res;
  struct sockaddr_in bind_addr;

  struct sockaddr_storage their_addr;

  memset(&hints, 0, sizeof(hints));

  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_INET;

  socklen_t sin_size;

  if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
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
  sin_size = sizeof(their_addr);
  // TODO: event-loop()
  int new_fd = accept(fd , (struct sockaddr *)&their_addr , &sin_size);
  // TODO: deserialise byte stream
  //
  // TODO: primitive event loop
  // while (true) {

  //   poll(struct pollfd * fds, nfds_t nfds, int timeout)
  // }

  return 0;
}
