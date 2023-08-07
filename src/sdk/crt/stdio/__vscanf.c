/* SPDX-FileCopyrightText: (C) 2023 ilmmatias
 * SPDX-License-Identifier: BSD-3-Clause */

#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MOD_NONE 0
#define MOD_hh 1
#define MOD_h 2
#define MOD_ll 3
#define MOD_l 4
#define MOD_j 5
#define MOD_z 6
#define MOD_t 7
#define MOD_L 8

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     Alternative version of strtoll, reading character-by-character instead of expecting the
 *     whole string to be already in place.
 *
 * PARAMETERS:
 *     base - Which base the number is in, or 0 for auto detection.
 *     value - Output; Where to store the number on success.
 *     context - Platform-specific implementation context/detail.
 *     read_ch - What to do when we need to read something; Do not pass a NULL pointer!
 *     unread_ch - What to do when we failed to match something, and we need to unwind the input.
 *
 * RETURN VALUE:
 *     1 on success, 0 on failure.
 *-----------------------------------------------------------------------------------------------*/
static int _strtoll(
    int base,
    long long *value,
    void *context,
    int (*read_ch)(void *context),
    void (*unread_ch)(void *context, int ch)) {
    /* An almost rule-by-rule match with the strtol implementation; We're optimistic, and
       we're guessing it is a valid integer; It should fail by the end (and unwind) if we
       were wrong. Some sections are commented in the original implementation but not here,
       they are functionally equivalent in both (but using read_ch here instead of *(str++)). */

    int ch = read_ch(context);
    while (isspace(ch)) {
        ch = read_ch(context);
    }

    long sign = ch == '-' ? -1 : 1;
    if (sign < 0 || ch == '+') {
        ch = read_ch(context);
    }

    int prefix = 0;
    if (base == 0 && ch == '0') {
        ch = read_ch(context);

        if (ch == 'x' || ch == 'X') {
            base = 16;
        } else {
            base = 10;
        }

        ch = read_ch(context);
        prefix = 1;
    } else if (base == 0) {
        base = 10;
    }

    if (!prefix && base == 16 && ch == '0') {
        ch = read_ch(context);

        if (ch == 'x' || ch == 'X') {
            ch = read_ch(context);
        } else {
            unread_ch(context, ch);
            ch = '0';
        }
    }

    size_t accum = 0;
    int overflow = 0;

    *value = 0;
    while (1) {
        long long last = *value;

        if (islower(ch)) {
            ch -= 0x57;
        } else if (isupper(ch)) {
            ch -= 0x37;
        } else if (isdigit(ch)) {
            ch -= 0x30;
        } else {
            break;
        }

        if (ch >= base) {
            break;
        }

        /* The other main difference between this and strtoll, is that overflow isn't an error
           here (we just fix the value to LLONG_MAX). */
        if (!overflow) {
            *value = (*value * base) + ch;
            if (*value < last) {
                *value = LLONG_MAX;
                overflow = 1;
            }
        }

        ch = read_ch(context);
        accum++;
    }

    unread_ch(context, ch);

    if (overflow) {
        return 1;
    } else if (accum) {
        *value *= sign;
        return 1;
    } else {
        *value = 0;
        return 0;
    }
}

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     Alternative version of strtoull, reading character-by-character instead of expecting the
 *     whole string to be already in place.
 *
 * PARAMETERS:
 *     base - Which base the number is in, or 0 for auto detection.
 *     value - Output; Where to store the number on success.
 *     context - Platform-specific implementation context/detail.
 *     read_ch - What to do when we need to read something; Do not pass a NULL pointer!
 *     unread_ch - What to do when we failed to match something, and we need to unwind the input.
 *
 * RETURN VALUE:
 *     1 on success, 0 on failure.
 *-----------------------------------------------------------------------------------------------*/
