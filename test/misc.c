/* Miscellaneous tests, e.g. regression tests */

#include "tests.h"

static void issue128(void) /* #128 */
{
    utf8proc_uint8_t input[] = {0x72, 0xcc, 0x87, 0xcc, 0xa3, 0x00}; /* "r\u0307\u0323" */
    utf8proc_uint8_t nfc[] = {0xe1, 0xb9, 0x9b, 0xcc, 0x87, 0x00}; /* "\u1E5B\u0307" */
    utf8proc_uint8_t nfd[] = {0x72, 0xcc, 0xa3, 0xcc, 0x87, 0x00}; /* "r\u0323\u0307" */

    check_compare("NFC", input, nfc, utf8proc_NFC(input), 1);
    check_compare("NFD", input, nfd, utf8proc_NFD(input), 1);
}

static void issue102(void) /* #102 */
{
    utf8proc_uint8_t input[] = {0x58, 0xe2, 0x81, 0xa5, 0x45, 0xcc, 0x80, 0xc2, 0xad, 0xe1, 0xb4, 0xac, 0x00}; /* "X\u2065E\u0300\u00ad\u1d2c" */
    utf8proc_uint8_t stripna[] = {0x78, 0xc3, 0xa8, 0x61, 0x00}; /* "x\u00e8a" */
    utf8proc_uint8_t correct[] = {0x78, 0xe2, 0x81, 0xa5, 0xc3, 0xa8, 0x61, 0x00}; /* "x\u2065\u00e8a" */
    utf8proc_uint8_t *output;

    utf8proc_map(input, 0, &output, UTF8PROC_NULLTERM | UTF8PROC_STABLE |
        UTF8PROC_COMPOSE | UTF8PROC_COMPAT | UTF8PROC_CASEFOLD | UTF8PROC_IGNORE | UTF8PROC_STRIPNA);
    check_compare("NFKC_Casefold+stripna", input, stripna, output, 1);
    check_compare("NFKC_Casefold", input, correct, utf8proc_NFKC_Casefold(input), 1);
}

static void issue317(void) /* #317 */
{
    utf8proc_uint8_t input[] = {0xec, 0xa3, 0xa0, 0xe1, 0x86, 0xa7, 0x00}; /* "\uc8e0\u11a7" */
    utf8proc_uint8_t combined[] = {0xec, 0xa3, 0xa, 0x00}; /* "\uc8e1" */
    utf8proc_int32_t codepoint;

    /* inputs that should *not* be combined* */
    check_compare("NFC", input, input, utf8proc_NFC(input), 1);
    utf8proc_encode_char(0x11c3, input+3);
    check_compare("NFC", input, input, utf8proc_NFC(input), 1);

    /* inputs that *should* be combined (TCOUNT-1 chars starting at TBASE+1) */
    for (codepoint = 0x11a8; codepoint < 0x11c3; ++codepoint) {
        utf8proc_encode_char(codepoint, input+3);
        utf8proc_encode_char(0xc8e0 + (codepoint - 0x11a7), combined);
        check_compare("NFC", input, combined, utf8proc_NFC(input), 1);
    }
}

/* check that utf8proc_normalize_utf32 passes uninterpretable codepoints through
   rather than deleting them, and never composes across one */
static void check_normalize_utf32(const char *name,
                                  const utf8proc_int32_t *input, utf8proc_ssize_t len,
                                  const utf8proc_int32_t *expected, utf8proc_ssize_t expected_len)
{
    utf8proc_int32_t buffer[8];
    utf8proc_ssize_t i, result;

    check(len <= (utf8proc_ssize_t) (sizeof(buffer)/sizeof(*buffer)), "%s: test buffer too small", name);
    for (i = 0; i < len; ++i) buffer[i] = input[i];
    result = utf8proc_normalize_utf32(buffer, len, UTF8PROC_COMPOSE);
    check(result == expected_len, "%s: returned length %zd, expected %zd", name, result, expected_len);
    for (i = 0; i < expected_len; ++i)
        check(buffer[i] == expected[i], "%s: buffer[%zd] = 0x%x, expected 0x%x",
              name, i, buffer[i], expected[i]);
}

