#!/usr/pkg/bin/ruby

#  This file was used to generate the 'unicode_data.c' file by parsing the
#  Unicode data file 'UnicodeData.txt' of the Unicode Character Database.
#  It is included for informational purposes only and not intended for
#  production use.


#  Copyright (c) 2009 Public Software Group e. V., Berlin, Germany
#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.


#  This file contains derived data from a modified version of the
#  Unicode data files. The following license applies to that data:
#
#  COPYRIGHT AND PERMISSION NOTICE
#
#  Copyright (c) 1991-2007 Unicode, Inc. All rights reserved. Distributed
#  under the Terms of Use in http://www.unicode.org/copyright.html.
#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of the Unicode data files and any associated documentation (the "Data
#  Files") or Unicode software and any associated documentation (the
#  "Software") to deal in the Data Files or Software without restriction,
#  including without limitation the rights to use, copy, modify, merge,
#  publish, distribute, and/or sell copies of the Data Files or Software, and
#  to permit persons to whom the Data Files or Software are furnished to do
#  so, provided that (a) the above copyright notice(s) and this permission
#  notice appear with all copies of the Data Files or Software, (b) both the
#  above copyright notice(s) and this permission notice appear in associated
#  documentation, and (c) there is clear notice in each modified Data File or
#  in the Software as well as in the documentation associated with the Data
#  File(s) or Software that the data or software has been modified.
#
#  THE DATA FILES AND SOFTWARE ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
#  KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF
#  THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS
#  INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR
#  CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
#  USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
#  TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
#  PERFORMANCE OF THE DATA FILES OR SOFTWARE.
#
#  Except as contained in this notice, the name of a copyright holder shall
#  not be used in advertising or otherwise to promote the sale, use or other
#  dealings in these Data Files or Software without prior written
#  authorization of the copyright holder.



$ignorable_list = <<END_OF_LIST
0000..0008    ; Default_Ignorable_Code_Point # Cc   [9] <control-0000>..<control-0008>
000E..001F    ; Default_Ignorable_Code_Point # Cc  [18] <control-000E>..<control-001F>
007F..0084    ; Default_Ignorable_Code_Point # Cc   [6] <control-007F>..<control-0084>
0086..009F    ; Default_Ignorable_Code_Point # Cc  [26] <control-0086>..<control-009F>
00AD          ; Default_Ignorable_Code_Point # Cf       SOFT HYPHEN
034F          ; Default_Ignorable_Code_Point # Mn       COMBINING GRAPHEME JOINER
0600..0603    ; Default_Ignorable_Code_Point # Cf   [4] ARABIC NUMBER SIGN..ARABIC SIGN SAFHA
06DD          ; Default_Ignorable_Code_Point # Cf       ARABIC END OF AYAH
070F          ; Default_Ignorable_Code_Point # Cf       SYRIAC ABBREVIATION MARK
115F..1160    ; Default_Ignorable_Code_Point # Lo   [2] HANGUL CHOSEONG FILLER..HANGUL JUNGSEONG FILLER
17B4..17B5    ; Default_Ignorable_Code_Point # Cf   [2] KHMER VOWEL INHERENT AQ..KHMER VOWEL INHERENT AA
180B..180D    ; Default_Ignorable_Code_Point # Mn   [3] MONGOLIAN FREE VARIATION SELECTOR ONE..MONGOLIAN FREE VARIATION SELECTOR THREE
200B..200F    ; Default_Ignorable_Code_Point # Cf   [5] ZERO WIDTH SPACE..RIGHT-TO-LEFT MARK
202A..202E    ; Default_Ignorable_Code_Point # Cf   [5] LEFT-TO-RIGHT EMBEDDING..RIGHT-TO-LEFT OVERRIDE
2060..2063    ; Default_Ignorable_Code_Point # Cf   [4] WORD JOINER..INVISIBLE SEPARATOR
2064..2069    ; Default_Ignorable_Code_Point # Cn   [6] <reserved-2064>..<reserved-2069>
206A..206F    ; Default_Ignorable_Code_Point # Cf   [6] INHIBIT SYMMETRIC SWAPPING..NOMINAL DIGIT SHAPES
3164          ; Default_Ignorable_Code_Point # Lo       HANGUL FILLER
D800..DFFF    ; Default_Ignorable_Code_Point # Cs [2048] <surrogate-D800>..<surrogate-DFFF>
FE00..FE0F    ; Default_Ignorable_Code_Point # Mn  [16] VARIATION SELECTOR-1..VARIATION SELECTOR-16
FEFF          ; Default_Ignorable_Code_Point # Cf       ZERO WIDTH NO-BREAK SPACE
FFA0          ; Default_Ignorable_Code_Point # Lo       HALFWIDTH HANGUL FILLER
FFF0..FFF8    ; Default_Ignorable_Code_Point # Cn   [9] <reserved-FFF0>..<reserved-FFF8>
1D173..1D17A  ; Default_Ignorable_Code_Point # Cf   [8] MUSICAL SYMBOL BEGIN BEAM..MUSICAL SYMBOL END PHRASE
E0001         ; Default_Ignorable_Code_Point # Cf       LANGUAGE TAG
E0002..E001F  ; Default_Ignorable_Code_Point # Cn  [30] <reserved-E0002>..<reserved-E001F>
E0020..E007F  ; Default_Ignorable_Code_Point # Cf  [96] TAG SPACE..CANCEL TAG
E0080..E00FF  ; Default_Ignorable_Code_Point # Cn [128] <reserved-E0080>..<reserved-E00FF>
E0100..E01EF  ; Default_Ignorable_Code_Point # Mn [240] VARIATION SELECTOR-17..VARIATION SELECTOR-256
E01F0..E0FFF  ; Default_Ignorable_Code_Point # Cn [3600] <reserved-E01F0>..<reserved-E0FFF>
END_OF_LIST

