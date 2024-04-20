#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

const size_t max_msg_bytes = 4096;

// guarantees we've read / written exactly n bytes
int read_full(int connection_fd, char *buf, size_t n)
{
  while (n > 0)
  {
    ssize_t rv = read(connection_fd, buf, n);

    if (rv <= 0)
    {
      printf("Read full error: rv: %d\n", rv);
      return -1;
    }

    assert(rv <= n);
    n -= rv;
    buf += rv;
  }
  return 0;
}

int write_full(int connection_fd, char *buf, size_t n)
{
  while (n > 0)
  {
    ssize_t rv = write(connection_fd, buf, n);
    if (rv <= 0)
    {
      printf("Write full error: rv: %d\n", rv);
      return -1;
    }

    assert(rv <= n);
    n -= rv;
    buf += rv;
  }
  return 0;
}
