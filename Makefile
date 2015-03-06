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
	rm -f utf8proc.o libutf8proc.a libutf8proc.$(SHLIB_EXT) normtest graphemetest UnicodeData.txt DerivedCoreProperties.txt CompositionExclusions.txt CaseFolding.txt NormalizationTest.txt GraphemeBreakTest.txt
	$(MAKE) -C bench clean

update: utf8proc_data.c.new
	cp -f utf8proc_data.c.new utf8proc_data.c

# real targets

utf8proc_data.c.new: data_generator.rb UnicodeData.txt GraphemeBreakProperty.txt DerivedCoreProperties.txt CompositionExclusions.txt CaseFolding.txt
	$(RUBY) data_generator.rb < UnicodeData.txt > utf8proc_data.c.new

UnicodeData.txt:
	$(CURL) -O http://www.unicode.org/Public/UNIDATA/UnicodeData.txt

GraphemeBreakProperty.txt:
	$(CURL) -O http://www.unicode.org/Public/UCD/latest/ucd/auxiliary/GraphemeBreakProperty.txt

DerivedCoreProperties.txt:
	$(CURL) -O http://www.unicode.org/Public/UNIDATA/DerivedCoreProperties.txt

CompositionExclusions.txt:
	$(CURL) -O http://www.unicode.org/Public/UNIDATA/CompositionExclusions.txt

CaseFolding.txt:
	$(CURL) -O http://www.unicode.org/Public/UNIDATA/CaseFolding.txt

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

NormalizationTest.txt:
	$(CURL) -O http://www.unicode.org/Public/UNIDATA/NormalizationTest.txt

GraphemeBreakTest.txt:
	$(CURL) http://www.unicode.org/Public/UCD/latest/ucd/auxiliary/GraphemeBreakTest.txt | $(PERL) -pe 's,÷,/,g;s,×,+,g' > $@

normtest: normtest.c utf8proc.o utf8proc.h tests.h
	$(cc) normtest.c utf8proc.o -o $@

graphemetest: graphemetest.c utf8proc.o utf8proc.h tests.h
	$(cc) graphemetest.c utf8proc.o -o $@

printproperty: printproperty.c utf8proc.o utf8proc.h tests.h
	$(cc) printproperty.c utf8proc.o -o $@

check: normtest NormalizationTest.txt graphemetest GraphemeBreakTest.txt
	./normtest
	./graphemetest