$ignorable = []
$ignorable_list.each do |entry|
  if entry =~ /^([0-9A-F]+)\.\.([0-9A-F]+)/
    $1.hex.upto($2.hex) { |e2| $ignorable << e2 }
  elsif entry =~ /^[0-9A-F]+/
    $ignorable << $&.hex
  end
end

$grapheme_extend_list = <<END_OF_LIST
0300..036F    ; Grapheme_Extend # Mn [112] COMBINING GRAVE ACCENT..COMBINING LATIN SMALL LETTER X
0483..0486    ; Grapheme_Extend # Mn   [4] COMBINING CYRILLIC TITLO..COMBINING CYRILLIC PSILI PNEUMATA
0488..0489    ; Grapheme_Extend # Me   [2] COMBINING CYRILLIC HUNDRED THOUSANDS SIGN..COMBINING CYRILLIC MILLIONS SIGN
0591..05BD    ; Grapheme_Extend # Mn  [45] HEBREW ACCENT ETNAHTA..HEBREW POINT METEG
05BF          ; Grapheme_Extend # Mn       HEBREW POINT RAFE
05C1..05C2    ; Grapheme_Extend # Mn   [2] HEBREW POINT SHIN DOT..HEBREW POINT SIN DOT
05C4..05C5    ; Grapheme_Extend # Mn   [2] HEBREW MARK UPPER DOT..HEBREW MARK LOWER DOT
05C7          ; Grapheme_Extend # Mn       HEBREW POINT QAMATS QATAN
0610..0615    ; Grapheme_Extend # Mn   [6] ARABIC SIGN SALLALLAHOU ALAYHE WASSALLAM..ARABIC SMALL HIGH TAH
064B..065E    ; Grapheme_Extend # Mn  [20] ARABIC FATHATAN..ARABIC FATHA WITH TWO DOTS
0670          ; Grapheme_Extend # Mn       ARABIC LETTER SUPERSCRIPT ALEF
06D6..06DC    ; Grapheme_Extend # Mn   [7] ARABIC SMALL HIGH LIGATURE SAD WITH LAM WITH ALEF MAKSURA..ARABIC SMALL HIGH SEEN
06DE          ; Grapheme_Extend # Me       ARABIC START OF RUB EL HIZB
06DF..06E4    ; Grapheme_Extend # Mn   [6] ARABIC SMALL HIGH ROUNDED ZERO..ARABIC SMALL HIGH MADDA
06E7..06E8    ; Grapheme_Extend # Mn   [2] ARABIC SMALL HIGH YEH..ARABIC SMALL HIGH NOON
06EA..06ED    ; Grapheme_Extend # Mn   [4] ARABIC EMPTY CENTRE LOW STOP..ARABIC SMALL LOW MEEM
0711          ; Grapheme_Extend # Mn       SYRIAC LETTER SUPERSCRIPT ALAPH
0730..074A    ; Grapheme_Extend # Mn  [27] SYRIAC PTHAHA ABOVE..SYRIAC BARREKH
07A6..07B0    ; Grapheme_Extend # Mn  [11] THAANA ABAFILI..THAANA SUKUN
07EB..07F3    ; Grapheme_Extend # Mn   [9] NKO COMBINING SHORT HIGH TONE..NKO COMBINING DOUBLE DOT ABOVE
0901..0902    ; Grapheme_Extend # Mn   [2] DEVANAGARI SIGN CANDRABINDU..DEVANAGARI SIGN ANUSVARA
093C          ; Grapheme_Extend # Mn       DEVANAGARI SIGN NUKTA
0941..0948    ; Grapheme_Extend # Mn   [8] DEVANAGARI VOWEL SIGN U..DEVANAGARI VOWEL SIGN AI
094D          ; Grapheme_Extend # Mn       DEVANAGARI SIGN VIRAMA
0951..0954    ; Grapheme_Extend # Mn   [4] DEVANAGARI STRESS SIGN UDATTA..DEVANAGARI ACUTE ACCENT
0962..0963    ; Grapheme_Extend # Mn   [2] DEVANAGARI VOWEL SIGN VOCALIC L..DEVANAGARI VOWEL SIGN VOCALIC LL
0981          ; Grapheme_Extend # Mn       BENGALI SIGN CANDRABINDU
09BC          ; Grapheme_Extend # Mn       BENGALI SIGN NUKTA
09BE          ; Grapheme_Extend # Mc       BENGALI VOWEL SIGN AA
09C1..09C4    ; Grapheme_Extend # Mn   [4] BENGALI VOWEL SIGN U..BENGALI VOWEL SIGN VOCALIC RR
09CD          ; Grapheme_Extend # Mn       BENGALI SIGN VIRAMA
09D7          ; Grapheme_Extend # Mc       BENGALI AU LENGTH MARK
09E2..09E3    ; Grapheme_Extend # Mn   [2] BENGALI VOWEL SIGN VOCALIC L..BENGALI VOWEL SIGN VOCALIC LL
0A01..0A02    ; Grapheme_Extend # Mn   [2] GURMUKHI SIGN ADAK BINDI..GURMUKHI SIGN BINDI
0A3C          ; Grapheme_Extend # Mn       GURMUKHI SIGN NUKTA
0A41..0A42    ; Grapheme_Extend # Mn   [2] GURMUKHI VOWEL SIGN U..GURMUKHI VOWEL SIGN UU
0A47..0A48    ; Grapheme_Extend # Mn   [2] GURMUKHI VOWEL SIGN EE..GURMUKHI VOWEL SIGN AI
0A4B..0A4D    ; Grapheme_Extend # Mn   [3] GURMUKHI VOWEL SIGN OO..GURMUKHI SIGN VIRAMA
0A70..0A71    ; Grapheme_Extend # Mn   [2] GURMUKHI TIPPI..GURMUKHI ADDAK
0A81..0A82    ; Grapheme_Extend # Mn   [2] GUJARATI SIGN CANDRABINDU..GUJARATI SIGN ANUSVARA
0ABC          ; Grapheme_Extend # Mn       GUJARATI SIGN NUKTA
0AC1..0AC5    ; Grapheme_Extend # Mn   [5] GUJARATI VOWEL SIGN U..GUJARATI VOWEL SIGN CANDRA E
0AC7..0AC8    ; Grapheme_Extend # Mn   [2] GUJARATI VOWEL SIGN E..GUJARATI VOWEL SIGN AI
0ACD          ; Grapheme_Extend # Mn       GUJARATI SIGN VIRAMA
0AE2..0AE3    ; Grapheme_Extend # Mn   [2] GUJARATI VOWEL SIGN VOCALIC L..GUJARATI VOWEL SIGN VOCALIC LL
0B01          ; Grapheme_Extend # Mn       ORIYA SIGN CANDRABINDU
0B3C          ; Grapheme_Extend # Mn       ORIYA SIGN NUKTA
0B3E          ; Grapheme_Extend # Mc       ORIYA VOWEL SIGN AA
0B3F          ; Grapheme_Extend # Mn       ORIYA VOWEL SIGN I
0B41..0B43    ; Grapheme_Extend # Mn   [3] ORIYA VOWEL SIGN U..ORIYA VOWEL SIGN VOCALIC R
0B4D          ; Grapheme_Extend # Mn       ORIYA SIGN VIRAMA
0B56          ; Grapheme_Extend # Mn       ORIYA AI LENGTH MARK
0B57          ; Grapheme_Extend # Mc       ORIYA AU LENGTH MARK
0B82          ; Grapheme_Extend # Mn       TAMIL SIGN ANUSVARA
0BBE          ; Grapheme_Extend # Mc       TAMIL VOWEL SIGN AA
0BC0          ; Grapheme_Extend # Mn       TAMIL VOWEL SIGN II
0BCD          ; Grapheme_Extend # Mn       TAMIL SIGN VIRAMA
0BD7          ; Grapheme_Extend # Mc       TAMIL AU LENGTH MARK
0C3E..0C40    ; Grapheme_Extend # Mn   [3] TELUGU VOWEL SIGN AA..TELUGU VOWEL SIGN II
0C46..0C48    ; Grapheme_Extend # Mn   [3] TELUGU VOWEL SIGN E..TELUGU VOWEL SIGN AI
0C4A..0C4D    ; Grapheme_Extend # Mn   [4] TELUGU VOWEL SIGN O..TELUGU SIGN VIRAMA
0C55..0C56    ; Grapheme_Extend # Mn   [2] TELUGU LENGTH MARK..TELUGU AI LENGTH MARK
0CBC          ; Grapheme_Extend # Mn       KANNADA SIGN NUKTA
0CBF          ; Grapheme_Extend # Mn       KANNADA VOWEL SIGN I
0CC2          ; Grapheme_Extend # Mc       KANNADA VOWEL SIGN UU
0CC6          ; Grapheme_Extend # Mn       KANNADA VOWEL SIGN E
0CCC..0CCD    ; Grapheme_Extend # Mn   [2] KANNADA VOWEL SIGN AU..KANNADA SIGN VIRAMA
0CD5..0CD6    ; Grapheme_Extend # Mc   [2] KANNADA LENGTH MARK..KANNADA AI LENGTH MARK
0CE2..0CE3    ; Grapheme_Extend # Mn   [2] KANNADA VOWEL SIGN VOCALIC L..KANNADA VOWEL SIGN VOCALIC LL
0D3E          ; Grapheme_Extend # Mc       MALAYALAM VOWEL SIGN AA
0D41..0D43    ; Grapheme_Extend # Mn   [3] MALAYALAM VOWEL SIGN U..MALAYALAM VOWEL SIGN VOCALIC R
0D4D          ; Grapheme_Extend # Mn       MALAYALAM SIGN VIRAMA
0D57          ; Grapheme_Extend # Mc       MALAYALAM AU LENGTH MARK
0DCA          ; Grapheme_Extend # Mn       SINHALA SIGN AL-LAKUNA
0DCF          ; Grapheme_Extend # Mc       SINHALA VOWEL SIGN AELA-PILLA
0DD2..0DD4    ; Grapheme_Extend # Mn   [3] SINHALA VOWEL SIGN KETTI IS-PILLA..SINHALA VOWEL SIGN KETTI PAA-PILLA
0DD6          ; Grapheme_Extend # Mn       SINHALA VOWEL SIGN DIGA PAA-PILLA
0DDF          ; Grapheme_Extend # Mc       SINHALA VOWEL SIGN GAYANUKITTA
0E31          ; Grapheme_Extend # Mn       THAI CHARACTER MAI HAN-AKAT
0E34..0E3A    ; Grapheme_Extend # Mn   [7] THAI CHARACTER SARA I..THAI CHARACTER PHINTHU
0E47..0E4E    ; Grapheme_Extend # Mn   [8] THAI CHARACTER MAITAIKHU..THAI CHARACTER YAMAKKAN
0EB1          ; Grapheme_Extend # Mn       LAO VOWEL SIGN MAI KAN
0EB4..0EB9    ; Grapheme_Extend # Mn   [6] LAO VOWEL SIGN I..LAO VOWEL SIGN UU
0EBB..0EBC    ; Grapheme_Extend # Mn   [2] LAO VOWEL SIGN MAI KON..LAO SEMIVOWEL SIGN LO
0EC8..0ECD    ; Grapheme_Extend # Mn   [6] LAO TONE MAI EK..LAO NIGGAHITA
0F18..0F19    ; Grapheme_Extend # Mn   [2] TIBETAN ASTROLOGICAL SIGN -KHYUD PA..TIBETAN ASTROLOGICAL SIGN SDONG TSHUGS
0F35          ; Grapheme_Extend # Mn       TIBETAN MARK NGAS BZUNG NYI ZLA
0F37          ; Grapheme_Extend # Mn       TIBETAN MARK NGAS BZUNG SGOR RTAGS
0F39          ; Grapheme_Extend # Mn       TIBETAN MARK TSA -PHRU
0F71..0F7E    ; Grapheme_Extend # Mn  [14] TIBETAN VOWEL SIGN AA..TIBETAN SIGN RJES SU NGA RO
0F80..0F84    ; Grapheme_Extend # Mn   [5] TIBETAN VOWEL SIGN REVERSED I..TIBETAN MARK HALANTA
0F86..0F87    ; Grapheme_Extend # Mn   [2] TIBETAN SIGN LCI RTAGS..TIBETAN SIGN YANG RTAGS
0F90..0F97    ; Grapheme_Extend # Mn   [8] TIBETAN SUBJOINED LETTER KA..TIBETAN SUBJOINED LETTER JA
0F99..0FBC    ; Grapheme_Extend # Mn  [36] TIBETAN SUBJOINED LETTER NYA..TIBETAN SUBJOINED LETTER FIXED-FORM RA
0FC6          ; Grapheme_Extend # Mn       TIBETAN SYMBOL PADMA GDAN
102D..1030    ; Grapheme_Extend # Mn   [4] MYANMAR VOWEL SIGN I..MYANMAR VOWEL SIGN UU
1032          ; Grapheme_Extend # Mn       MYANMAR VOWEL SIGN AI
1036..1037    ; Grapheme_Extend # Mn   [2] MYANMAR SIGN ANUSVARA..MYANMAR SIGN DOT BELOW
1039          ; Grapheme_Extend # Mn       MYANMAR SIGN VIRAMA
1058..1059    ; Grapheme_Extend # Mn   [2] MYANMAR VOWEL SIGN VOCALIC L..MYANMAR VOWEL SIGN VOCALIC LL
135F          ; Grapheme_Extend # Mn       ETHIOPIC COMBINING GEMINATION MARK
1712..1714    ; Grapheme_Extend # Mn   [3] TAGALOG VOWEL SIGN I..TAGALOG SIGN VIRAMA
1732..1734    ; Grapheme_Extend # Mn   [3] HANUNOO VOWEL SIGN I..HANUNOO SIGN PAMUDPOD
1752..1753    ; Grapheme_Extend # Mn   [2] BUHID VOWEL SIGN I..BUHID VOWEL SIGN U
1772..1773    ; Grapheme_Extend # Mn   [2] TAGBANWA VOWEL SIGN I..TAGBANWA VOWEL SIGN U
17B7..17BD    ; Grapheme_Extend # Mn   [7] KHMER VOWEL SIGN I..KHMER VOWEL SIGN UA
17C6          ; Grapheme_Extend # Mn       KHMER SIGN NIKAHIT
17C9..17D3    ; Grapheme_Extend # Mn  [11] KHMER SIGN MUUSIKATOAN..KHMER SIGN BATHAMASAT
17DD          ; Grapheme_Extend # Mn       KHMER SIGN ATTHACAN
180B..180D    ; Grapheme_Extend # Mn   [3] MONGOLIAN FREE VARIATION SELECTOR ONE..MONGOLIAN FREE VARIATION SELECTOR THREE
18A9          ; Grapheme_Extend # Mn       MONGOLIAN LETTER ALI GALI DAGALGA
1920..1922    ; Grapheme_Extend # Mn   [3] LIMBU VOWEL SIGN A..LIMBU VOWEL SIGN U
1927..1928    ; Grapheme_Extend # Mn   [2] LIMBU VOWEL SIGN E..LIMBU VOWEL SIGN O
1932          ; Grapheme_Extend # Mn       LIMBU SMALL LETTER ANUSVARA
1939..193B    ; Grapheme_Extend # Mn   [3] LIMBU SIGN MUKPHRENG..LIMBU SIGN SA-I
1A17..1A18    ; Grapheme_Extend # Mn   [2] BUGINESE VOWEL SIGN I..BUGINESE VOWEL SIGN U
1B00..1B03    ; Grapheme_Extend # Mn   [4] BALINESE SIGN ULU RICEM..BALINESE SIGN SURANG
1B34          ; Grapheme_Extend # Mn       BALINESE SIGN REREKAN
1B36..1B3A    ; Grapheme_Extend # Mn   [5] BALINESE VOWEL SIGN ULU..BALINESE VOWEL SIGN RA REPA
1B3C          ; Grapheme_Extend # Mn       BALINESE VOWEL SIGN LA LENGA
1B42          ; Grapheme_Extend # Mn       BALINESE VOWEL SIGN PEPET
1B6B..1B73    ; Grapheme_Extend # Mn   [9] BALINESE MUSICAL SYMBOL COMBINING TEGEH..BALINESE MUSICAL SYMBOL COMBINING GONG
1DC0..1DCA    ; Grapheme_Extend # Mn  [11] COMBINING DOTTED GRAVE ACCENT..COMBINING LATIN SMALL LETTER R BELOW
1DFE..1DFF    ; Grapheme_Extend # Mn   [2] COMBINING LEFT ARROWHEAD ABOVE..COMBINING RIGHT ARROWHEAD AND DOWN ARROWHEAD BELOW
200C..200D    ; Grapheme_Extend # Cf   [2] ZERO WIDTH NON-JOINER..ZERO WIDTH JOINER
20D0..20DC    ; Grapheme_Extend # Mn  [13] COMBINING LEFT HARPOON ABOVE..COMBINING FOUR DOTS ABOVE
20DD..20E0    ; Grapheme_Extend # Me   [4] COMBINING ENCLOSING CIRCLE..COMBINING ENCLOSING CIRCLE BACKSLASH
20E1          ; Grapheme_Extend # Mn       COMBINING LEFT RIGHT ARROW ABOVE
20E2..20E4    ; Grapheme_Extend # Me   [3] COMBINING ENCLOSING SCREEN..COMBINING ENCLOSING UPWARD POINTING TRIANGLE
20E5..20EF    ; Grapheme_Extend # Mn  [11] COMBINING REVERSE SOLIDUS OVERLAY..COMBINING RIGHT ARROW BELOW
302A..302F    ; Grapheme_Extend # Mn   [6] IDEOGRAPHIC LEVEL TONE MARK..HANGUL DOUBLE DOT TONE MARK
3099..309A    ; Grapheme_Extend # Mn   [2] COMBINING KATAKANA-HIRAGANA VOICED SOUND MARK..COMBINING KATAKANA-HIRAGANA SEMI-VOICED SOUND MARK
A806          ; Grapheme_Extend # Mn       SYLOTI NAGRI SIGN HASANTA
A80B          ; Grapheme_Extend # Mn       SYLOTI NAGRI SIGN ANUSVARA
A825..A826    ; Grapheme_Extend # Mn   [2] SYLOTI NAGRI VOWEL SIGN U..SYLOTI NAGRI VOWEL SIGN E
FB1E          ; Grapheme_Extend # Mn       HEBREW POINT JUDEO-SPANISH VARIKA
FE00..FE0F    ; Grapheme_Extend # Mn  [16] VARIATION SELECTOR-1..VARIATION SELECTOR-16
FE20..FE23    ; Grapheme_Extend # Mn   [4] COMBINING LIGATURE LEFT HALF..COMBINING DOUBLE TILDE RIGHT HALF
10A01..10A03  ; Grapheme_Extend # Mn   [3] KHAROSHTHI VOWEL SIGN I..KHAROSHTHI VOWEL SIGN VOCALIC R
10A05..10A06  ; Grapheme_Extend # Mn   [2] KHAROSHTHI VOWEL SIGN E..KHAROSHTHI VOWEL SIGN O
10A0C..10A0F  ; Grapheme_Extend # Mn   [4] KHAROSHTHI VOWEL LENGTH MARK..KHAROSHTHI SIGN VISARGA
10A38..10A3A  ; Grapheme_Extend # Mn   [3] KHAROSHTHI SIGN BAR ABOVE..KHAROSHTHI SIGN DOT BELOW
10A3F         ; Grapheme_Extend # Mn       KHAROSHTHI VIRAMA
1D165         ; Grapheme_Extend # Mc       MUSICAL SYMBOL COMBINING STEM
1D167..1D169  ; Grapheme_Extend # Mn   [3] MUSICAL SYMBOL COMBINING TREMOLO-1..MUSICAL SYMBOL COMBINING TREMOLO-3
1D16E..1D172  ; Grapheme_Extend # Mc   [5] MUSICAL SYMBOL COMBINING FLAG-1..MUSICAL SYMBOL COMBINING FLAG-5
1D17B..1D182  ; Grapheme_Extend # Mn   [8] MUSICAL SYMBOL COMBINING ACCENT..MUSICAL SYMBOL COMBINING LOURE
1D185..1D18B  ; Grapheme_Extend # Mn   [7] MUSICAL SYMBOL COMBINING DOIT..MUSICAL SYMBOL COMBINING TRIPLE TONGUE
1D1AA..1D1AD  ; Grapheme_Extend # Mn   [4] MUSICAL SYMBOL COMBINING DOWN BOW..MUSICAL SYMBOL COMBINING SNAP PIZZICATO
1D242..1D244  ; Grapheme_Extend # Mn   [3] COMBINING GREEK MUSICAL TRISEME..COMBINING GREEK MUSICAL PENTASEME
E0100..E01EF  ; Grapheme_Extend # Mn [240] VARIATION SELECTOR-17..VARIATION SELECTOR-256
END_OF_LIST