static void issue288(void) /* #288 */
{
    /* baseline: composition still happens */
    {
        utf8proc_int32_t input[] = {0x0041, 0x0301};        /* "A" + combining acute */
        utf8proc_int32_t expected[] = {0x00c1};             /* "\u00c1" */
        check_normalize_utf32("compose", input, 2, expected, 1);
    }

    /* an out-of-range codepoint is preserved, and blocks composition across it
       (unsafe_get_property would read past utf8proc_stage1table) */
    {
        utf8proc_int32_t input[] = {0x0041, 0x110000, 0x0301};
        check_normalize_utf32("out-of-range codepoint", input, 3, input, 3);
    }

    /* likewise the grapheme-break sentinel written by utf8proc_decompose_char */
    {
        utf8proc_int32_t input[] = {0x0041, -1, 0x0301};
        check_normalize_utf32("grapheme-break sentinel", input, 3, input, 3);
    }

    printf("PASSED: utf8proc_normalize_utf32 preserves uninterpretable codepoints\n");
}

/* canonical ordering of a run of n combining marks: the output must be the
   stable sort by combining class for any run length (long runs take a
   different code path than short ones), and must not take quadratic time */
static void check_canonical_order(utf8proc_ssize_t n)
{
    /* three classes, three marks each, so stability within a class is checked */
    static const utf8proc_int32_t marks[3][3] = {
        {0x0300, 0x0301, 0x0302}, /* combining class 230 */
        {0x0323, 0x0324, 0x0325}, /* combining class 220 */
        {0x0334, 0x0335, 0x0336}  /* combining class 1 */
    };
    utf8proc_uint8_t *input = (utf8proc_uint8_t *) malloc((size_t) (2*n + 2));
    utf8proc_uint8_t *expected = (utf8proc_uint8_t *) malloc((size_t) (2*n + 2));
    utf8proc_uint8_t *output;
    utf8proc_ssize_t i, cls, len, explen, result;

    check(input && expected, "canonical order: out of memory");
    /* input: 'a' followed by the three classes in descending order */
    len = utf8proc_encode_char(0x61, input);
    for (i = 0; i < n; ++i)
        len += utf8proc_encode_char(marks[i*3/n][i%3], input + len);
    input[len] = 0;
    /* expected: 'a' followed by the same marks in ascending class order,
       the order within each class unchanged */
    explen = utf8proc_encode_char(0x61, expected);
    for (cls = 2; cls >= 0; --cls)
        for (i = 0; i < n; ++i)
            if (i*3/n == cls)
                explen += utf8proc_encode_char(marks[cls][i%3], expected + explen);
    expected[explen] = 0;

    result = utf8proc_map(input, len, &output, UTF8PROC_STABLE | UTF8PROC_DECOMPOSE);
    check(result == explen, "canonical order (n=%zd): NFD returned %zd, expected %zd", n, result, explen);
    check(!memcmp(output, expected, (size_t) explen), "canonical order (n=%zd): NFD output is not the stable sort by combining class", n);
    utf8proc_free(output);
    free(input);
    free(expected);
    printf("PASSED: canonical ordering of %zd combining marks\n", n);
}

int main(void)
{
    issue128();
    issue102();
    issue317();
    issue288();
    check_canonical_order(9);     /* sorted in place */
    check_canonical_order(1<<16); /* sorted through a scratch buffer */
#ifdef UNICODE_VERSION
    printf("Unicode version: Makefile has %s, has API %s\n", UNICODE_VERSION, utf8proc_unicode_version());
    check(!strcmp(UNICODE_VERSION, utf8proc_unicode_version()), "utf8proc_unicode_version mismatch");
#endif
    printf("Misc tests SUCCEEDED.\n");
    return 0;
}
