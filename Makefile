# libutf8proc Makefile

# programs
MAKE=make
AR?=ar
CC?=gcc
INSTALL=install

# compiler settings
CFLAGS ?= -O2
PICFLAG = -fPIC
C99FLAG = -std=c99
UCFLAGS = $(CFLAGS) $(PICFLAG) $(C99FLAG) -DUTF8PROC_EXPORTS

# shared-library version MAJOR.MINOR.PATCH ... this may be *different*
# from the utf8proc version number because it indicates ABI compatibility,
# not API compatibility: MAJOR should be incremented whenever *binary*
# compatibility is broken, even if the API is backward-compatible
# Be sure to also update these in CMakeLists.txt!
MAJOR=1
MINOR=3
PATCH=0

OS := $(shell uname)
ifeq ($(OS),Darwin) # MacOS X
  SHLIB_EXT = dylib
  SHLIB_VERS_EXT = $(MAJOR).dylib
else # GNU/Linux, at least (Windows should probably use cmake)
  SHLIB_EXT = so
  SHLIB_VERS_EXT = so.$(MAJOR).$(MINOR).$(PATCH)
endif

# installation directories (for 'make install')
prefix=/usr/local
libdir=$(prefix)/lib
includedir=$(prefix)/include

# meta targets

.PHONY: all, clean, update, data

all: libutf8proc.a libutf8proc.$(SHLIB_EXT)

clean:
	rm -f utf8proc.o libutf8proc.a libutf8proc.$(SHLIB_VERS_EXT) libutf8proc.$(SHLIB_EXT) test/normtest test/graphemetest test/printproperty test/charwidth test/valid test/iterate
	$(MAKE) -C bench clean
	$(MAKE) -C data clean

data: data/utf8proc_data.c.new

update: data/utf8proc_data.c.new
	cp -f data/utf8proc_data.c.new utf8proc_data.c

# real targets

data/utf8proc_data.c.new: libutf8proc.$(SHLIB_EXT) data/data_generator.rb data/charwidths.jl
	$(MAKE) -C data utf8proc_data.c.new

utf8proc.o: utf8proc.h utf8proc.c utf8proc_data.c
	$(CC) $(UCFLAGS) -c -o utf8proc.o utf8proc.c

libutf8proc.a: utf8proc.o
	rm -f libutf8proc.a
	$(AR) rs libutf8proc.a utf8proc.o

libutf8proc.so.$(MAJOR).$(MINOR).$(PATCH): utf8proc.o
	$(CC) $(LDFLAGS) -shared -o $@ -Wl,-soname -Wl,libutf8proc.so.$(MAJOR) utf8proc.o
	chmod a-x $@

libutf8proc.so: libutf8proc.so.$(MAJOR).$(MINOR).$(PATCH)
	ln -f -s libutf8proc.so.$(MAJOR).$(MINOR).$(PATCH) $@
	ln -f -s libutf8proc.so.$(MAJOR).$(MINOR).$(PATCH) $@.$(MAJOR)

libutf8proc.$(MAJOR).dylib: utf8proc.o
	$(CC) -dynamiclib -o $@ $^ -install_name $(libdir)/$@ -Wl,-compatibility_version -Wl,$(MAJOR) -Wl,-current_version -Wl,$(MAJOR).$(MINOR).$(PATCH)

libutf8proc.dylib: libutf8proc.$(MAJOR).dylib
	ln -f -s libutf8proc.$(MAJOR).dylib $@

install: libutf8proc.a libutf8proc.$(SHLIB_EXT) libutf8proc.$(SHLIB_VERS_EXT)
	mkdir -m 755 -p $(DESTDIR)$(includedir)
	$(INSTALL) -m 644 utf8proc.h $(DESTDIR)$(includedir)
	mkdir -m 755 -p $(DESTDIR)$(libdir)
	$(INSTALL) -m 644 libutf8proc.a $(DESTDIR)$(libdir)
	$(INSTALL) -m 755 libutf8proc.$(SHLIB_VERS_EXT) $(DESTDIR)$(libdir)
	ln -f -s $(libdir)/libutf8proc.$(SHLIB_VERS_EXT) $(DESTDIR)$(libdir)/libutf8proc.$(SHLIB_EXT)
ifneq ($(OS),Darwin)
	ln -f -s $(libdir)/libutf8proc.$(SHLIB_VERS_EXT) $(DESTDIR)$(libdir)/libutf8proc.so.$(MAJOR)
	ln -f -s $(libdir)/libutf8proc.$(SHLIB_VERS_EXT) $(DESTDIR)$(libdir)/libutf8proc.so.$(MAJOR).$(MINOR)
endif

# Test programs

data/NormalizationTest.txt:
	$(MAKE) -C data NormalizationTest.txt

data/GraphemeBreakTest.txt:
	$(MAKE) -C data GraphemeBreakTest.txt

test/normtest: test/normtest.c utf8proc.o utf8proc.h test/tests.h
	$(CC) $(UCFLAGS) test/normtest.c utf8proc.o -o $@

test/graphemetest: test/graphemetest.c utf8proc.o utf8proc.h test/tests.h
	$(CC) $(UCFLAGS) test/graphemetest.c utf8proc.o -o $@

test/printproperty: test/printproperty.c utf8proc.o utf8proc.h test/tests.h
	$(CC) $(UCFLAGS) test/printproperty.c utf8proc.o -o $@

test/charwidth: test/charwidth.c utf8proc.o utf8proc.h test/tests.h
	$(CC) $(UCFLAGS) test/charwidth.c utf8proc.o -o $@

test/valid: test/valid.c utf8proc.o utf8proc.h test/tests.h
	$(CC) $(UCFLAGS) test/valid.c utf8proc.o -o $@

test/iterate: test/iterate.c utf8proc.o utf8proc.h test/tests.h
	$(CC) $(UCFLAGS) test/iterate.c utf8proc.o -o $@

test/case: test/case.c utf8proc.o utf8proc.h test/tests.h
	$(CC) $(UCFLAGS) test/case.c utf8proc.o -o $@

check: test/normtest data/NormalizationTest.txt test/graphemetest data/GraphemeBreakTest.txt test/printproperty test/case test/charwidth test/valid test/iterate bench/bench.c bench/util.c bench/util.h utf8proc.o
	$(MAKE) -C bench
	test/normtest data/NormalizationTest.txt
	test/graphemetest data/GraphemeBreakTest.txt
	test/charwidth
	test/valid
	test/iterate
	test/case
