/* Common functions and includes for our test programs. */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#include "../utf8proc.h"

extern size_t lineno;

void check(int cond, const char *format, ...);
size_t skipspaces(const char *buf, size_t i);
size_t encode(char *dest, const char *buf);
