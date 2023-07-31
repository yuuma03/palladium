#include <stddef.h>

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function implements generic code to compare the data in two buffers.
 *
 * PARAMETERS:
 *     lhs - Left-hand side of the expression.
 *     rhs - Right-hand side of the expression.
 *     count - Size of the smallest buffer.
 *
 * RETURN VALUE:
 *     0 for equality, *lhs - *rhs if an inequality is found (where `lhs` and `rhs` are both
 *     reinterpreted as char*, and are pointing to where the inequality happened).
 *-----------------------------------------------------------------------------------------------*/
int memcmp(const void *lhs, const void *rhs, size_t count) {
    const char *Left = lhs;
    const char *Right = rhs;

    while (*Left == *Right && count--) {
        Left++;
        Right++;
    }

    return *Left - *Right;
}
