#include <stdlib.h>
#include <string.h>

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function implements generic code to create a copy of a C string, limiting the newly
 *     allocated string to at most `size` bytes + 1 (space for the null terminator).
 *
 * PARAMETERS:
 *     src - The string to be copied.
 *     size - How many bytes are we allowed to copy at most.
 *
 * RETURN VALUE:
 *     Copy of the source string, or NULL if we couldn't allocate the memory.
 *-----------------------------------------------------------------------------------------------*/
char *strndup(const char *src, size_t size) {
    /* The source string is allowed to not end with a null terminator, so we can't use
       strlen(). */

    const char *Source = src;
    size_t CopySize = 0;

    while (*(Source++) && size--) {
        CopySize++;
    }

    char *Copy = malloc(CopySize + 1);
    if (Copy) {
        memcpy(Copy, src, CopySize);
    }

    Copy[CopySize] = 0;

    return Copy;
}
