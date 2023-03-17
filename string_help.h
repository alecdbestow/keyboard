#ifndef _STRING_HELP_H_
#define _STRING_HELP_H_

#include <stddef.h>
#include <stdbool.h>

size_t lstrcpy(char *dest, const char *source);
bool prefix(const char *pre, const char *str);
char *skipPrefix(char *string, char *match);
#endif