static int _strtoull(
    int base,
    unsigned long long *value,
    void *context,
    int (*read_ch)(void *context),
    void (*unread_ch)(void *context, int ch)) {
    int ch = read_ch(context);
    while (isspace(ch)) {
        ch = read_ch(context);
    }

    /* Base detection is disabled/absent, as scanf doesn't have an auto-detect specifier for
       unsigned integers. */
    if (base == 16 && ch == '0') {
        ch = read_ch(context);

        if (ch == 'x' || ch == 'X') {
            ch = read_ch(context);
        } else {
            unread_ch(context, ch);
            ch = '0';
        }
    }

    size_t accum = 0;
    int overflow = 0;

    *value = 0;
    while (1) {
        unsigned long long last = *value;

        if (islower(ch)) {
            ch -= 0x57;
        } else if (isupper(ch)) {
            ch -= 0x37;
        } else if (isdigit(ch)) {
            ch -= 0x30;
        } else {
            break;
        }

        if (ch >= base) {
            break;
        }

        if (!overflow) {
            *value = (*value * base) + ch;
            if (*value < last) {
                *value = ULLONG_MAX;
                overflow = 1;
            }
        }

        ch = read_ch(context);
        accum++;
    }

    unread_ch(context, ch);

    return accum > 0;
}

/*-------------------------------------------------------------------------------------------------
 * PURPOSE:
 *     This function implements our internal formatted input routine.
 *     Do not call it unless you know what you're doing!
 *
 * PARAMETERS:
 *     format - Base format string.
 *     vlist - Variadic argument list.
 *     context - Implementation-defined context.
 *     read_ch - What to do when we need to read something; Do not pass a NULL pointer!
 *     unread_ch - What to do when we failed to match something, and we need to unwind the input.
 *
 * RETURN VALUE:
 *     How many of the variadic arguments we filled.
 *-----------------------------------------------------------------------------------------------*/
