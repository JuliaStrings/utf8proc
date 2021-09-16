# Unicode data generation rules.  Except for the test data files, most
# users will not use these Makefile rules, which are primarily to re-generate
# unicode_data.c when we get a new Unicode version or charwidth data.
# Requires python 3.7+, curl & sed to be installed.

CURL = /usr/bin/curl --retry 5 --location

.PHONY: clean

.DELETE_ON_ERROR:

utf8proc_data.c.new: ../data_make.py UnicodeData.txt GraphemeBreakProperty.txt DerivedCoreProperties.txt \
					 CompositionExclusions.txt CaseFolding.txt emoji-data.txt EastAsianWidth.txt
	../data_make.py --format=1 --fix26 --output $@ .

# Unicode data version (must also update utf8proc_unicode_version function)
UNICODE_VERSION?=14.0.0
URL_ROOT = $(URLCACHE)https://www.unicode.org/Public/$(UNICODE_VERSION)/ucd

UnicodeData.txt:
	$(CURL) -o $@ $(URL_ROOT)/UnicodeData.txt

EastAsianWidth.txt:
	$(CURL) -o $@ $(URL_ROOT)/EastAsianWidth.txt

GraphemeBreakProperty.txt:
	$(CURL) -o $@ $(URL_ROOT)/auxiliary/GraphemeBreakProperty.txt

DerivedCoreProperties.txt:
	$(CURL) -o $@ $(URL_ROOT)/DerivedCoreProperties.txt

CompositionExclusions.txt:
	$(CURL) -o $@ $(URL_ROOT)/CompositionExclusions.txt

CaseFolding.txt:
	$(CURL) -o $@ $(URL_ROOT)/CaseFolding.txt

NormalizationTest.txt:
	$(CURL) -o $@ $(URL_ROOT)/NormalizationTest.txt

GraphemeBreakTest.txt:
	$(CURL) -o $@ $(URL_ROOT)/auxiliary/GraphemeBreakTest.txt

emoji-data.txt:
	$(CURL) -o $@ $(URL_ROOT)/emoji/emoji-data.txt

Uppercase.txt: DerivedCoreProperties.txt
	sed -En '/^# Derived Property: Uppercase/,/^# Total code points:/p' DerivedCoreProperties.txt > $@

Lowercase.txt: DerivedCoreProperties.txt
	sed -En '/^# Derived Property: Lowercase/,/^# Total code points:/p' DerivedCoreProperties.txt > $@

clean:
	rm -f UnicodeData.txt EastAsianWidth.txt GraphemeBreakProperty.txt DerivedCoreProperties.txt \
		  CompositionExclusions.txt CaseFolding.txt NormalizationTest.txt GraphemeBreakTest.txt \
		  emoji-data.txt Uppercase.txt Lowercase.txt
	rm -f utf8proc_data.c.new
