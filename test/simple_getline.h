/* simplistic, portable replacement for getline, sufficient for our tests */
static size_t simple_getline(char buf[8192], FILE *f) {
    size_t i = 0;
    while (i < 1023) {
        int c = getc(f);
        if (c == EOF || c == '\n') break;
        buf[i++] = (char) c;
    }
    buf[i] = 0;
    return i;
}