$grapheme_extend = []
$grapheme_extend_list.each do |entry|
  if entry =~ /^([0-9A-F]+)\.\.([0-9A-F]+)/
    $1.hex.upto($2.hex) { |e2| $grapheme_extend << e2 }
  elsif entry =~ /^[0-9A-F]+/
    $grapheme_extend << $&.hex
  end
end

$exclusions = <<END_OF_LIST
0958    #  DEVANAGARI LETTER QA
0959    #  DEVANAGARI LETTER KHHA
095A    #  DEVANAGARI LETTER GHHA
095B    #  DEVANAGARI LETTER ZA
095C    #  DEVANAGARI LETTER DDDHA
095D    #  DEVANAGARI LETTER RHA
095E    #  DEVANAGARI LETTER FA
095F    #  DEVANAGARI LETTER YYA
09DC    #  BENGALI LETTER RRA
09DD    #  BENGALI LETTER RHA
09DF    #  BENGALI LETTER YYA
0A33    #  GURMUKHI LETTER LLA
0A36    #  GURMUKHI LETTER SHA
0A59    #  GURMUKHI LETTER KHHA
0A5A    #  GURMUKHI LETTER GHHA
0A5B    #  GURMUKHI LETTER ZA
0A5E    #  GURMUKHI LETTER FA
0B5C    #  ORIYA LETTER RRA
0B5D    #  ORIYA LETTER RHA
0F43    #  TIBETAN LETTER GHA
0F4D    #  TIBETAN LETTER DDHA
0F52    #  TIBETAN LETTER DHA
0F57    #  TIBETAN LETTER BHA
0F5C    #  TIBETAN LETTER DZHA
0F69    #  TIBETAN LETTER KSSA
0F76    #  TIBETAN VOWEL SIGN VOCALIC R
0F78    #  TIBETAN VOWEL SIGN VOCALIC L
0F93    #  TIBETAN SUBJOINED LETTER GHA
0F9D    #  TIBETAN SUBJOINED LETTER DDHA
0FA2    #  TIBETAN SUBJOINED LETTER DHA
0FA7    #  TIBETAN SUBJOINED LETTER BHA
0FAC    #  TIBETAN SUBJOINED LETTER DZHA
0FB9    #  TIBETAN SUBJOINED LETTER KSSA
FB1D    #  HEBREW LETTER YOD WITH HIRIQ
FB1F    #  HEBREW LIGATURE YIDDISH YOD YOD PATAH
FB2A    #  HEBREW LETTER SHIN WITH SHIN DOT
FB2B    #  HEBREW LETTER SHIN WITH SIN DOT
FB2C    #  HEBREW LETTER SHIN WITH DAGESH AND SHIN DOT
FB2D    #  HEBREW LETTER SHIN WITH DAGESH AND SIN DOT
FB2E    #  HEBREW LETTER ALEF WITH PATAH
FB2F    #  HEBREW LETTER ALEF WITH QAMATS
FB30    #  HEBREW LETTER ALEF WITH MAPIQ
FB31    #  HEBREW LETTER BET WITH DAGESH
FB32    #  HEBREW LETTER GIMEL WITH DAGESH
FB33    #  HEBREW LETTER DALET WITH DAGESH
FB34    #  HEBREW LETTER HE WITH MAPIQ
FB35    #  HEBREW LETTER VAV WITH DAGESH
FB36    #  HEBREW LETTER ZAYIN WITH DAGESH
FB38    #  HEBREW LETTER TET WITH DAGESH
FB39    #  HEBREW LETTER YOD WITH DAGESH
FB3A    #  HEBREW LETTER FINAL KAF WITH DAGESH
FB3B    #  HEBREW LETTER KAF WITH DAGESH
FB3C    #  HEBREW LETTER LAMED WITH DAGESH
FB3E    #  HEBREW LETTER MEM WITH DAGESH
FB40    #  HEBREW LETTER NUN WITH DAGESH
FB41    #  HEBREW LETTER SAMEKH WITH DAGESH
FB43    #  HEBREW LETTER FINAL PE WITH DAGESH
FB44    #  HEBREW LETTER PE WITH DAGESH
FB46    #  HEBREW LETTER TSADI WITH DAGESH
FB47    #  HEBREW LETTER QOF WITH DAGESH
FB48    #  HEBREW LETTER RESH WITH DAGESH
FB49    #  HEBREW LETTER SHIN WITH DAGESH
FB4A    #  HEBREW LETTER TAV WITH DAGESH
FB4B    #  HEBREW LETTER VAV WITH HOLAM
FB4C    #  HEBREW LETTER BET WITH RAFE
FB4D    #  HEBREW LETTER KAF WITH RAFE
FB4E    #  HEBREW LETTER PE WITH RAFE
END_OF_LIST
$exclusions = $exclusions.chomp.split("\n").collect { |e| e.hex }

