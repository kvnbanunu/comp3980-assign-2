#ifndef SEND_H
#define SEND_H

#include <unistd.h>

ssize_t send(int fd, const char *buf, size_t size, int *err);

#endif    // SEND_H
