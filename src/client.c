/*
* Kevin Nguyen
* A00955925
*/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void print_usage();

int main(int argc, char *argv[])
{
    int opt;
    int messageSize;
    const char* filter;
    const char* message = NULL;

    while ((opt = getopt(argc, argv, "f:"))!= -1)
    {
        switch(opt)
        {
            case 'f':
                if (sizeof(optarg) != 1)
                {
                    fprintf(stderr, "Error: Filter setting needs to be a single char\n");
                    print_usage();
                }
                filter = optarg;
                break;
            default:
                print_usage();
        }
    }
    return EXIT_SUCCESS;
}

void print_usage()
{
    fprintf(stderr, "Usage: %s [-f filter (u|l|n)] [message]\n". argv[0]);
    exit(EXIT_FAILURE);
}
