#include "../include/readfully.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

ssize_t read_fully(int fd, void *buf, size_t count, int *err)
{
    size_t bytes_read = 0;
    while(bytes_read < count)
    {
        ssize_t result = read(fd, (char *)buf + bytes_read, count - bytes_read);
        if(result == 0)
        {
            break;    // EOF reached
        }
        if(result == -1)
        {
            if(errno == EINTR)
            {
                continue;    // Interrupted, retry
            }
            *err = errno;
            return -1;    // Error occurred
        }
        bytes_read += (size_t)result;
    }
    return (ssize_t)bytes_read;
}
