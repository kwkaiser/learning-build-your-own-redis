#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

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

  char msg[] = "hello";
  write(fd, msg, strlen(msg));

  char read_buffer[64] = {};
  ssize_t byte_count = read(fd, read_buffer, sizeof(read_buffer) - 1);
  if (byte_count < 0)
  {
    printf("Read error\n");
    return 1;
  }

  printf("Server says: %s\n", read_buffer);
  close(fd);
}