# libutf8proc Makefile


# settings

cflags = -O2 -std=c99 -pedantic -Wall -fpic $(CFLAGS)
cc = $(CC) $(cflags)
AR = ar


# meta targets

all: c-library

c-library: libutf8proc.a libutf8proc.so

clean:
	rm -f utf8proc.o libutf8proc.a libutf8proc.so

# real targets

utf8proc.o: utf8proc.h utf8proc.c utf8proc_data.c
	$(cc) -c -o utf8proc.o utf8proc.c

libutf8proc.a: utf8proc.o
	rm -f libutf8proc.a
	$(AR) rs libutf8proc.a utf8proc.o

libutf8proc.so: utf8proc.o
	$(cc) -shared -o libutf8proc.so utf8proc.o
	chmod a-x libutf8proc.so

libutf8proc.dylib: utf8proc.o
	$(cc) -dynamiclib -o $@ $^ -install_name $(libdir)/$@
