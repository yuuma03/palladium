/* SPDX-FileCopyrightText: (C) 2023 yuuma03
 * SPDX-License-Identifier: BSD-3-Clause */

#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

double atof(const char *str);
int atoi(const char *str);
long atol(const char *str);
long long atoll(const char *str);
long strtol(const char *str, char **str_end, int base);
long long strtoll(const char *str, char **str_end, int base);
unsigned long strtoul(const char *str, char **str_end, int base);
unsigned long long strtoull(const char *str, char **str_end, int base);
float strtof(const char *str, char **str_end);
double strtod(const char *str, char **str_end);
long double strtold(const char *str, char **str_end);

void *malloc(size_t size);
void *calloc(size_t num, size_t size);
void free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* STDLIB_H */
