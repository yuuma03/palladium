/* SPDX-FileCopyrightText: (C) 2023 ilmmatias
 * SPDX-License-Identifier: BSD-3-Clause */

#include <stdio.h>

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function outputs a string followed by a new-line character to the screen.
 *
 * PARAMETERS:
 *     str - The string to be written.
 *
 * RETURN VALUE:
 *     How many characters we wrote on success, EOF on failure.
 *-----------------------------------------------------------------------------------------------*/
int puts(const char *str) {
    int accum = fputs(str, stdout);

    if (accum == EOF || fputc('\n', stdout) == EOF) {
        return EOF;
    }

    return accum + 1;
}