$excl_version = <<END_OF_LIST
2ADC    #  FORKING
1D15E   #  MUSICAL SYMBOL HALF NOTE
1D15F   #  MUSICAL SYMBOL QUARTER NOTE
1D160   #  MUSICAL SYMBOL EIGHTH NOTE
1D161   #  MUSICAL SYMBOL SIXTEENTH NOTE
1D162   #  MUSICAL SYMBOL THIRTY-SECOND NOTE
1D163   #  MUSICAL SYMBOL SIXTY-FOURTH NOTE
1D164   #  MUSICAL SYMBOL ONE HUNDRED TWENTY-EIGHTH NOTE
1D1BB   #  MUSICAL SYMBOL MINIMA
1D1BC   #  MUSICAL SYMBOL MINIMA BLACK
1D1BD   #  MUSICAL SYMBOL SEMIMINIMA WHITE
1D1BE   #  MUSICAL SYMBOL SEMIMINIMA BLACK
1D1BF   #  MUSICAL SYMBOL FUSA WHITE
1D1C0   #  MUSICAL SYMBOL FUSA BLACK
END_OF_LIST
$excl_version = $excl_version.chomp.split("\n").collect { |e| e.hex }

$case_folding_string = <<END_OF_LIST
#XXX THE NONEMPTY, NON-COMMENT LINES OF
#XXX http://www.unicode.org/Public/UNIDATA/CaseFolding.txt
#XXX GO HERE
END_OF_LIST

