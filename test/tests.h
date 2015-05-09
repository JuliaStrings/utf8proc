/* Common functions and includes for our test programs. */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#include "../utf8proc.h"

size_t lineno = 0;

void check(int cond, const char *format, ...)
{
     if (!cond) {
          va_list args;
          fprintf(stderr, "line %zd: ", lineno);
          va_start(args, format);
          vfprintf(stderr, format, args);
          va_end(args);
          fprintf(stderr, "\n");
          exit(1);
     }
}

size_t skipspaces(const char *buf, size_t i)
{
    while (isspace(buf[i])) ++i;
    return i;
}

/* if buf points to a sequence of codepoints encoded as hexadecimal strings,
   separated by whitespace, and terminated by any character not in
   [0-9a-fA-F] or whitespace, then stores the corresponding utf8 string
   in dest, returning the number of bytes read from buf */
utf8proc_ssize_t unsafe_encode_char(utf8proc_int32_t uc, utf8proc_uint8_t *dst);
size_t encode(char *dest, const char *buf)
{
     size_t i = 0, j, d = 0;
     for (;;) {
          int c;
          i = skipspaces(buf, i);
          for (j=i; buf[j] && strchr("0123456789abcdef", tolower(buf[j])); ++j)
               ; /* find end of hex input */
          if (j == i) { /* no codepoint found */
               dest[d] = 0; /* NUL-terminate destination string */
               return i + 1;
          }
          check(sscanf(buf + i, "%x", (unsigned int *)&c) == 1, "invalid hex input %s", buf+i);
          i = j; /* skip to char after hex input */
          d += unsafe_encode_char(c, (utf8proc_uint8_t *) (dest + d));
     }
}

