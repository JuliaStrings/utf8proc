# libutf8proc Makefile

CURL=curl
RUBY=ruby

# settings

cflags = -O2 -std=c99 -pedantic -Wall -fpic $(CFLAGS)
cc = $(CC) $(cflags)


# meta targets

all: c-library

c-library: libutf8proc.a libutf8proc.so

clean:
	rm -f utf8proc.o libutf8proc.a libutf8proc.so

update: utf8proc_data.c.new

# real targets

utf8proc_data.c.new: UnicodeData.txt DerivedCoreProperties.txt CompositionExclusions.txt CaseFolding.txt
	$(RUBY) data_generator.rb < UnicodeData.txt > utf8proc_data.c.new

UnicodeData.txt:
	$(CURL) -O http://www.unicode.org/Public/UNIDATA/UnicodeData.txt

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
	ar rs libutf8proc.a utf8proc.o

libutf8proc.so: utf8proc.o
	$(cc) -shared -o libutf8proc.so utf8proc.o
	chmod a-x libutf8proc.so

libutf8proc.dylib: utf8proc.o
	$(cc) -dynamiclib -o $@ $^ -install_name $(libdir)/$@
