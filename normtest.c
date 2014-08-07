#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#include "mojibake.h"

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

/* if buf points to a sequence of codepoints encoded as hexadecimal strings,
   separated by whitespace, and terminated by any character not in
   [0-9a-fA-F] or whitespace, then stores the corresponding utf8 string
   in dest, returning the number of bytes read from buf */
size_t encode(char *dest, const char *buf)
{
     size_t i = 0, j, d = 0;
     do {
          int c;
          while (isspace(buf[i])) ++i; /* skip whitespace */
          for (j=i; buf[j] && strchr("0123456789abcdef", tolower(buf[j])); ++j)
               ; /* find end of hex input */
          if (j == i) { /* no codepoint found */
               dest[d] = 0; /* NUL-terminate destination string */
               return i + 1;
          }
          check(sscanf(buf + i, "%x", &c) == 1, "invalid hex input %s", buf+i);
          i = j; /* skip to char after hex input */
          d += utf8proc_encode_char(c, (uint8_t *) (dest + d));
     } while (1);
}

#define CHECK_NORM(NRM, norm, src) {                                 \
    char *src_norm = (char*) utf8proc_ ## NRM((uint8_t*) src);      \
    check(!strcmp(norm, src_norm),                                  \
          "normalization failed for %s -> %s", src, norm);          \
    free(src_norm);                                                 \
}

int main(void)
{
     char *buf = NULL;
     size_t bufsize = 0;
     FILE *f = fopen("NormalizationTest.txt", "r");
     char source[1024], NFC[1024], NFD[1024], NFKC[1024], NFKD[1024];

     check(f != NULL, "error opening NormalizationTest.txt");
     while (getline(&buf, &bufsize, f) > 0) {
          size_t offset;
          lineno += 1;

          if (buf[0] == '@') {
               printf("line %zd: %s", lineno, buf + 1);
               continue;
          }
          else if (lineno % 1000 == 0)
               printf("checking line %zd...\n", lineno);

          if (buf[0] == '#') continue;

          offset = encode(source, buf);
          offset += encode(NFC, buf + offset);
          offset += encode(NFD, buf + offset);
          offset += encode(NFKC, buf + offset);
          offset += encode(NFKD, buf + offset);

          CHECK_NORM(NFC, NFC, source);
          CHECK_NORM(NFC, NFC, NFC);
          CHECK_NORM(NFC, NFC, NFD);
          CHECK_NORM(NFC, NFKC, NFKC);
          CHECK_NORM(NFC, NFKC, NFKD);

          CHECK_NORM(NFD, NFD, source);
          CHECK_NORM(NFD, NFD, NFC);
          CHECK_NORM(NFD, NFD, NFD);
          CHECK_NORM(NFD, NFKD, NFKC);
          CHECK_NORM(NFD, NFKD, NFKD);

          CHECK_NORM(NFKC, NFKC, source);
          CHECK_NORM(NFKC, NFKC, NFC);
          CHECK_NORM(NFKC, NFKC, NFD);
          CHECK_NORM(NFKC, NFKC, NFKC);
          CHECK_NORM(NFKC, NFKC, NFKD);

          CHECK_NORM(NFKD, NFKD, source);
          CHECK_NORM(NFKD, NFKD, NFC);
          CHECK_NORM(NFKD, NFKD, NFD);
          CHECK_NORM(NFKD, NFKD, NFKC);
          CHECK_NORM(NFKD, NFKD, NFKD);
     }
     fclose(f);
     printf("Passed tests after %zd lines!\n", lineno);
     return 0;
}
