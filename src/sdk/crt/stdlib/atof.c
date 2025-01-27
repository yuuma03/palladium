/* SPDX-FileCopyrightText: (C) 2023-2025 ilmmatias
 * SPDX-License-Identifier: GPL-3.0-or-later */

#include <crt_impl.h>
#include <ctype.h>
#include <math.h>

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function parses a double (floating-point value) from a string.
 *
 * PARAMETERS:
 *     str - String to be parsed.
 *
 * RETURN VALUE:
 *     Result of the parsing if the string was a valid double, 0.0 otherwise.
 *-----------------------------------------------------------------------------------------------*/
double atof(const char *str) {
    double sign = *str == '-' ? -1. : 1.;

    if (sign < 0 || *str == '+') {
        str++;
    }

    if (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')) {
        return __strtod_hex(str + 2, sign);
    }

    if (isdigit(*str)) {
        return __strtod_dec(str, sign);
    }

    if ((*str == 'i' || *str == 'I') && (*(str + 1) == 'n' || *(str + 1) == 'N') &&
        (*(str + 2) == 'f' || *(str + 2) == 'F')) {
        return sign < 0 ? -INFINITY : INFINITY;
    }

    /* TODO: We should also support quiet NaNs in the future. */
    if ((*str == 'n' || *str == 'N') && (*(str + 1) == 'a' || *(str + 1) == 'A') &&
        (*(str + 2) == 'n' || *(str + 2) == 'N')) {
        return sign < 0 ? -NAN : NAN;
    }

    return 0.;
}
