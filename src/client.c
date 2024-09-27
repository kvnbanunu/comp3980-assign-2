/*
 * Kevin Nguyen
 * A00955925
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

_Noreturn void print_usage(const char *prog_name);

int main(int argc, char *argv[])
{
    int         opt;
    int         argind      = 1;
    size_t      messageSize = 0;
    const char *filter      = NULL;
    char       *message;
    // Reading the option flags
    while((opt = getopt(argc, argv, "f:")) != -1)
    {
        if(opt == 'f')
        {
            if(sizeof(optarg) != 1)
            {
                fprintf(stderr, "Error: Filter option needs to be a single char\n");
                print_usage(argv[0]);
            }
            filter = optarg;
            if(!(strcmp(filter, "U") == 0 || strcmp(filter, "L") == 0 || strcmp(filter, "N") == 0))
            {
                fprintf(stderr, "Error: Invalid filter option %s\n", optarg);
                print_usage(argv[0]);
            }
        }
        else
        {
            fprintf(stderr, "Invalid flag %d\n", opt);
            print_usage(argv[0]);
        }
    }

    // Checking if there are arguments after the filter option
    if(optind == argc - 1)
    {
        fprintf(stderr, "Error: Message cannot be empty");
        print_usage(argv[0]);
    }

    if(filter)
    {
        argind++;
    }

    if(argind == argc - 1)
    {
        fprintf(stderr, "Error: Message cannot be empty");
        print_usage(argv[0]);
    }

    for(int i = argind; i < argc; i++)
    {
        messageSize += strlen(argv[i]) + 1;
    }

    if(messageSize > 0)
    {
        message = (char *)malloc(messageSize * sizeof(char));
        strncpy(message, argv[argind], messageSize);
    }
    else
    {
        fprintf(stderr, "Error: Message cannot be empty");
        print_usage(argv[0]);
    }

    // Add to message
    for(int i = argind + 1; i < argc; i++)
    {
        strcat(message, argv[i]);
        printf("%s\n", message);
    }

    // Testing the filter
    if(filter == NULL)
    {
        printf("filter is null\n");
    }
    else
    {
        printf("%s", filter);
    }
    free(message);
    return EXIT_SUCCESS;
}

_Noreturn void print_usage(const char *prog_name)
{
    fprintf(stderr, "Usage: %s [-f filter] [message]\n\t-f\tU | uppercase\n\t\tL | lowercase\n\t\tN | null\n", prog_name);
    exit(EXIT_FAILURE);
}
