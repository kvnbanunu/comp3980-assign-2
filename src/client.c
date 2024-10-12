/*
 * Kevin Nguyen
 * A00955925
 */

#include "../include/usage.h"
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_IN "/tmp/assign2_in"
#define FIFO_OUT "/tmp/assign2_out"

void *send(void *arg);

typedef struct
{
    int   fd;
    char *filter;
    char *message;
} thread_args;

int main(int argc, char *argv[])
{
    int fdin;
    int fdout;
    int opt;

    ssize_t     bytesRead;
    pthread_t   thread;
    thread_args thread_data;

    char       *msgReceived;
    const char *filter  = NULL;
    const char *message = NULL;
    while((opt = getopt(argc, argv, "f:")) != -1)
    {
        if(opt == 'f')
        {
            filter = optarg;
        }
        else
        {
            fprintf(stderr, "Error: Invalid option flag %d\n", opt);
            print_usage(argv[0]);
            goto done;
        }
    }

    if(filter == NULL)
    {
        fprintf(stderr, "Error: Filter cannot be null\n");
        print_usage(argv[0]);
        goto done;
    }

    if(strlen(filter) != 1)
    {
        fprintf(stderr, "Error: Filter option needs to be a single char\n");
        print_usage(argv[0]);
        goto done;
    }

    if(!(strcmp(filter, "U") == 0 || strcmp(filter, "L") == 0 || strcmp(filter, "N") == 0))
    {
        fprintf(stderr, "Error: Invalid filter option %s\n", filter);
        print_usage(argv[0]);
        goto done;
    }

    if(optind >= argc)
    {
        fprintf(stderr, "Error: Unexpected arguments\n");
        print_usage(argv[0]);
        goto done;
    }

    message = argv[optind];
    if(message == NULL)
    {
        fprintf(stderr, "Error: Message cannot be null\n");
        print_usage(argv[0]);
        goto done;
    }

    fdin = open(FIFO_IN, O_WRONLY | O_CLOEXEC);
    if(fdin == -1)
    {
        fprintf(stderr, "Error opening input FIFO\n");
        goto done;
    }

    thread_data.fd     = fdin;
    thread_data.filter = (char *)malloc(2);
    if(thread_data.filter == NULL)
    {
        perror("malloc");
        goto done;
    }
    thread_data.message = (char *)malloc(strlen(message) + 1);
    if(thread_data.message == NULL)
    {
        perror("malloc");
        free(thread_data.filter);
        goto done;
    }
    strncpy(thread_data.filter, filter, 1);
    strncpy(thread_data.message, message, strlen(message));

    if(pthread_create(&thread, NULL, send, (void *)&thread_data) != 0)
    {
        fprintf(stderr, "Error creating thread\n");
        goto cleanup;
    }

    pthread_join(thread, NULL);

    fdout = open(FIFO_OUT, O_RDONLY | O_CLOEXEC);
    if(fdout == -1)
    {
        fprintf(stderr, "Error opening output FIFO\n");
        goto cleanup;
    }

    msgReceived = (char *)malloc(strlen(message) + 1);

    bytesRead = read(fdout, msgReceived, strlen(message) + 1);
    if(bytesRead == -1)
    {
        fprintf(stderr, "Error reading from server\n");
        close(fdout);
        goto cleanup;
    }

    printf("Message Received from Server: %s\n", msgReceived);
    close(fdout);

cleanup:
    free(thread_data.filter);
    free(thread_data.message);
    close(fdin);

done:
    return EXIT_SUCCESS;
}

void *send(void *arg)
{
    int                fdin;
    char              *buf;
    size_t             fullLen;
    const thread_args *data = (thread_args *)arg;

    fullLen = (size_t)snprintf(NULL, 0, "%s:%s", data->filter, data->message) + 1;
    buf     = (char *)malloc(fullLen);
    snprintf(buf, fullLen, "%s:%s", data->filter, data->message);

    fdin = data->fd;
    if(write(fdin, buf, fullLen - 1) == -1)
    {
        fprintf(stderr, "Error writing to input FIFO\n");
        free(buf);
        close(fdin);
        pthread_exit(NULL);
    }
    free(buf);
    close(fdin);
    return NULL;
}
