/*
 * Kevin Nguyen
 * A00955925
 */

#include "../include/send.h"
#include <errno.h>
#include <stdint.h>
#include <unistd.h>

ssize_t send(int fd, const char *buf, size_t size, int *err)
{
    ssize_t retval;
    size_t  nwrote;

    *err  = 0;
    errno = 0;

    nwrote = 0;

    do
    {
        ssize_t twrote;
        size_t  remaining;

        remaining = size - nwrote;
        errno     = 0;
        twrote    = write(fd, &buf[nwrote], remaining);

        if(twrote < 0)
        {
            *err   = errno;
            retval = -1;
            goto done;
        }
        nwrote += (size_t)twrote;
    } while(nwrote != size);

    retval = (ssize_t)nwrote;

done:
    return retval;
}
