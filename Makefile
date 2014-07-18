# libmojibake Makefile


# settings

cflags = -O2 -std=c99 -pedantic -Wall -fpic $(CFLAGS)
cc = $(CC) $(cflags)


# meta targets

all: c-library

c-library: libmojibake.a libmojibake.so

clean:
	rm -f utf8proc.o libmojibake.a libmojibake.so

# real targets

utf8proc.o: utf8proc.h utf8proc.c utf8proc_data.c
	$(cc) -c -o utf8proc.o utf8proc.c

libmojibake.a: utf8proc.o
	rm -f libmojibake.a
	ar rs libmojibake.a utf8proc.o

libmojibake.so: utf8proc.o
	$(cc) -shared -o libmojibake.so utf8proc.o
	chmod a-x libmojibake.so

libmojibake.dylib: utf8proc.o
	$(cc) -dynamiclib -o $@ $^ -install_name $(libdir)/$@
