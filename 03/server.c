#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

void do_something(int connection_fd)
{
  char read_buffer[64] = {};
  ssize_t byte_count = read(connection_fd, read_buffer, sizeof(read_buffer) - 1);
  printf("Going to do something\n");

  if (byte_count < 0)
  {
    printf("read() error in do something\n");
    return;
  }

  printf("Client says: %s\n", read_buffer);
  char write_buffer[] = "world";
  write(connection_fd, write_buffer, strlen(write_buffer));
}

int main()
{
  // Build connection info
  struct sockaddr_in addr = {
      .sin_family = AF_INET,
      .sin_port = ntohs(1234),
      .sin_addr = ntohl(0),
  };

  // Create file descriptor, enable repeated use of the same port
  int val = 1;
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

  // Bind to port
  int rv = bind(fd, (struct sockaddr *)&addr, sizeof(addr));

  if (rv)
  {
    printf("bind(): rv: %d\n", rv);
    return 1;
  }

  // Listen to port
  rv = listen(fd, SOMAXCONN);
  if (rv)
  {
    printf("listen(): rv: %d\n", rv);
    return 1;
  }

  // loop through connections
  while (1)
  {
    printf("Listening...\n");
    struct sockaddr_in client_addr = {};
    socklen_t socket_length = sizeof(client_addr);
    int connection_fd = accept(fd, (struct sockaddr *)&client_addr, &socket_length);
    printf("Accepted!\n");

    if (connection_fd < 0)
    {
      printf("Error connecting\n");
      continue;
    }

    do_something(connection_fd);
    close(connection_fd);
  }
}