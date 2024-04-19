#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include "common.h"

extern const size_t max_msg_bytes;

int handle_one_request(int connection_fd)
{
  // Read first 4 bytes into header buffer
  char read_buffer[4 + max_msg_bytes + 1];
  int err = read_full(connection_fd, read_buffer, 4);

  if (err)
  {
    if (err == 0)
    {
      printf("EOF\n");
    }
    else
    {
      printf("read_full() error\n");
    }
    return err;
  }

  uint32_t len = 0;
  memcpy(&len, read_buffer, 4); // assumes little endian
  if (len > max_msg_bytes)
  {
    printf("Message too long: %d\n", len);
    return -1;
  }

  // get body, reading into the 5th byte onward of buffer
  err = read_full(connection_fd, &read_buffer[4], len);
  if (err)
  {
    printf("read_full() error\n");
    return err;
  }

  // Null terminate buffer
  read_buffer[4 + len] = '\0';
  printf("Client says %s\n", &read_buffer[4]);

  // Reply using same protocol
  const char reply[] = "world";
  char write_buffer[4 + sizeof(reply)];
  len = strlen(reply);

  // Copy length to first 4 bytes
  memcpy(write_buffer, &len, 4);
  // Copy content to remaining bytes
  memcpy(&write_buffer[4], reply, len);
  return write_full(connection_fd, write_buffer, 4 + len);
}

void handle_connection(int connection_fd)
{
  char read_buffer[64] = {};
  ssize_t byte_count = read(connection_fd, read_buffer, sizeof(read_buffer) - 1);

  if (byte_count < 0)
  {
    printf("read() error\n");
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
    struct sockaddr_in client_addr = {};
    socklen_t socket_length = sizeof(client_addr);
    int connection_fd = accept(fd, (struct sockaddr *)&client_addr, &socket_length);

    if (connection_fd < 0)
    {
      printf("Error connecting\n");
      continue;
    }

    while (1)
    {
      int32_t err = handle_one_request(connection_fd);
      if (err)
      {
        break;
      }
    }

    handle_connection(connection_fd);
    close(connection_fd);
  }
}