$case_folding = {}
$case_folding_string.chomp.split("\n").each do |line|
  next unless line =~ /([0-9A-F]+); [CFS]; ([0-9A-F ]+);/i
  $case_folding[$1.hex] = $2.split(" ").collect { |e| e.hex }
end

$int_array = []
$int_array_indicies = {}

def str2c(string, prefix)
  return "0" if string.nil?
  return "UTF8PROC_#{prefix}_#{string.upcase}"
end
def ary2c(array)
  return "NULL" if array.nil?
  unless $int_array_indicies[array]
    $int_array_indicies[array] = $int_array.length
    array.each { |entry| $int_array << entry }
    $int_array << -1
  end
  return "utf8proc_sequences + #{$int_array_indicies[array]}"
end

class UnicodeChar
  attr_accessor :code, :name, :category, :combining_class, :bidi_class,
                :decomp_type, :decomp_mapping,
                :bidi_mirrored,
                :uppercase_mapping, :lowercase_mapping, :titlecase_mapping
  def initialize(line)
    raise "Could not parse input." unless line =~ /^
      ([0-9A-F]+);        # code
      ([^;]+);            # name
      ([A-Z]+);           # general category
      ([0-9]+);           # canonical combining class
      ([A-Z]+);           # bidi class
      (<([A-Z]*)>)?       # decomposition type
      ((\ ?[0-9A-F]+)*);  # decompomposition mapping
      ([0-9]*);           # decimal digit
      ([0-9]*);           # digit
      ([^;]*);            # numeric
      ([YN]*);            # bidi mirrored
      ([^;]*);            # unicode 1.0 name
      ([^;]*);            # iso comment
      ([0-9A-F]*);        # simple uppercase mapping
      ([0-9A-F]*);        # simple lowercase mapping
      ([0-9A-F]*)$/ix     # simple titlecase mapping
    @code              = $1.hex
    @name              = $2
    @category          = $3
    @combining_class   = Integer($4)
    @bidi_class        = $5
    @decomp_type       = $7
    @decomp_mapping    = ($8=='') ? nil :
                         $8.split.collect { |element| element.hex }
    @bidi_mirrored     = ($13=='Y') ? true : false
    @uppercase_mapping = ($16=='') ? nil : $16.hex
    @lowercase_mapping = ($17=='') ? nil : $17.hex
    @titlecase_mapping = ($18=='') ? nil : $18.hex
  end
  def case_folding
    $case_folding[code]
  end
  def c_entry(comb1_indicies, comb2_indicies)
    "  " <<
    "{#{str2c category, 'CATEGORY'}, #{combining_class}, " <<
    "#{str2c bidi_class, 'BIDI_CLASS'}, " <<
    "#{str2c decomp_type, 'DECOMP_TYPE'}, " <<
    "#{ary2c decomp_mapping}, " <<
    "#{bidi_mirrored}, " <<
    "#{uppercase_mapping or -1}, " <<
    "#{lowercase_mapping or -1}, " <<
    "#{titlecase_mapping or -1}, " <<
    "#{comb1_indicies[code] ?
       (comb1_indicies[code]*comb2_indicies.keys.length) : -1
      }, #{comb2_indicies[code] or -1}, " <<
    "#{$exclusions.include?(code) or $excl_version.include?(code)}, " <<
    "#{$ignorable.include?(code)}, " <<
    "#{%W[Zl Zp Cc Cf].include?(category) and not [0x200C, 0x200D].include?(category)}, " <<
    "#{$grapheme_extend.include?(code)}, " <<
    "#{ary2c case_folding}},\n"
  end
