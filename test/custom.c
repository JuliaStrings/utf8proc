#include "tests.h"

static int thunk_test = 1;

static utf8proc_int32_t custom(utf8proc_int32_t codepoint, void *thunk)
{
    check(((int *) thunk) == &thunk_test, "unexpected thunk passed");
    if (codepoint == 'a')
        return 'b';
    if (codepoint == 'S')
        return 0x00df; /* ß */
    return codepoint;
}

/* a callback whose output depends on how many times it has been called, so
   utf8proc_map_custom's two internal decompose passes see different data: the
   second pass expands to more codepoints than the buffer was sized for. This
   must be rejected rather than overflowing the heap (issue #249). */
static utf8proc_int32_t inconsistent(utf8proc_int32_t codepoint, void *thunk)
{
    int *calls = (int *) thunk;
    utf8proc_int32_t r = (*calls == 0) ? 'a' : 0x0390; /* 0x0390 has a 3-codepoint NFD */
    (void) codepoint;
    (*calls)++;
    return r;
}

int main(void)
{
    utf8proc_uint8_t input[] = {0x41,0x61,0x53,0x62,0xef,0xbd,0x81,0x00}; /* "AaSb\uff41" */
    utf8proc_uint8_t correct[] = {0x61,0x62,0x73,0x73,0x62,0x61,0x00}; /* "abssba" */
    utf8proc_uint8_t *output;
    utf8proc_map_custom(input, 0, &output, UTF8PROC_CASEFOLD | UTF8PROC_COMPOSE | UTF8PROC_COMPAT | UTF8PROC_NULLTERM,
                        custom, &thunk_test);
    check_compare("map_custom", input, correct, output, 1);
    printf("map_custom tests SUCCEEDED.\n");

    {
        utf8proc_uint8_t in[] = {0x78, 0x00}; /* "x" */
        utf8proc_uint8_t *out = NULL;
        int calls = 0;
        utf8proc_ssize_t r = utf8proc_map_custom(in, 0, &out,
            UTF8PROC_NULLTERM | UTF8PROC_DECOMPOSE, inconsistent, &calls);
        check(r == UTF8PROC_ERROR_OVERFLOW,
              "inconsistent custom_func must fail, not overflow the buffer (got %zd)", r);
        check(out == NULL, "no buffer should be returned on error");
        printf("map_custom inconsistent-callback test SUCCEEDED.\n");
    }
    return 0;
}
