# Unicode data generation rules.  Except for the test data files, most
# users will not use these Makefile rules, which are primarily to re-generate
# unicode_data.c when we get a new Unicode version or charwidth data; they
# require ruby, fontforge, and julia to be installed.

# programs
CURL=curl
RUBY=ruby
PERL=perl
MAKE=make
JULIA=julia
CURLFLAGS = --retry 5 --location

# use JuliaLang caching (https://github.com/staticfloat/cache.julialang.org)
# so that Travis builds do not depend on anyone's flaky servers but our own
URLCACHE=https://cache.e.ip.saba.us/

.PHONY: clean

.DELETE_ON_ERROR:

utf8proc_data.c.new: data_generator.rb UnicodeData.txt GraphemeBreakProperty.txt DerivedCoreProperties.txt CompositionExclusions.txt CaseFolding.txt CharWidths.txt
	$(RUBY) data_generator.rb < UnicodeData.txt > $@

# GNU Unifont version for font metric calculations:
UNIFONT_VERSION=8.0.01

unifont-$(UNIFONT_VERSION).ttf:
	$(CURL) $(CURLFLAGS) -o $@ $(URLCACHE)http://unifoundry.com/pub/unifont-$(UNIFONT_VERSION)/font-builds/unifont-$(UNIFONT_VERSION).ttf

unifont_upper-$(UNIFONT_VERSION).ttf:
	$(CURL) $(CURLFLAGS) -o $@ $(URLCACHE)http://unifoundry.com/pub/unifont-$(UNIFONT_VERSION)/font-builds/unifont_upper-$(UNIFONT_VERSION).ttf

CharWidths.txt: charwidths.jl unifont-$(UNIFONT_VERSION).ttf unifont_upper-$(UNIFONT_VERSION).ttf EastAsianWidth.txt
	UNIFONT_VERSION=$(UNIFONT_VERSION) $(JULIA) charwidths.jl > $@

UnicodeData.txt:
	$(CURL) $(CURLFLAGS) -o $@ -O $(URLCACHE)http://www.unicode.org/Public/UNIDATA/UnicodeData.txt

EastAsianWidth.txt:
	$(CURL) $(CURLFLAGS) -o $@ -O $(URLCACHE)http://www.unicode.org/Public/UNIDATA/EastAsianWidth.txt

GraphemeBreakProperty.txt:
	$(CURL) $(CURLFLAGS) -o $@ -O $(URLCACHE)http://www.unicode.org/Public/UCD/latest/ucd/auxiliary/GraphemeBreakProperty.txt

DerivedCoreProperties.txt:
	$(CURL) $(CURLFLAGS) -o $@ -O $(URLCACHE)http://www.unicode.org/Public/UNIDATA/DerivedCoreProperties.txt

CompositionExclusions.txt:
	$(CURL) $(CURLFLAGS) -o $@ -O $(URLCACHE)http://www.unicode.org/Public/UNIDATA/CompositionExclusions.txt

CaseFolding.txt:
	$(CURL) $(CURLFLAGS) -o $@ -O $(URLCACHE)http://www.unicode.org/Public/UNIDATA/CaseFolding.txt

NormalizationTest.txt:
	$(CURL) $(CURLFLAGS) -o $@ -O $(URLCACHE)http://www.unicode.org/Public/UNIDATA/NormalizationTest.txt

GraphemeBreakTest.txt:
	$(CURL) $(CURLFLAGS) $(URLCACHE)http://www.unicode.org/Public/UCD/latest/ucd/auxiliary/GraphemeBreakTest.txt | $(PERL) -pe 's,÷,/,g;s,×,+,g' > $@

clean:
	rm -f UnicodeData.txt EastAsianWidth.txt DerivedCoreProperties.txt CompositionExclusions.txt CaseFolding.txt NormalizationTest.txt GraphemeBreakTest.txt CharWidths.txt unifont*.ttf unifont*.sfd
