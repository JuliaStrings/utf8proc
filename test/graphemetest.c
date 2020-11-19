#include "tests.h"

int main(int argc, char **argv)
{
    unsigned char buf[8192];
    FILE *f = argc > 1 ? fopen(argv[1], "r") : NULL;
    utf8proc_uint8_t src[1024];

    check(f != NULL, "error opening GraphemeBreakTest.txt");
    while (simple_getline(buf, f) > 0) {
        size_t bi = 0, si = 0;
        lineno += 1;

        if (lineno % 100 == 0)
            printf("checking line %zd...\n", lineno);

        if (buf[0] == '#') continue;

        while (buf[bi]) {
            bi = skipspaces(buf, bi);
            if (buf[bi] == 0xc3 && buf[bi+1] == 0xb7) { /* U+00f7 = grapheme break */
                src[si++] = '/';
                bi += 2;
            }
            else if (buf[bi] == 0xc3 && buf[bi+1] == 0x97) { /* U+00d7 = no break */
                bi += 2;
            }
            else if (buf[bi] == '#') { /* start of comments */
                break;
            }
	    else { /* hex-encoded codepoint */
                size_t len = encode((unsigned char*) (src + si), buf + bi) - 1;
                while (src[si]) ++si; /* advance to NUL termination */
                bi += len;
            }
        }
        if (si && src[si-1] == '/')
            --si; /* no break after final grapheme */
        src[si] = 0; /* NUL-terminate */

        if (si) {
            utf8proc_uint8_t utf8[1024]; /* copy src without 0xff grapheme separators */
            size_t i = 0, j = 0;
            utf8proc_ssize_t glen, k;
            utf8proc_uint8_t *g; /* utf8proc_map grapheme results */
            while (i < si) {
                if (src[i] != '/')
                    utf8[j++] = src[i++];
                else
                    i++;
            }
            glen = utf8proc_map(utf8, j, &g, UTF8PROC_CHARBOUND);
            if (glen == UTF8PROC_ERROR_INVALIDUTF8) {
                 /* the test file contains surrogate codepoints, which are only for UTF-16 */
                 printf("line %zd: ignoring invalid UTF-8 codepoints\n", lineno);
            }
            else {
                 check(glen >= 0, "utf8proc_map error = %s",
                       utf8proc_errmsg(glen));
                 for (k = 0; k <= glen; ++k)
                      if (g[k] == 0xff)
                          g[k] = '/'; /* easier-to-read output (/ is not in test strings) */
                 check(!strcmp((char*)g, (char*)src),
                       "grapheme mismatch: \"%s\" instead of \"%s\"", (char*)g, (char*)src);
            }
            free(g);
        }
    }
    fclose(f);
    printf("Passed tests after %zd lines!\n", lineno);

    /* issue 144 */
    {
        utf8proc_uint8_t input[] = {0xef,0xbf,0xbf,0xef,0xbf,0xbe,0x00}; /* "\uffff\ufffe" */
        utf8proc_uint8_t output[] = {0xff,0xef,0xbf,0xbf,0xff,0xef,0xbf,0xbe,0x00}; /* with 0xff grapheme markers */
        utf8proc_ssize_t glen;
        utf8proc_uint8_t *g;
        glen = utf8proc_map(input, 6, &g, UTF8PROC_CHARBOUND);
        check(!strcmp((char*)g, (char*)output), "mishandled u+ffff and u+fffe grapheme breaks");
        free(g);
    };

    /*https://github.com/JuliaLang/julia/issues/37680*/
    {
        // Two swedish flags after each other
        utf8proc_int32_t double_sweden[] = {
            0x0001f1f8, 0x0001f1ea, 0x0001f1f8, 0x0001f1ea
        };
        // facepalm + pale skin + zwj + male sign + FE0F
        utf8proc_int32_t facepalm[] ={
           0x0001f926, 0x0001f3fc, 0x0000200d, 0x00002642, 0x0000fe0f
        };
        // man face + pale skin + zwj + hand holding + zwj + man face + dark skin
        utf8proc_int32_t family[] = {
            0x0001f468, 0x0001f3fb, 0x0000200d, 0x00001f91d, 0x0000200d, 0x0001f468, 0x0001f3fd
        };
        bool expected_double_sweden[] = {false, true, false};
        bool expected_facepalm[] = {false, false, false, false};
        bool expected_family[] = {false, false, false, false, false, false};
        bool results_double_sweden[4];
        bool results_facepalm[5];
        bool results_family[6];

        utf8proc_int32_t state = 0;
        for (int i = 0; i < 3; i++) {
            utf8proc_int32_t c1 = double_sweden[i];
            utf8proc_int32_t c2 = double_sweden[i+1];
            results_double_sweden[i] = utf8proc_grapheme_break_stateful(c1, c2, &state);
            check(results_double_sweden[i] == expected_double_sweden[i], "Incorrect grapheme break on initial repeated flags");
        }

        state = 0;
        for (int i = 0; i < 4; i++) {
            utf8proc_int32_t c1 = facepalm[i];
            utf8proc_int32_t c2 = facepalm[i+1];
            results_facepalm[i] = utf8proc_grapheme_break_stateful(c1, c2, &state);
            check(results_facepalm[i] == expected_facepalm[i], "Incorrect grapheme break on initial extended + zwj emoji");
        }

        state = 0;
        for (int i = 0; i < 5; i++) {
            utf8proc_int32_t c1 = family[i];
            utf8proc_int32_t c2 = family[i+1];
            results_family[i] = utf8proc_grapheme_break_stateful(c1, c2, &state);
            check(results_family[i] == expected_family[i], "Incorrect grapheme break on initial extended + zwj emoji");
        }
    }

    return 0;
}
