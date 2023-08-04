/* SPDX-FileCopyrightText: (C) 2023 ilmmatias
 * SPDX-License-Identifier: BSD-3-Clause */

#include <crt_impl.h>
#include <stddef.h>

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function outputs a single character to the screen.
 *
 * PARAMETERS:
 *     ch - Which character to write.
 *
 * RETURN VALUE:
 *     Whatever stored in `ch` on success, EOF otherwise.
 *-----------------------------------------------------------------------------------------------*/
int putchar(int ch) {
    __put_stdout(&ch, 1, NULL);
    return ch;
}