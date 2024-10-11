#ifndef READFULLY_H
#define READFULLY_H

#include <stdio.h>
ssize_t read_fully(int fd, void *buf, size_t count, int *err);

#endif    // READFULLY_H
