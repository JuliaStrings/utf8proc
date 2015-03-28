#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utf8proc.h"
#include "util.h"

int main(int argc, char **argv)
{
	 int i;
	 int options = 0;
	 
	 for (i = 1; i < argc; ++i) {
		  if (!strcmp(argv[i], "-nfkc")) {
			   options |= UTF8PROC_STABLE|UTF8PROC_COMPOSE|UTF8PROC_COMPAT;
			   continue;
		  }
		  if (!strcmp(argv[i], "-nfkd")) {
			   options |= UTF8PROC_STABLE|UTF8PROC_DECOMPOSE|UTF8PROC_COMPAT;
			   continue;
		  }
		  if (!strcmp(argv[i], "-nfc")) {
			   options |= UTF8PROC_STABLE|UTF8PROC_COMPOSE;
			   continue;
		  }
		  if (!strcmp(argv[i], "-nfd")) {
			   options |= UTF8PROC_STABLE|UTF8PROC_DECOMPOSE;
			   continue;
		  }
		  if (!strcmp(argv[i], "-casefold")) {
			   options |= UTF8PROC_CASEFOLD;
			   continue;
		  }
		  if (argv[i][0] == '-') {
			   fprintf(stderr, "unrecognized option: %s\n", argv[i]);
			   return EXIT_FAILURE;
		  }

		  size_t len;
		  uint8_t *src = readfile(argv[i], &len);
		  if (!src) {
			   fprintf(stderr, "error reading %s\n", argv[i]);
			   return EXIT_FAILURE;
		  }
		  uint8_t *dest;
		  mytime start = gettime();
		  for (int i = 0; i < 100; ++i) {
			   utf8proc_map(src, len, &dest, options);
			   free(dest);
		  }
		  printf("%s: %g\n", argv[i], elapsed(gettime(), start) / 100);
		  free(src);
	 }

	 return EXIT_SUCCESS;
}
