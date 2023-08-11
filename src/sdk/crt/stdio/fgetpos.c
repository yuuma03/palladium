/* SPDX-FileCopyrightText: (C) 2023 ilmmatias
 * SPDX-License-Identifier: BSD-3-Clause */

#include <crt_impl.h>

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function stores the current position along a FILE stream into an opaque pointer, that
 *     later can be restored using fsetpos.
 *
 * PARAMETERS:
 *     stream - Input file stream.
 *     pos - Output; Where to save the file position.
 *
 * RETURN VALUE:
 *     0 on success, 1 otherwise.
 *-----------------------------------------------------------------------------------------------*/
int fgetpos(struct FILE *stream, struct fpos_t *pos) {
    if (!stream || !pos) {
        return 1;
    }

    pos->file_pos = stream->file_pos;
    return 0;
}
