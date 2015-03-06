# libutf8proc Makefile

CURL=curl
RUBY=ruby
PERL=perl
MAKE=make

# settings

cflags = -O2 -std=c99 -pedantic -Wall -fpic -DUTF8PROC_EXPORTS $(CFLAGS)
cc = $(CC) $(cflags)
AR = ar

OS := $(shell uname)
ifeq ($(OS),Darwin)
	SHLIB_EXT = dylib
else #TODO Windows
	SHLIB_EXT = so
endif

# meta targets

all: c-library

c-library: libutf8proc.a libutf8proc.$(SHLIB_EXT)

clean:
	rm -f utf8proc.o libutf8proc.a libutf8proc.$(SHLIB_EXT) test/normtest test/graphemetest data/UnicodeData.txt data/DerivedCoreProperties.txt data/CompositionExclusions.txt data/CaseFolding.txt data/NormalizationTest.txt data/GraphemeBreakTest.txt
	$(MAKE) -C bench clean

update: utf8proc_data.c.new
	cp -f utf8proc_data.c.new utf8proc_data.c

# real targets

utf8proc_data.c.new: data/data_generator.rb data/UnicodeData.txt data/GraphemeBreakProperty.txt data/DerivedCoreProperties.txt data/CompositionExclusions.txt data/CaseFolding.txt
	(cd data; $(RUBY) data_generator.rb < UnicodeData.txt) > utf8proc_data.c.new

data/UnicodeData.txt:
	$(CURL) -o $@ -O http://www.unicode.org/Public/UNIDATA/UnicodeData.txt

data/GraphemeBreakProperty.txt:
	$(CURL) -o $@ -O http://www.unicode.org/Public/UCD/latest/ucd/auxiliary/GraphemeBreakProperty.txt

data/DerivedCoreProperties.txt:
	$(CURL) -o $@ -O http://www.unicode.org/Public/UNIDATA/DerivedCoreProperties.txt

data/CompositionExclusions.txt:
	$(CURL) -o $@ -O http://www.unicode.org/Public/UNIDATA/CompositionExclusions.txt

data/CaseFolding.txt:
	$(CURL) -o $@ -O http://www.unicode.org/Public/UNIDATA/CaseFolding.txt

utf8proc.o: utf8proc.h utf8proc.c utf8proc_data.c
	$(cc) -c -o utf8proc.o utf8proc.c

libutf8proc.a: utf8proc.o
	rm -f libutf8proc.a
	$(AR) rs libutf8proc.a utf8proc.o

libutf8proc.so: utf8proc.o
	$(cc) -shared -o libutf8proc.$(SHLIB_EXT) utf8proc.o
	chmod a-x libutf8proc.$(SHLIB_EXT)

libutf8proc.dylib: utf8proc.o
	$(cc) -dynamiclib -o $@ $^ -install_name $(libdir)/$@


# Test programs

data/NormalizationTest.txt:
	$(CURL) -o $@ -O http://www.unicode.org/Public/UNIDATA/NormalizationTest.txt

data/GraphemeBreakTest.txt:
	$(CURL) http://www.unicode.org/Public/UCD/latest/ucd/auxiliary/GraphemeBreakTest.txt | $(PERL) -pe 's,÷,/,g;s,×,+,g' > $@

test/normtest: test/normtest.c utf8proc.o utf8proc.h test/tests.h
	$(cc) test/normtest.c utf8proc.o -o $@

test/graphemetest: test/graphemetest.c utf8proc.o utf8proc.h test/tests.h
	$(cc) test/graphemetest.c utf8proc.o -o $@

test/printproperty: test/printproperty.c utf8proc.o utf8proc.h test/tests.h
	$(cc) test/printproperty.c utf8proc.o -o $@

check: test/normtest data/NormalizationTest.txt test/graphemetest data/GraphemeBreakTest.txt
	test/normtest data/NormalizationTest.txt
	test/graphemetest data/GraphemeBreakTest.txt
