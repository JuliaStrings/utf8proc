#include "tests.h"
#include <ctype.h>
#include <wchar.h>

int main(int argc, char **argv)
{
     int c, error = 0;

     (void) argc; /* unused */
     (void) argv; /* unused */

     /* some simple sanity tests of  */
     for (c = 0; c < 0x80; c++) {
        if (utf8proc_sizeof_char(c) != 1) {
           fprintf(stderr, "Failed: sizeof_char(%04x) != 1\n", c);
           error++;
        }
     }
     for (;c < 0x800; c++) {
        if (utf8proc_sizeof_char(c) != 2) {
           fprintf(stderr, "Failed: sizeof_char(%04x) != 2\n", c);
           error++;
        }
     }
     for (;c < 0x10000; c++) {
        if (utf8proc_sizeof_char(c) != 3) {
           fprintf(stderr, "Failed: sizeof_char(%06x) != 3\n", c);
           error++;
        }
     }
     for (;c < 0x110000; c++) {
        if (utf8proc_sizeof_char(c) != 4) {
           fprintf(stderr, "Failed: sizeof_char(%06x) != 4\n", c);
           error++;
        }
     }
     check(!error, "utf8proc_sizeof_char FAILED %d tests.", error);
     printf("Validity tests SUCCEEDED.\n");

     return 0;
}
