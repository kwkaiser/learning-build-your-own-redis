#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
int read_full(int connection_fd, char *buf, size_t n);
int write_full(int connection_fd, char *buf, size_t n);
#endif