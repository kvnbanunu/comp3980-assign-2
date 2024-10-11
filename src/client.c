/*
 * Kevin Nguyen
 * A00955925
 */

#include "../include/send.h"
#include "../include/usage.h"
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>
#include <unistd.h>

#define FIFO_IN "/tmp/assign2_in"
#define FIFO_OUT "/tmp/assign2_out"

int main(int argc, char *argv[])
{
    char       *buf;
    const char *message;
    int         opt;
    int         fdin;
    int         fdout;
    int         err;
    size_t      msgLength;
    int         numDigits;
    size_t      msgSize;
    ssize_t     result;
    const char *filter = NULL;

    bool close_fdin  = false;
    bool close_fdout = false;

    errno = 0;

    while((opt = getopt(argc, argv, "f:")) != -1)
    {
        if(opt == 'f')
        {
            if(strlen(optarg) != 1)
            {
                fprintf(stderr, "Error: Filter option needs to be a single char\n");
                print_usage(argv[0]);
                goto done;
            }
            if(!(strcmp(optarg, "U") == 0 || strcmp(optarg, "L") == 0 || strcmp(optarg, "N") == 0))
            {
                fprintf(stderr, "Error: Invalid filter option %s\n", optarg);
                print_usage(argv[0]);
                goto done;
            }
            filter = optarg;
        }
        else
        {
            fprintf(stderr, "Error: Invalid option flag %d\n", opt);
            print_usage(argv[0]);
            goto done;
        }
    }

    if(optind >= argc)
    {
        fprintf(stderr, "Error: Unexpected arguments\n");
        print_usage(argv[0]);
        goto done;
    }

    message   = argv[optind];
    msgLength = strlen(message);
    numDigits = snprintf(NULL, 0, "%zu", msgLength);
    msgSize   = msgLength + (size_t)numDigits + 1;
    buf       = (char *)malloc(msgSize);
    snprintf(buf, msgSize + 1, "%zu%s%s", msgLength, filter, message);

    fdin       = open(FIFO_IN, O_WRONLY | O_CLOEXEC, S_IRWXU);
    close_fdin = true;
    if(fdin == -1)
    {
        fprintf(stderr, "Error opening input FIFO\n");
        goto cleanup;
    }
    result = send(fdin, buf, sizeof(buf), &err);
    if(result == -1)
    {
        perror("write");
        goto cleanup;
    }

    // Now waiting for server to write to FIFO_OUT

    fdout       = open(FIFO_OUT, O_RDONLY | O_CLOEXEC);
    close_fdout = true;
    if(fdout == -1)
    {
        fprintf(stderr, "Error opening output FIFO\n");
        goto cleanup;
    }

    // clear the buffer
    memset(buf, 0, msgSize);

    // read from the fifo
    do
    {
        result = read(fdout, buf, msgSize);
        if(result < 0)
        {
            fprintf(stderr, "Error reading output FIFO\n");
            goto cleanup;
        }
    } while(result != 0);

    printf("Processed string: %s\n", buf);

    if(close_fdout)
    {
        close(fdout);
    }

cleanup:
    free(buf);
    if(close_fdin)
    {
        close(fdin);
    }

done:
    return EXIT_SUCCESS;
}