end

chars = []
char_hash = {}

while gets
  if $_ =~ /^([0-9A-F]+);<[^;>,]+, First>;/i
    first = $1.hex
    gets
    char = UnicodeChar.new($_)
    raise "No last character of sequence found." unless
      $_ =~ /^([0-9A-F]+);<([^;>,]+), Last>;/i
    last = $1.hex
    name = "<#{$2}>"
    for i in first..last
      char_clone = char.clone
      char_clone.code = i
      char_clone.name = name
      char_hash[char_clone.code] = char_clone
      chars << char_clone
    end
  else
    char = UnicodeChar.new($_)
    char_hash[char.code] = char
    chars << char
  end
end

comb1st_indicies = {}
comb2nd_indicies = {}
comb_array = []

chars.each do |char|
  if char.decomp_type.nil? and char.decomp_mapping and
      char.decomp_mapping.length == 2 and
      char_hash[char.decomp_mapping[0]].combining_class == 0 and
      not $exclusions.include?(char.code)
    unless comb1st_indicies[char.decomp_mapping[0]]
      comb1st_indicies[char.decomp_mapping[0]] = comb1st_indicies.keys.length
    end
    unless comb2nd_indicies[char.decomp_mapping[1]]
      comb2nd_indicies[char.decomp_mapping[1]] = comb2nd_indicies.keys.length
    end
    comb_array[comb1st_indicies[char.decomp_mapping[0]]] ||= []
    raise "Duplicate canonical mapping" if
      comb_array[comb1st_indicies[char.decomp_mapping[0]]][
      comb2nd_indicies[char.decomp_mapping[1]]]
    comb_array[comb1st_indicies[char.decomp_mapping[0]]][
      comb2nd_indicies[char.decomp_mapping[1]]] = char.code
  end
