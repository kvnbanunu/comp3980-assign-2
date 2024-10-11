#include "../include/readfully.h"
#include <ctype.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define FIFO_IN "/tmp/assign2_in"
#define FIFO_OUT "/tmp/assign2_out"
#define BUF_SIZE 128

typedef struct
{
    bool run;
} server_state_t;

void *process_req(void *arg);
void  handle_signal(int sig, siginfo_t *siginfo, void *context);

int main(void)
{
    int              fdin;
    int              fdout;
    char            *buf;
    int              err;
    pthread_t        thread;
    server_state_t   state = {0};
    struct sigaction sa;

    // open fifos
    fdin = open(FIFO_IN, O_RDONLY | O_CLOEXEC);
    if(fdin == -1)
    {
        perror("open input fifo");
        goto done;
    }

    fdout = open(FIFO_OUT, O_WRONLY | O_CLOEXEC, S_IRWXU);
    if(fdout == -1)
    {
        perror("open output fifo");
        close(fdin);
        goto done;
    }
    state.run = true;

    // register signal handler for SIGINT (CTRL-C)
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#endif
    sa.sa_sigaction = handle_signal;
#if defined(__clang__)
    #pragma clang diagnostic pop
#endif
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    if(sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("sigaction");
        goto cleanup;
    }

    printf("Server listening...\n");

    while(state.run)
    {
        ssize_t bytes_read;
        buf = (char *)malloc(BUF_SIZE);
        if(buf == NULL)
        {
            perror("malloc");
            goto cleanup;
        }

        // clear buffer for each iteration
        memset(buf, 0, BUF_SIZE);

        // ensure server reads the full message
        bytes_read = read_fully(fdin, buf, BUF_SIZE, &err);
        if(bytes_read > 0)
        {
            // create a new thread to handle each new client req
            char *request = strdup(buf);
            pthread_create(&thread, NULL, process_req, request);
            pthread_detach(thread);
        }
        // free for the next iteration.
        free(buf);
    }

cleanup:
    close(fdin);
    close(fdout);

done:
    return EXIT_SUCCESS;
}

// parses filter and message from buffer
// applies filter to message
// attempts to fully write the message to FIFO_OUT
void *process_req(void *arg)
{
    char *request = (char *)arg;
    int   fdout;

    // filter:message
    const char *filter  = strtok_r(request, ":", &request);
    char       *message = strtok_r(NULL, ":", &request);

    if(strcmp(filter, "U") == 0)
    {
        for(int i = 0; message[i]; i++)
        {
            message[i] = (char)toupper(message[i]);
        }
    }
    else if(strcmp(filter, "L") == 0)
    {
        for(int i = 0; message[i]; i++)
        {
            message[i] = (char)tolower(message[i]);
        }
    }

    fdout = open(FIFO_OUT, O_WRONLY | O_CLOEXEC, S_IRWXU);
    if(fdout != -1)
    {
        size_t bytes_written = 0;
        size_t count         = strlen(message);
        while(bytes_written < count)
        {
            ssize_t result = write(fdout, (const char *)message + bytes_written, count - bytes_written);
            if(result == -1)
            {
                perror("partial write");
                goto done;
            }
            bytes_written += (size_t)result;
        }
    }

done:
    free(request);
    return NULL;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

// Signal handler for SIGINT
// using server state context
void handle_signal(int sig, siginfo_t *siginfo, void *context)
{
    if(sig == SIGINT)
    {
        server_state_t *state = (server_state_t *)context;
        state->run            = false;
        printf("SIGINT Received. Shutting down server...\n");
    }
}

#pragma GCC diagnostic pop
