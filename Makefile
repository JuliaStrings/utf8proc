# libmojibake Makefile

CURL=curl
RUBY=ruby

# settings

cflags = -O2 -std=c99 -pedantic -Wall -fpic $(CFLAGS)
cc = $(CC) $(cflags)
AR = ar

# meta targets

all: c-library

c-library: libmojibake.a libmojibake.so

clean:
	rm -f utf8proc.o libmojibake.a libmojibake.so

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

libmojibake.a: utf8proc.o
	rm -f libmojibake.a
	$(AR) rs libmojibake.a utf8proc.o

libmojibake.so: utf8proc.o
	$(cc) -shared -o libmojibake.so utf8proc.o
	chmod a-x libmojibake.so

libmojibake.dylib: utf8proc.o
	$(cc) -dynamiclib -o $@ $^ -install_name $(libdir)/$@
