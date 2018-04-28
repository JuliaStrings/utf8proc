/* Miscellaneous tests, e.g. regression tests */

#include "tests.h"

static void issue128(void) /* #128 */
{
    utf8proc_uint8_t input[] = {0x72, 0xcc, 0x87, 0xcc, 0xa3, 0x00}; /* "r\u0307\u0323" */
    utf8proc_uint8_t nfc[] = {0xe1, 0xb9, 0x9b, 0xcc, 0x87, 0x00}; /* "\u1E5B\u0307" */
    utf8proc_uint8_t nfd[] = {0x72, 0xcc, 0xa3, 0xcc, 0x87, 0x00}; /* "r\u0323\u0307" */
    utf8proc_uint8_t *nfc_out, *nfd_out;
    nfc_out = utf8proc_NFC(input);
    printf("NFC \"%s\" -> \"%s\" vs. \"%s\"\n", (char*)input, (char*)nfc_out, (char*)nfc);
    check(strlen((char*) nfc_out) == 5, "incorrect nfc length");
    check(!memcmp(nfc, nfc_out, 6), "incorrect nfc data");
    nfd_out = utf8proc_NFD(input);
    printf("NFD \"%s\" -> \"%s\" vs. \"%s\"\n", (char*)input, (char*)nfd_out, (char*)nfd);
    check(strlen((char*) nfd_out) == 5, "incorrect nfd length");
    check(!memcmp(nfd, nfd_out, 6), "incorrect nfd data");
    free(nfd_out); free(nfc_out);
}

int main(void)
{
    issue128();
    printf("Misc tests SUCCEEDED.\n");
    return 0;
}
