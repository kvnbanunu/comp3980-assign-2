/*
 * Kevin Nguyen
 * A00955925
 */

#include <ctype.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_IN "/tmp/assign2_in"
#define FIFO_OUT "/tmp/assign2_out"
#define PERMISSIONS 0666
#define BUFFER_SIZE 128

void *process_req(void *arg);
int   filter_message(const char *filter, char *message, size_t msgSize);
void  sig_handler(int sig);

int main(void)
{
    int       fdin;
    pthread_t thread;
    signal(SIGINT, sig_handler);

    mkfifo(FIFO_IN, PERMISSIONS);
    mkfifo(FIFO_OUT, PERMISSIONS);

    printf("Server listening for requests...\n");

    while(1)
    {
        fdin = open(FIFO_IN, O_RDONLY | O_CLOEXEC);
        if(fdin == -1 && SIGINT == 2)
        {
            printf("\nSIGINT Received. Shutting down Server.\n");
            goto done;
        }

        if(pthread_create(&thread, NULL, process_req, (void *)&fdin) != 0)
        {
            fprintf(stderr, "Error creating thread\n");
            goto cleanup;
        }

        pthread_join(thread, NULL);
        close(fdin);
    }

cleanup:
    close(fdin);
    unlink(FIFO_IN);
    unlink(FIFO_OUT);

done:
    return EXIT_SUCCESS;
}

void sig_handler(int sig)
{
    (void)sig;
    unlink(FIFO_IN);
    unlink(FIFO_OUT);
}

int filter_message(const char *filter, char *message, size_t msgSize)
{
    if(filter == NULL)
    {
        fprintf(stderr, "Error NULL filter\n");
        return -1;
    }

    // uppercase
    if(strcmp(filter, "U") == 0)
    {
        for(size_t i = 0; i < msgSize && message[i] != '\0'; i++)
        {
            message[i] = (char)toupper((unsigned char)message[i]);
        }
        return 0;
    }

    // lowercase
    if(strcmp(filter, "L") == 0)
    {
        for(size_t i = 0; i < msgSize && message[i] != '\0'; i++)
        {
            message[i] = (char)tolower((unsigned char)message[i]);
        }
        return 0;
    }

    // do nothing
    if(strcmp(filter, "N") == 0)
    {
        return 0;
    }

    // should not go here
    fprintf(stderr, "Error applying filter\n");
    return -1;
}

void *process_req(void *arg)
{
    int  fdout;
    int  fdin = *((int *)arg);    // Cast and dereference the argument
    char buf[BUFFER_SIZE];

    const char *filter;
    char       *message;
    char       *state;
    ssize_t     bytesRead = read(fdin, buf, BUFFER_SIZE - 1);
    if(bytesRead == -1)
    {
        fprintf(stderr, "Error: couldn't read from FIFO");
        pthread_exit(NULL);
    }

    printf("Request received. Processing...\n");

    // Null terminate the end of the buffer
    buf[bytesRead] = '\0';

    // Open the server fd
    fdout = open(FIFO_OUT, O_WRONLY | O_CLOEXEC);
    if(fdout == -1)
    {
        fprintf(stderr, "Error opening server file descriptor\n");
        pthread_exit(NULL);
    }

    // Split the argument and the message
    filter  = strtok_r(buf, ":", &state);
    message = strtok_r(NULL, ":", &state);

    // Compare the strings and apply the filter
    if(filter_message(filter, message, BUFFER_SIZE) == -1)
    {
        fprintf(stderr, "Error: Filter is invalid");
        close(fdout);
        pthread_exit(NULL);
    }

    printf("Request processed. Sending response.\n");

    // Write back to the other server
    if(write(fdout, message, BUFFER_SIZE - 1) == -1)
    {
        fprintf(stderr, "Error trying to write to server FIFO");
        close(fdout);
        pthread_exit(NULL);
    }

    printf("Response sent\n");

    close(fdout);
    return NULL;
}