end

properties_indicies = {}
properties = []
chars.each do |char|
  c_entry = char.c_entry(comb1st_indicies, comb2nd_indicies)
  unless properties_indicies[c_entry]
    properties_indicies[c_entry] = properties.length
    properties << c_entry
  end
end

stage1 = []
stage2 = []
for code in 0...0x110000
  next unless code % 0x100 == 0
  stage2_entry = []
  for code2 in code...(code+0x100)
    if char_hash[code2]
      stage2_entry << (properties_indicies[char_hash[code2].c_entry(
        comb1st_indicies, comb2nd_indicies)] + 1)
    else
      stage2_entry << 0
    end
  end
  old_index = stage2.index(stage2_entry)
  if old_index
    stage1 << (old_index * 0x100)
  else
    stage1 << (stage2.length * 0x100)
    stage2 << stage2_entry
  end
end

$stdout << "const int32_t utf8proc_sequences[] = {\n  "
i = 0
$int_array.each do |entry|
  i += 1
  if i == 8
    i = 0
    $stdout << "\n  "
  end
  $stdout << entry << ", "
end
$stdout << "};\n\n"

$stdout << "const uint16_t utf8proc_stage1table[] = {\n  "
i = 0
stage1.each do |entry|
  i += 1
  if i == 8
    i = 0
    $stdout << "\n  "
  end
  $stdout << entry << ", "
end
$stdout << "};\n\n"

$stdout << "const uint16_t utf8proc_stage2table[] = {\n  "
i = 0
stage2.flatten.each do |entry|
  i += 1
  if i == 8
    i = 0
    $stdout << "\n  "
  end
  $stdout << entry << ", "
end
$stdout << "};\n\n"

$stdout << "const utf8proc_property_t utf8proc_properties[] = {\n"
$stdout << "  {0, 0, 0, 0, NULL, false, -1, -1, -1, -1, -1, false},\n"
properties.each { |line|
  $stdout << line
}
$stdout << "};\n\n"

$stdout << "const int32_t utf8proc_combinations[] = {\n  "
i = 0
comb1st_indicies.keys.each_index do |a|
  comb2nd_indicies.keys.each_index do |b|
    i += 1
    if i == 8
      i = 0
      $stdout << "\n  "
    end
    $stdout << ( comb_array[a][b] or -1 ) << ", "
  end
end
$stdout << "};\n\n"

