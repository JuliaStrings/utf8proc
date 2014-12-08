#include "tests.h"

int main(void)
{
    char *buf = NULL;
    size_t bufsize = 0;
    FILE *f = fopen("GraphemeBreakTest.txt", "r");
    uint8_t src[1024];
    
    check(f != NULL, "error opening NormalizationTest.txt");
    while (getline(&buf, &bufsize, f) > 0) {
        size_t bi = 0, si = 0;
        lineno += 1;
        
        if (lineno % 100 == 0)
            printf("checking line %zd...\n", lineno);
        
        if (buf[0] == '#') continue;
        
        while (buf[bi]) {
            bi = skipspaces(buf, bi);
            if (buf[bi] == '/') { /* grapheme break */
                src[si++] = 0xff;
                bi++;
            }
            else if (buf[bi] == '+') { /* no break */
                bi++;
            }
            else if (buf[bi] == '#') { /* start of comments */
                break;
            }
            else { /* hex-encoded codepoint */
                bi += encode((char*) (src + si), buf + bi) - 1;
                while (src[si]) ++si; /* advance to NUL termination */
            }
        }
        if (si && src[si-1] == 0xff)
            --si; /* no 0xff after final grapheme */
        src[si] = 0; /* NUL-terminate */
        
        if (si) {
            uint8_t utf8[1024]; /* copy src without 0xff grapheme separators */
            size_t i = 0, j = 0;
            ssize_t glen;
            uint8_t *g; /* utf8proc_map grapheme results */
            while (i < si) {
                if (src[i] != 0xff)
                    utf8[j++] = src[i++];
                else
                    i++;
            }
            glen = utf8proc_map(utf8, j, &g, UTF8PROC_CHARBOUND);
            check(glen >= 0, "utf8proc_map error = %s",
                  utf8proc_errmsg(glen));
            check(!strcmp((char*)g, (char*)src),
                  "grapheme mismatch: %s vs. %s", (char*)g, (char*)src);
        }
    }
    fclose(f);
    printf("Passed tests after %zd lines!\n", lineno);
    return 0;
}
