#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include "common.h"

extern const size_t max_msg_bytes;

int query(int connection_fd, const char *text)
{
  uint32_t len = strlen(text);
  if (len > max_msg_bytes)
  {
    printf("Message too long: %d\n", len);
    return -1;
  }

  char write_buffer[4 + max_msg_bytes];
  memcpy(write_buffer, &len, 4);
  memcpy(&write_buffer[4], text, len);
  int err = write_full(connection_fd, write_buffer, 4 + len);
  if (err)
  {
    printf("Error writing\n");
    return err;
  }

  char read_buffer[4 + max_msg_bytes + 1];
  err = read_full(connection_fd, read_buffer, 4);
  if (err)
  {
    if (err == 0)
    {
      printf("EOF\n");
    }
    else
    {
      printf("client read_full() error\n");
    }
    return err;
  }

  memcpy(&len, read_buffer, 4);
  if (len > max_msg_bytes)
  {
    printf("Message too long:%d\n", len);
    return -1;
  }

  err = read_full(connection_fd, &read_buffer[4], len);
  if (err)
  {
    printf("client read_full() error\n");
    return err;
  }

  // null terminate reply data
  read_buffer[4 + len] = '\0';
  printf("Server says: %s\n", &read_buffer[4]);
  return 0;
}

int main()
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
  {
    printf("socket() error: %d\n", fd);
    return 1;
  }

  struct sockaddr_in addr =
      {
          .sin_family = AF_INET,
          .sin_port = ntohs(1234),
          .sin_addr = ntohl(INADDR_LOOPBACK)};

  int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
  if (rv)
  {
    printf("connect error\n");
    return 1;
  }

  query(fd, "What's going on you ugly son of a gun");
  close(fd);
}