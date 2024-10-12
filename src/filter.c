/*
 * Kevin Nguyen
 * A00955925
 */

#include "../include/filter.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

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
