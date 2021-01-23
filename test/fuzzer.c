#include <utf8proc.h>

#ifdef __cplusplus
extern "C"
#endif
int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if(size < 1) return 0;

    if(data[size-1] != '\0') return 0;

    free(utf8proc_NFD(data));
    free(utf8proc_NFC(data));
    free(utf8proc_NFKD(data));
    free(utf8proc_NFKC(data));
    free(utf8proc_NFKC_Casefold(data));

    return 0;
}