int __vscanf(
    const char *format,
    va_list vlist,
    void *context,
    int (*read_ch)(void *context),
    void (*unread_ch)(void *context, int ch)) {
    int filled = 0;

    while (*format) {
        const char *start = format;
        int ch = *(format++);

        if (isspace(ch)) {
            ch = *format;
            while (isspace(ch)) {
                ch = *(++format);
            }

            while (1) {
                ch = read_ch(context);
                if (!isspace(ch)) {
                    unread_ch(context, ch);
                    break;
                }
            }

            continue;
        }

        if (ch != '%') {
            int read = read_ch(context);
            if (read != ch) {
                unread_ch(context, read);
                return filled ? filled : EOF;
            }

            continue;
        }

        /* Output supressor, doesn't write to any varidic arguemnts when set. */
        int supress = *format == '*';
        if (supress) {
            format++;
        }

        /* Width specifier, use this together with '%s', unless you want a buffer overflow. */
        int width = 0;
        int width_set = 0;
        if (isdigit(*format)) {
            width = strtol(format, (char **)&format, 10);
            width_set = 1;
        }

        /* Final group, length modifiers. */
        int mod = 0;
        switch (*format) {
            case 'h':
                if (*(format + 1) == 'h') {
                    format += 2;
                    mod = MOD_hh;
                } else {
                    format++;
                    mod = MOD_h;
                }
                break;
            case 'l':
                if (*(format + 1) == 'l') {
                    format += 2;
                    mod = MOD_ll;
                } else {
                    format++;
                    mod = MOD_l;
                }
                break;
            case 'j':
                format++;
                mod = MOD_j;
                break;
            case 'z':
                format++;
                mod = MOD_z;
                break;
            case 't':
                format++;
                mod = MOD_t;
                break;
            case 'L':
                format++;
                mod = MOD_L;
                break;
        }

        unsigned long long unsigned_value;
        long long signed_value;
        char *str_start;
        char *str;

        ch = *(format++);

        /* At last, handle the specifiers. */
        switch (ch) {
            case '%':
                ch = read_ch(context);
                if (ch != '%') {
                    unread_ch(context, ch);
                    return filled ? filled : EOF;
                }

                break;
            /* Reads exactly 1 or `width` characters; Don't use it if you expect NULL
               termination! */
            case 'c':
                if (!supress) {
                    str_start = va_arg(vlist, char *);
                    str = str_start;
                }

                width = width_set ? width : 1;
                while (width-- > 0) {
                    int ch = read_ch(context);
                    if (ch == EOF && str - str_start) {
                        return supress ? filled : filled + 1;
                    } else if (ch == EOF) {
                        return filled ? filled : EOF;
                    }

                    if (!supress) {
                        *(str++) = ch;
                    }
                }

                if (!supress) {
                    filled++;
                }

                break;
            /* Reads until whitespace/end, or until width == 0; Buffer size should be width+1,
               as we always write the NULL terminator. */
            case 's':
                if (!supress) {
                    str_start = va_arg(vlist, char *);
                    str = str_start;
                }

                while (1) {
                    if (width_set && width-- <= 0) {
                        break;
                    }

                    ch = read_ch(context);
                    if (ch == EOF && str - str_start) {
                        return supress ? filled : filled + 1;
                    } else if (ch == EOF) {
                        return filled ? filled : EOF;
                    } else if (!ch || isspace(ch)) {
                        break;
                    }

                    if (!supress) {
                        *(str++) = ch;
                    }
                }

                if (!supress) {
                    *str = 0;
                    filled++;
                }

                break;
            /* Is there any reason why signed integer specifiers have an auto-base, but not
               unsigned? */
            case 'd':
            case 'i':
                if (!_strtoll(ch == 'd' ? 10 : 0, &signed_value, context, read_ch, unread_ch)) {
                    return filled ? filled : EOF;
                }

                if (!supress) {
                    switch (mod) {
                        case MOD_hh:
                            *va_arg(vlist, char *) = signed_value;
                            break;
                        case MOD_h:
                            *va_arg(vlist, short *) = signed_value;
                            break;
                        case MOD_l:
                            *va_arg(vlist, long *) = signed_value;
                            break;
                        case MOD_ll:
                            *va_arg(vlist, long long *) = signed_value;
                            break;
                        case MOD_j:
                            *va_arg(vlist, intmax_t *) = signed_value;
                            break;
                        case MOD_z:
                            *va_arg(vlist, size_t *) = signed_value;
                            break;
                        case MOD_t:
                            *va_arg(vlist, ptrdiff_t *) = signed_value;
                            break;
                        default:
                            *va_arg(vlist, int *) = signed_value;
                            break;
                    }

                    filled++;
                }

                break;
            case 'u':
            case 'o':
            case 'x':
            case 'X':
                if (!_strtoull(
                        ch == 'u'   ? 10
                        : ch == 'o' ? 8
                                    : 16,
                        &unsigned_value,
                        context,
                        read_ch,
                        unread_ch)) {
                    return filled ? filled : EOF;
                }

                if (!supress) {
                    switch (mod) {
                        case MOD_hh:
                            *va_arg(vlist, unsigned char *) = unsigned_value;
                            break;
                        case MOD_h:
                            *va_arg(vlist, unsigned short *) = unsigned_value;
                            break;
                        case MOD_l:
                            *va_arg(vlist, unsigned long *) = unsigned_value;
                            break;
                        case MOD_ll:
                            *va_arg(vlist, unsigned long long *) = unsigned_value;
                            break;
                        case MOD_j:
                            *va_arg(vlist, intmax_t *) = unsigned_value;
                            break;
                        case MOD_z:
                            *va_arg(vlist, size_t *) = unsigned_value;
                            break;
                        case MOD_t:
                            *va_arg(vlist, ptrdiff_t *) = unsigned_value;
                            break;
                        default:
                            *va_arg(vlist, unsigned int *) = unsigned_value;
                            break;
                    }

                    filled++;
                }

                break;
            case 'p':
                if (!_strtoull(16, &unsigned_value, context, read_ch, unread_ch)) {
                    return filled ? filled : EOF;
                }

                if (!supress) {
                    *va_arg(vlist, void **) = (void *)unsigned_value;
                    filled++;
                }

                break;
            default:
                for (ptrdiff_t i = 0; i < format - start; i++) {
                    ch = read_ch(context);
                    if (ch != start[i]) {
                        unread_ch(context, ch);
                        return filled ? filled : EOF;
                    }

                    break;
                }

                break;
        }
    }

    return filled;
}
