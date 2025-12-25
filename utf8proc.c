/* -*- mode: c; c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*- */
/*
 *  Copyright (c) 2014-2021 Steven G. Johnson, Jiahao Chen, Peter Colberg, Tony Kelman, Scott P. Jones, and other contributors.
 *  Copyright (c) 2009 Public Software Group e. V., Berlin, Germany
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 */

/*
 *  This library contains derived data from a modified version of the
 *  Unicode data files.
 *
 *  The original data files are available at
 *  https://www.unicode.org/Public/UNIDATA/
 *
 *  Please notice the copyright statement in the file "utf8proc_data.c".
 */


/*
 *  File name:    utf8proc.c
 *
 *  Description:
 *  Implementation of libutf8proc.
 */


#include "utf8proc.h"

#ifndef SSIZE_MAX
#define SSIZE_MAX ((size_t)SIZE_MAX/2)
#endif
#ifndef UINT16_MAX
#  define UINT16_MAX 65535U
#endif

#include "utf8proc_data.c"


UTF8PROC_DLLEXPORT const utf8proc_int8_t utf8proc_utf8class[256] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0 };

#define UTF8PROC_HANGUL_SBASE 0xAC00
#define UTF8PROC_HANGUL_LBASE 0x1100
#define UTF8PROC_HANGUL_VBASE 0x1161
#define UTF8PROC_HANGUL_TBASE 0x11A7
#define UTF8PROC_HANGUL_LCOUNT 19
#define UTF8PROC_HANGUL_VCOUNT 21
#define UTF8PROC_HANGUL_TCOUNT 28
#define UTF8PROC_HANGUL_NCOUNT 588
#define UTF8PROC_HANGUL_SCOUNT 11172
/* END is exclusive */
#define UTF8PROC_HANGUL_L_START  0x1100
#define UTF8PROC_HANGUL_L_END    0x115A
#define UTF8PROC_HANGUL_L_FILLER 0x115F
#define UTF8PROC_HANGUL_V_START  0x1160
#define UTF8PROC_HANGUL_V_END    0x11A3
#define UTF8PROC_HANGUL_T_START  0x11A8
#define UTF8PROC_HANGUL_T_END    0x11FA
#define UTF8PROC_HANGUL_S_START  0xAC00
#define UTF8PROC_HANGUL_S_END    0xD7A4

/* Should follow semantic-versioning rules (semver.org) based on API
   compatibility.  (Note that the shared-library version number will
   be different, being based on ABI compatibility.): */
#define STRINGIZEx(x) #x
#define STRINGIZE(x) STRINGIZEx(x)
UTF8PROC_DLLEXPORT const char *utf8proc_version(void) {
  return STRINGIZE(UTF8PROC_VERSION_MAJOR) "." STRINGIZE(UTF8PROC_VERSION_MINOR) "." STRINGIZE(UTF8PROC_VERSION_PATCH) "";
}

UTF8PROC_DLLEXPORT const char *utf8proc_unicode_version(void) {
  return "17.0.0";
}

UTF8PROC_DLLEXPORT const char *utf8proc_errmsg(utf8proc_ssize_t errcode) {
  switch (errcode) {
    case UTF8PROC_ERROR_NOMEM:
    return "Memory for processing UTF-8 data could not be allocated.";
    case UTF8PROC_ERROR_OVERFLOW:
    return "UTF-8 string is too long to be processed.";
    case UTF8PROC_ERROR_INVALIDUTF8:
    return "Invalid UTF-8 string";
    case UTF8PROC_ERROR_NOTASSIGNED:
    return "Unassigned Unicode code point found in UTF-8 string.";
    case UTF8PROC_ERROR_INVALIDOPTS:
    return "Invalid options for UTF-8 processing chosen.";
    default:
    return "An unknown error occurred while processing UTF-8 data.";
  }
}

#define utf_cont(ch)  (((ch) & 0xc0) == 0x80)
UTF8PROC_DLLEXPORT utf8proc_ssize_t utf8proc_iterate(
  const utf8proc_uint8_t *str, utf8proc_ssize_t strlen, utf8proc_int32_t *dst
) {
  utf8proc_int32_t uc;
  const utf8proc_uint8_t *end;

  *dst = -1;
  if (!strlen) return 0;
  end = str + ((strlen < 0) ? 4 : strlen);
  uc = *str++;
  if (uc < 0x80) {
    *dst = uc;
    return 1;
  }
  // Must be between 0xc2 and 0xf4 inclusive to be valid
  if ((utf8proc_uint32_t)(uc - 0xc2) > (0xf4-0xc2)) return UTF8PROC_ERROR_INVALIDUTF8;
  if (uc < 0xe0) {         // 2-byte sequence
     // Must have valid continuation character
     if (str >= end || !utf_cont(*str)) return UTF8PROC_ERROR_INVALIDUTF8;
     *dst = ((uc & 0x1f)<<6) | (*str & 0x3f);
     return 2;
  }
  if (uc < 0xf0) {        // 3-byte sequence
     if ((str + 1 >= end) || !utf_cont(*str) || !utf_cont(str[1]))
        return UTF8PROC_ERROR_INVALIDUTF8;
     // Check for surrogate chars
     if (uc == 0xed && *str > 0x9f)
         return UTF8PROC_ERROR_INVALIDUTF8;
     uc = ((uc & 0xf)<<12) | ((*str & 0x3f)<<6) | (str[1] & 0x3f);
     if (uc < 0x800)
         return UTF8PROC_ERROR_INVALIDUTF8;
     *dst = uc;
     return 3;
  }
  // 4-byte sequence
  // Must have 3 valid continuation characters
  if ((str + 2 >= end) || !utf_cont(*str) || !utf_cont(str[1]) || !utf_cont(str[2]))
     return UTF8PROC_ERROR_INVALIDUTF8;
  // Make sure in correct range (0x10000 - 0x10ffff)
  if (uc == 0xf0) {
    if (*str < 0x90) return UTF8PROC_ERROR_INVALIDUTF8;
  } else if (uc == 0xf4) {
    if (*str > 0x8f) return UTF8PROC_ERROR_INVALIDUTF8;
  }
  *dst = ((uc & 7)<<18) | ((*str & 0x3f)<<12) | ((str[1] & 0x3f)<<6) | (str[2] & 0x3f);
  return 4;
}

UTF8PROC_DLLEXPORT utf8proc_bool utf8proc_codepoint_valid(utf8proc_int32_t uc) {
    return (((utf8proc_uint32_t)uc)-0xd800 > 0x07ff) && ((utf8proc_uint32_t)uc < 0x110000);
}

UTF8PROC_DLLEXPORT utf8proc_ssize_t utf8proc_encode_char(utf8proc_int32_t uc, utf8proc_uint8_t *dst) {
  if (uc < 0x00) {
    return 0;
  } else if (uc < 0x80) {
    dst[0] = (utf8proc_uint8_t) uc;
    return 1;
  } else if (uc < 0x800) {
    dst[0] = (utf8proc_uint8_t)(0xC0 + (uc >> 6));
    dst[1] = (utf8proc_uint8_t)(0x80 + (uc & 0x3F));
    return 2;
  // Note: we allow encoding 0xd800-0xdfff here, so as not to change
  // the API, however, these are actually invalid in UTF-8
  } else if (uc < 0x10000) {
    dst[0] = (utf8proc_uint8_t)(0xE0 + (uc >> 12));
    dst[1] = (utf8proc_uint8_t)(0x80 + ((uc >> 6) & 0x3F));
    dst[2] = (utf8proc_uint8_t)(0x80 + (uc & 0x3F));
    return 3;
  } else if (uc < 0x110000) {
    dst[0] = (utf8proc_uint8_t)(0xF0 + (uc >> 18));
    dst[1] = (utf8proc_uint8_t)(0x80 + ((uc >> 12) & 0x3F));
    dst[2] = (utf8proc_uint8_t)(0x80 + ((uc >> 6) & 0x3F));
    dst[3] = (utf8proc_uint8_t)(0x80 + (uc & 0x3F));
    return 4;
  } else return 0;
}

/* internal version used for inserting 0xff bytes between graphemes */
static utf8proc_ssize_t charbound_encode_char(utf8proc_int32_t uc, utf8proc_uint8_t *dst) {
   if (uc < 0x00) {
      if (uc == -1) { /* internal value used for grapheme breaks */
        dst[0] = (utf8proc_uint8_t)0xFF;
        return 1;
      }
      return 0;
   } else if (uc < 0x80) {
      dst[0] = (utf8proc_uint8_t)uc;
      return 1;
   } else if (uc < 0x800) {
      dst[0] = (utf8proc_uint8_t)(0xC0 + (uc >> 6));
      dst[1] = (utf8proc_uint8_t)(0x80 + (uc & 0x3F));
      return 2;
   } else if (uc < 0x10000) {
      dst[0] = (utf8proc_uint8_t)(0xE0 + (uc >> 12));
      dst[1] = (utf8proc_uint8_t)(0x80 + ((uc >> 6) & 0x3F));
      dst[2] = (utf8proc_uint8_t)(0x80 + (uc & 0x3F));
      return 3;
   } else if (uc < 0x110000) {
      dst[0] = (utf8proc_uint8_t)(0xF0 + (uc >> 18));
      dst[1] = (utf8proc_uint8_t)(0x80 + ((uc >> 12) & 0x3F));
      dst[2] = (utf8proc_uint8_t)(0x80 + ((uc >> 6) & 0x3F));
      dst[3] = (utf8proc_uint8_t)(0x80 + (uc & 0x3F));
      return 4;
   } else return 0;
}

/* internal "unsafe" version that does not check whether uc is in range */
static const utf8proc_property_t *unsafe_get_property(utf8proc_int32_t uc) {
  /* ASSERT: uc >= 0 && uc < 0x110000 */
  return utf8proc_properties + (
    utf8proc_stage2table[
      utf8proc_stage1table[uc >> 8] + (uc & 0xFF)
    ]
  );
}

UTF8PROC_DLLEXPORT const utf8proc_property_t *utf8proc_get_property(utf8proc_int32_t uc) {
  return uc < 0 || uc >= 0x110000 ? utf8proc_properties : unsafe_get_property(uc);
}

/* return whether there is a grapheme break between boundclasses lbc and tbc
   (according to the definition of extended grapheme clusters)

  Rule numbering refers to TR29 Version 29 (Unicode 9.0.0):
  http://www.unicode.org/reports/tr29/tr29-29.html

  CAVEATS:
   Please note that evaluation of GB10 (grapheme breaks between emoji zwj sequences)
   and GB 12/13 (regional indicator code points) require knowledge of previous characters
   and are thus not handled by this function. This may result in an incorrect break before
   an E_Modifier class codepoint and an incorrectly missing break between two
   REGIONAL_INDICATOR class code points if such support does not exist in the caller.

   See the special support in grapheme_break_extended, for required bookkeeping by the caller.
*/
static utf8proc_bool grapheme_break_simple(int lbc, int tbc) {
  return
    (lbc == UTF8PROC_BOUNDCLASS_START) ? true :       // GB1
    (lbc == UTF8PROC_BOUNDCLASS_CR &&                 // GB3
     tbc == UTF8PROC_BOUNDCLASS_LF) ? false :         // ---
    (lbc >= UTF8PROC_BOUNDCLASS_CR && lbc <= UTF8PROC_BOUNDCLASS_CONTROL) ? true :  // GB4
    (tbc >= UTF8PROC_BOUNDCLASS_CR && tbc <= UTF8PROC_BOUNDCLASS_CONTROL) ? true :  // GB5
    (lbc == UTF8PROC_BOUNDCLASS_L &&                  // GB6
     (tbc == UTF8PROC_BOUNDCLASS_L ||                 // ---
      tbc == UTF8PROC_BOUNDCLASS_V ||                 // ---
      tbc == UTF8PROC_BOUNDCLASS_LV ||                // ---
      tbc == UTF8PROC_BOUNDCLASS_LVT)) ? false :      // ---
    ((lbc == UTF8PROC_BOUNDCLASS_LV ||                // GB7
      lbc == UTF8PROC_BOUNDCLASS_V) &&                // ---
     (tbc == UTF8PROC_BOUNDCLASS_V ||                 // ---
      tbc == UTF8PROC_BOUNDCLASS_T)) ? false :        // ---
    ((lbc == UTF8PROC_BOUNDCLASS_LVT ||               // GB8
      lbc == UTF8PROC_BOUNDCLASS_T) &&                // ---
     tbc == UTF8PROC_BOUNDCLASS_T) ? false :          // ---
    (tbc == UTF8PROC_BOUNDCLASS_EXTEND ||             // GB9
     tbc == UTF8PROC_BOUNDCLASS_ZWJ ||                // ---
     tbc == UTF8PROC_BOUNDCLASS_SPACINGMARK ||        // GB9a
     lbc == UTF8PROC_BOUNDCLASS_PREPEND) ? false :    // GB9b
    (lbc == UTF8PROC_BOUNDCLASS_E_ZWG &&              // GB11 (requires additional handling below)
     tbc == UTF8PROC_BOUNDCLASS_EXTENDED_PICTOGRAPHIC) ? false : // ----
    (lbc == UTF8PROC_BOUNDCLASS_REGIONAL_INDICATOR &&          // GB12/13 (requires additional handling below)
     tbc == UTF8PROC_BOUNDCLASS_REGIONAL_INDICATOR) ? false :  // ----
    true; // GB999
}

static utf8proc_bool grapheme_break_extended(int lbc, int tbc, int licb, int ticb, utf8proc_int32_t *state)
{
  if (state) {
    int state_bc, state_icb; /* boundclass and indic_conjunct_break state */
    if (*state == 0) { /* state initialization */
      state_bc = lbc;
      state_icb = licb == UTF8PROC_INDIC_CONJUNCT_BREAK_CONSONANT ? licb : UTF8PROC_INDIC_CONJUNCT_BREAK_NONE;
    }
    else { /* lbc and licb are already encoded in *state */
      state_bc = *state & 0xff;  // 1st byte of state is bound class
      state_icb = *state >> 8;   // 2nd byte of state is indic conjunct break
    }

    utf8proc_bool break_permitted = grapheme_break_simple(state_bc, tbc) &&
       !(state_icb == UTF8PROC_INDIC_CONJUNCT_BREAK_LINKER
        && ticb == UTF8PROC_INDIC_CONJUNCT_BREAK_CONSONANT); // GB9c

    // Special support for GB9c.  Don't break between two consonants
    // separated 1+ linker characters and 0+ extend characters in any order.
    // After a consonant, we enter LINKER state after at least one linker.
    if (ticb == UTF8PROC_INDIC_CONJUNCT_BREAK_CONSONANT
        || state_icb == UTF8PROC_INDIC_CONJUNCT_BREAK_CONSONANT
        || state_icb == UTF8PROC_INDIC_CONJUNCT_BREAK_EXTEND)
      state_icb = ticb;
    else if (state_icb == UTF8PROC_INDIC_CONJUNCT_BREAK_LINKER)
      state_icb = ticb == UTF8PROC_INDIC_CONJUNCT_BREAK_EXTEND ?
                  UTF8PROC_INDIC_CONJUNCT_BREAK_LINKER : ticb;

    // Special support for GB 12/13 made possible by GB999. After two RI
    // class codepoints we want to force a break. Do this by resetting the
    // second RI's bound class to UTF8PROC_BOUNDCLASS_OTHER, to force a break
    // after that character according to GB999 (unless of course such a break is
    // forbidden by a different rule such as GB9).
    if (state_bc == tbc && tbc == UTF8PROC_BOUNDCLASS_REGIONAL_INDICATOR)
      state_bc = UTF8PROC_BOUNDCLASS_OTHER;
    // Special support for GB11 (emoji extend* zwj / emoji)
    else if (state_bc == UTF8PROC_BOUNDCLASS_EXTENDED_PICTOGRAPHIC) {
      if (tbc == UTF8PROC_BOUNDCLASS_EXTEND) // fold EXTEND codepoints into emoji
        state_bc = UTF8PROC_BOUNDCLASS_EXTENDED_PICTOGRAPHIC;
      else if (tbc == UTF8PROC_BOUNDCLASS_ZWJ)
        state_bc = UTF8PROC_BOUNDCLASS_E_ZWG; // state to record emoji+zwg combo
      else
        state_bc = tbc;
    }
    else
      state_bc = tbc;

    *state = state_bc + (state_icb << 8);
    return break_permitted;
  }
  else
    return grapheme_break_simple(lbc, tbc);
}

UTF8PROC_DLLEXPORT utf8proc_bool utf8proc_grapheme_break_stateful(
    utf8proc_int32_t c1, utf8proc_int32_t c2, utf8proc_int32_t *state) {

  const utf8proc_property_t *p1 = utf8proc_get_property(c1);
  const utf8proc_property_t *p2 = utf8proc_get_property(c2);
  return grapheme_break_extended(p1->boundclass,
                                 p2->boundclass,
                                 p1->indic_conjunct_break,
                                 p2->indic_conjunct_break,
                                 state);
}


UTF8PROC_DLLEXPORT utf8proc_bool utf8proc_grapheme_break(
    utf8proc_int32_t c1, utf8proc_int32_t c2) {
  return utf8proc_grapheme_break_stateful(c1, c2, NULL);
}

static utf8proc_int32_t seqindex_decode_entry(const utf8proc_uint16_t **entry)
{
  utf8proc_int32_t entry_cp = **entry;
  if ((entry_cp & 0xF800) == 0xD800) {
    *entry = *entry + 1;
    entry_cp = ((entry_cp & 0x03FF) << 10) | (**entry & 0x03FF);
    entry_cp += 0x10000;
  }
  return entry_cp;
}

static utf8proc_int32_t seqindex_decode_index(const utf8proc_uint32_t seqindex)
{
  const utf8proc_uint16_t *entry = &utf8proc_sequences[seqindex];
  return seqindex_decode_entry(&entry);
}

static utf8proc_ssize_t seqindex_write_char_decomposed(utf8proc_uint16_t seqindex, utf8proc_int32_t *dst, utf8proc_ssize_t bufsize, utf8proc_option_t options, int *last_boundclass) {
  utf8proc_ssize_t written = 0;
  const utf8proc_uint16_t *entry = &utf8proc_sequences[seqindex & 0x3FFF];
  int len = seqindex >> 14;
  if (len >= 3) {
    len = *entry;
    entry++;
  }
  for (; len >= 0; entry++, len--) {
    utf8proc_int32_t entry_cp = seqindex_decode_entry(&entry);

    written += utf8proc_decompose_char(entry_cp, dst ? dst+written : dst,
      (bufsize > written) ? (bufsize - written) : 0, options,
    last_boundclass);
    if (written < 0) return UTF8PROC_ERROR_OVERFLOW;
  }
  return written;
}

UTF8PROC_DLLEXPORT utf8proc_int32_t utf8proc_tolower(utf8proc_int32_t c)
{
  utf8proc_int32_t cl = utf8proc_get_property(c)->lowercase_seqindex;
  return cl != UINT16_MAX ? seqindex_decode_index((utf8proc_uint32_t)cl) : c;
}

UTF8PROC_DLLEXPORT utf8proc_int32_t utf8proc_toupper(utf8proc_int32_t c)
{
  utf8proc_int32_t cu = utf8proc_get_property(c)->uppercase_seqindex;
  return cu != UINT16_MAX ? seqindex_decode_index((utf8proc_uint32_t)cu) : c;
}

UTF8PROC_DLLEXPORT utf8proc_int32_t utf8proc_totitle(utf8proc_int32_t c)
{
  utf8proc_int32_t cu = utf8proc_get_property(c)->titlecase_seqindex;
  return cu != UINT16_MAX ? seqindex_decode_index((utf8proc_uint32_t)cu) : c;
}

UTF8PROC_DLLEXPORT int utf8proc_islower(utf8proc_int32_t c)
{
  const utf8proc_property_t *p = utf8proc_get_property(c);
  return p->lowercase_seqindex != p->uppercase_seqindex && p->lowercase_seqindex == UINT16_MAX;
}

UTF8PROC_DLLEXPORT int utf8proc_isupper(utf8proc_int32_t c)
{
  const utf8proc_property_t *p = utf8proc_get_property(c);
  return p->lowercase_seqindex != p->uppercase_seqindex && p->uppercase_seqindex == UINT16_MAX && p->category != UTF8PROC_CATEGORY_LT;
}

/* return a character width analogous to wcwidth (except portable and
   hopefully less buggy than most system wcwidth functions). */
UTF8PROC_DLLEXPORT int utf8proc_charwidth(utf8proc_int32_t c) {
  return utf8proc_get_property(c)->charwidth;
}

UTF8PROC_DLLEXPORT utf8proc_bool utf8proc_charwidth_ambiguous(utf8proc_int32_t c) {
  return utf8proc_get_property(c)->ambiguous_width;
}

UTF8PROC_DLLEXPORT utf8proc_category_t utf8proc_category(utf8proc_int32_t c) {
  return (utf8proc_category_t) utf8proc_get_property(c)->category;
}

UTF8PROC_DLLEXPORT const char *utf8proc_category_string(utf8proc_int32_t c) {
  static const char s[][3] = {"Cn","Lu","Ll","Lt","Lm","Lo","Mn","Mc","Me","Nd","Nl","No","Pc","Pd","Ps","Pe","Pi","Pf","Po","Sm","Sc","Sk","So","Zs","Zl","Zp","Cc","Cf","Cs","Co"};
  return s[utf8proc_category(c)];
}

#define utf8proc_decompose_lump(replacement_uc) \
  return utf8proc_decompose_char((replacement_uc), dst, bufsize, \
  (utf8proc_option_t)(options & ~(unsigned int)UTF8PROC_LUMP), last_boundclass)

UTF8PROC_DLLEXPORT utf8proc_ssize_t utf8proc_decompose_char(utf8proc_int32_t uc, utf8proc_int32_t *dst, utf8proc_ssize_t bufsize, utf8proc_option_t options, int *last_boundclass) {
  const utf8proc_property_t *property;
  utf8proc_propval_t category;
  utf8proc_int32_t hangul_sindex;
  if (uc < 0 || uc >= 0x110000) return UTF8PROC_ERROR_NOTASSIGNED;
  property = unsafe_get_property(uc);
  category = property->category;
  hangul_sindex = uc - UTF8PROC_HANGUL_SBASE;
  if (options & (UTF8PROC_COMPOSE|UTF8PROC_DECOMPOSE)) {
    if (hangul_sindex >= 0 && hangul_sindex < UTF8PROC_HANGUL_SCOUNT) {
      utf8proc_int32_t hangul_tindex;
      if (bufsize >= 1) {
        dst[0] = UTF8PROC_HANGUL_LBASE +
          hangul_sindex / UTF8PROC_HANGUL_NCOUNT;
        if (bufsize >= 2) dst[1] = UTF8PROC_HANGUL_VBASE +
          (hangul_sindex % UTF8PROC_HANGUL_NCOUNT) / UTF8PROC_HANGUL_TCOUNT;
      }
      hangul_tindex = hangul_sindex % UTF8PROC_HANGUL_TCOUNT;
      if (!hangul_tindex) return 2;
      if (bufsize >= 3) dst[2] = UTF8PROC_HANGUL_TBASE + hangul_tindex;
      return 3;
    }
  }
  if (options & UTF8PROC_REJECTNA) {
    if (!category) return UTF8PROC_ERROR_NOTASSIGNED;
  }
  if (options & UTF8PROC_IGNORE) {
    if (property->ignorable) return 0;
  }
  if (options & UTF8PROC_STRIPNA) {
    if (!category) return 0;
  }
  if (options & UTF8PROC_LUMP) {
    if (category == UTF8PROC_CATEGORY_ZS) utf8proc_decompose_lump(0x0020);
    if (uc == 0x2018 || uc == 0x2019 || uc == 0x02BC || uc == 0x02C8)
      utf8proc_decompose_lump(0x0027);
    if (category == UTF8PROC_CATEGORY_PD || uc == 0x2212)
      utf8proc_decompose_lump(0x002D);
    if (uc == 0x2044 || uc == 0x2215) utf8proc_decompose_lump(0x002F);
    if (uc == 0x2236) utf8proc_decompose_lump(0x003A);
    if (uc == 0x2039 || uc == 0x2329 || uc == 0x3008)
      utf8proc_decompose_lump(0x003C);
    if (uc == 0x203A || uc == 0x232A || uc == 0x3009)
      utf8proc_decompose_lump(0x003E);
    if (uc == 0x2216) utf8proc_decompose_lump(0x005C);
    if (uc == 0x02C4 || uc == 0x02C6 || uc == 0x2038 || uc == 0x2303)
      utf8proc_decompose_lump(0x005E);
    if (category == UTF8PROC_CATEGORY_PC || uc == 0x02CD)
      utf8proc_decompose_lump(0x005F);
    if (uc == 0x02CB) utf8proc_decompose_lump(0x0060);
    if (uc == 0x2223) utf8proc_decompose_lump(0x007C);
    if (uc == 0x223C) utf8proc_decompose_lump(0x007E);
    if ((options & UTF8PROC_NLF2LS) && (options & UTF8PROC_NLF2PS)) {
      if (category == UTF8PROC_CATEGORY_ZL ||
          category == UTF8PROC_CATEGORY_ZP)
        utf8proc_decompose_lump(0x000A);
    }
  }
  if (options & UTF8PROC_STRIPMARK) {
    if (category == UTF8PROC_CATEGORY_MN ||
      category == UTF8PROC_CATEGORY_MC ||
      category == UTF8PROC_CATEGORY_ME) return 0;
  }
  if (options & UTF8PROC_CASEFOLD) {
    if (property->casefold_seqindex != UINT16_MAX) {
      return seqindex_write_char_decomposed(property->casefold_seqindex, dst, bufsize, options, last_boundclass);
    }
  }
  if (options & (UTF8PROC_COMPOSE|UTF8PROC_DECOMPOSE)) {
    if (property->decomp_seqindex != UINT16_MAX &&
        (!property->decomp_type || (options & UTF8PROC_COMPAT))) {
      return seqindex_write_char_decomposed(property->decomp_seqindex, dst, bufsize, options, last_boundclass);
    }
  }
  if (options & UTF8PROC_CHARBOUND) {
    utf8proc_bool boundary;
    boundary = grapheme_break_extended(0, property->boundclass, 0, property->indic_conjunct_break,
                                       last_boundclass);
    if (boundary) {
      if (bufsize >= 1) dst[0] = -1; /* sentinel value for grapheme break */
      if (bufsize >= 2) dst[1] = uc;
      return 2;
    }
  }
  if (bufsize >= 1) *dst = uc;
  return 1;
}

UTF8PROC_DLLEXPORT utf8proc_ssize_t utf8proc_decompose(
  const utf8proc_uint8_t *str, utf8proc_ssize_t strlen,
  utf8proc_int32_t *buffer, utf8proc_ssize_t bufsize, utf8proc_option_t options
) {
    return utf8proc_decompose_custom(str, strlen, buffer, bufsize, options, NULL, NULL);
}

UTF8PROC_DLLEXPORT utf8proc_ssize_t utf8proc_decompose_custom(
  const utf8proc_uint8_t *str, utf8proc_ssize_t strlen,
  utf8proc_int32_t *buffer, utf8proc_ssize_t bufsize, utf8proc_option_t options,
  utf8proc_custom_func custom_func, void *custom_data
) {
  /* strlen will be ignored, if UTF8PROC_NULLTERM is set in options */
  utf8proc_ssize_t wpos = 0;
  if ((options & UTF8PROC_COMPOSE) && (options & UTF8PROC_DECOMPOSE))
    return UTF8PROC_ERROR_INVALIDOPTS;
  if ((options & UTF8PROC_STRIPMARK) &&
      !(options & UTF8PROC_COMPOSE) && !(options & UTF8PROC_DECOMPOSE))
    return UTF8PROC_ERROR_INVALIDOPTS;
  {
    utf8proc_int32_t uc;
    utf8proc_ssize_t rpos = 0;
    utf8proc_ssize_t decomp_result;
    int boundclass = UTF8PROC_BOUNDCLASS_START;
    while (1) {
      if (options & UTF8PROC_NULLTERM) {
        rpos += utf8proc_iterate(str + rpos, -1, &uc);
        /* checking of return value is not necessary,
           as 'uc' is < 0 in case of error */
        if (uc < 0) return UTF8PROC_ERROR_INVALIDUTF8;
        if (rpos < 0) return UTF8PROC_ERROR_OVERFLOW;
        if (uc == 0) break;
      } else {
        if (rpos >= strlen) break;
        rpos += utf8proc_iterate(str + rpos, strlen - rpos, &uc);
        if (uc < 0) return UTF8PROC_ERROR_INVALIDUTF8;
      }
      if (custom_func != NULL) {
        uc = custom_func(uc, custom_data);   /* user-specified custom mapping */
      }
      decomp_result = utf8proc_decompose_char(
        uc, buffer ? buffer+wpos : buffer, (bufsize > wpos) ? (bufsize - wpos) : 0, options,
        &boundclass
      );
      if (decomp_result < 0) return decomp_result;
      wpos += decomp_result;
      /* prohibiting integer overflows due to too long strings: */
      if (wpos < 0 ||
          wpos > (utf8proc_ssize_t)(SSIZE_MAX/sizeof(utf8proc_int32_t)/2))
        return UTF8PROC_ERROR_OVERFLOW;
    }
  }
  if ((options & (UTF8PROC_COMPOSE|UTF8PROC_DECOMPOSE)) && bufsize >= wpos) {
    utf8proc_ssize_t pos = 0;
    while (pos < wpos-1) {
      utf8proc_int32_t uc1, uc2;
      const utf8proc_property_t *property1, *property2;
      uc1 = buffer[pos];
      if (uc1 < 0) {
        /* skip grapheme break */
        pos++;
        continue;
      }
      uc2 = buffer[pos+1];
      if (uc2 < 0) {
        /* cannot recombine; skip grapheme break */
        pos+=2;
        continue;
      }
      property1 = unsafe_get_property(uc1);
      property2 = unsafe_get_property(uc2);
      if (property1->combining_class > property2->combining_class &&
          property2->combining_class > 0) {
        buffer[pos] = uc2;
        buffer[pos+1] = uc1;
        if (pos > 0) pos--; else pos++;
      } else {
        pos++;
      }
    }
  }
  return wpos;
}

UTF8PROC_DLLEXPORT utf8proc_ssize_t utf8proc_normalize_utf32(utf8proc_int32_t *buffer, utf8proc_ssize_t length, utf8proc_option_t options) {
  /* UTF8PROC_NULLTERM option will be ignored, 'length' is never ignored */
  if (options & (UTF8PROC_NLF2LS | UTF8PROC_NLF2PS | UTF8PROC_STRIPCC)) {
    utf8proc_ssize_t rpos;
    utf8proc_ssize_t wpos = 0;
    utf8proc_int32_t uc;
    for (rpos = 0; rpos < length; rpos++) {
      uc = buffer[rpos];
      if (uc == 0x000D && rpos < length-1 && buffer[rpos+1] == 0x000A) rpos++;
      if (uc == 0x000A || uc == 0x000D || uc == 0x0085 ||
          ((options & UTF8PROC_STRIPCC) && (uc == 0x000B || uc == 0x000C))) {
        if (options & UTF8PROC_NLF2LS) {
          if (options & UTF8PROC_NLF2PS) {
            buffer[wpos++] = 0x000A;
          } else {
            buffer[wpos++] = 0x2028;
          }
        } else {
          if (options & UTF8PROC_NLF2PS) {
            buffer[wpos++] = 0x2029;
          } else {
            buffer[wpos++] = 0x0020;
          }
        }
      } else if ((options & UTF8PROC_STRIPCC) &&
          (uc < 0x0020 || (uc >= 0x007F && uc < 0x00A0))) {
        if (uc == 0x0009) buffer[wpos++] = 0x0020;
      } else {
        buffer[wpos++] = uc;
      }
    }
    length = wpos;
  }
  if (options & UTF8PROC_COMPOSE) {
    utf8proc_int32_t *starter = NULL;
    const utf8proc_property_t *starter_property = NULL;
    utf8proc_propval_t max_combining_class = -1;
    utf8proc_ssize_t rpos;
    utf8proc_ssize_t wpos = 0;
    for (rpos = 0; rpos < length; rpos++) {
      utf8proc_int32_t current_char = buffer[rpos];
      const utf8proc_property_t *current_property = unsafe_get_property(current_char);
      if (starter && current_property->combining_class > max_combining_class) {
        /* combination perhaps possible */
        utf8proc_int32_t hangul_lindex;
        utf8proc_int32_t hangul_sindex;
        hangul_lindex = *starter - UTF8PROC_HANGUL_LBASE;
        if (hangul_lindex >= 0 && hangul_lindex < UTF8PROC_HANGUL_LCOUNT) {
          utf8proc_int32_t hangul_vindex;
          hangul_vindex = current_char - UTF8PROC_HANGUL_VBASE;
          if (hangul_vindex >= 0 && hangul_vindex < UTF8PROC_HANGUL_VCOUNT) {
            *starter = UTF8PROC_HANGUL_SBASE +
              (hangul_lindex * UTF8PROC_HANGUL_VCOUNT + hangul_vindex) *
              UTF8PROC_HANGUL_TCOUNT;
            starter_property = NULL;
            continue;
          }
        }
        hangul_sindex = *starter - UTF8PROC_HANGUL_SBASE;
        if (hangul_sindex >= 0 && hangul_sindex < UTF8PROC_HANGUL_SCOUNT &&
            (hangul_sindex % UTF8PROC_HANGUL_TCOUNT) == 0) {
          utf8proc_int32_t hangul_tindex;
          hangul_tindex = current_char - UTF8PROC_HANGUL_TBASE;
          if (hangul_tindex > 0 && hangul_tindex < UTF8PROC_HANGUL_TCOUNT) {
            *starter += hangul_tindex;
            starter_property = NULL;
            continue;
          }
        }
        if (!starter_property) {
          starter_property = unsafe_get_property(*starter);
        }
        int idx = starter_property->comb_index;
        if (idx < 0x3FF && current_property->comb_issecond) {
          int len = starter_property->comb_length;
          utf8proc_int32_t max_second = utf8proc_combinations_second[idx + len - 1];
          if (current_char <= max_second) {
            int off;
            // TODO: binary search? arithmetic search?
            for (off = 0; off < len; ++off) {
              utf8proc_int32_t second = utf8proc_combinations_second[idx + off];
              if (current_char < second) {
                /* not found */
                break;
              }
              if (current_char == second) {
                /* found */
                utf8proc_int32_t composition = utf8proc_combinations_combined[idx + off];
                *starter = composition;
                starter_property = NULL;
                break;
              }
            }
            if (starter_property == NULL) {
              /* found */
              continue;
            }
          }
        }
      }
      buffer[wpos] = current_char;
      if (current_property->combining_class) {
        if (current_property->combining_class > max_combining_class) {
          max_combining_class = current_property->combining_class;
        }
      } else {
        starter = buffer + wpos;
        starter_property = NULL;
        max_combining_class = -1;
      }
      wpos++;
    }
    length = wpos;
  }
  return length;
}

UTF8PROC_DLLEXPORT utf8proc_ssize_t utf8proc_reencode(utf8proc_int32_t *buffer, utf8proc_ssize_t length, utf8proc_option_t options) {
  /* UTF8PROC_NULLTERM option will be ignored, 'length' is never ignored
     ASSERT: 'buffer' has one spare byte of free space at the end! */
  length = utf8proc_normalize_utf32(buffer, length, options);
  if (length < 0) return length;
  {
    utf8proc_ssize_t rpos, wpos = 0;
    utf8proc_int32_t uc;
    if (options & UTF8PROC_CHARBOUND) {
        for (rpos = 0; rpos < length; rpos++) {
            uc = buffer[rpos];
            wpos += charbound_encode_char(uc, ((utf8proc_uint8_t *)buffer) + wpos);
        }
    } else {
        for (rpos = 0; rpos < length; rpos++) {
            uc = buffer[rpos];
            wpos += utf8proc_encode_char(uc, ((utf8proc_uint8_t *)buffer) + wpos);
        }
    }
    ((utf8proc_uint8_t *)buffer)[wpos] = 0;
    return wpos;
  }
}

UTF8PROC_DLLEXPORT utf8proc_ssize_t utf8proc_map(
  const utf8proc_uint8_t *str, utf8proc_ssize_t strlen, utf8proc_uint8_t **dstptr, utf8proc_option_t options
) {
    return utf8proc_map_custom(str, strlen, dstptr, options, NULL, NULL);
}

UTF8PROC_DLLEXPORT utf8proc_ssize_t utf8proc_map_custom(
  const utf8proc_uint8_t *str, utf8proc_ssize_t strlen, utf8proc_uint8_t **dstptr, utf8proc_option_t options,
  utf8proc_custom_func custom_func, void *custom_data
) {
  utf8proc_int32_t *buffer;
  utf8proc_ssize_t result;
  *dstptr = NULL;
  result = utf8proc_decompose_custom(str, strlen, NULL, 0, options, custom_func, custom_data);
  if (result < 0) return result;
  buffer = (utf8proc_int32_t *) malloc(((utf8proc_size_t)result) * sizeof(utf8proc_int32_t) + 1);
  if (!buffer) return UTF8PROC_ERROR_NOMEM;
  result = utf8proc_decompose_custom(str, strlen, buffer, result, options, custom_func, custom_data);
  if (result < 0) {
    free(buffer);
    return result;
  }
  result = utf8proc_reencode(buffer, result, options);
  if (result < 0) {
    free(buffer);
    return result;
  }
  {
    utf8proc_int32_t *newptr;
    newptr = (utf8proc_int32_t *) realloc(buffer, (size_t)result+1);
    if (newptr) buffer = newptr;
  }
  *dstptr = (utf8proc_uint8_t *)buffer;
  return result;
}

UTF8PROC_DLLEXPORT utf8proc_uint8_t *utf8proc_NFD(const utf8proc_uint8_t *str) {
  utf8proc_uint8_t *retval;
  utf8proc_map(str, 0, &retval, (utf8proc_option_t)(UTF8PROC_NULLTERM | UTF8PROC_STABLE |
    UTF8PROC_DECOMPOSE));
  return retval;
}

UTF8PROC_DLLEXPORT utf8proc_uint8_t *utf8proc_NFC(const utf8proc_uint8_t *str) {
  utf8proc_uint8_t *retval;
  utf8proc_map(str, 0, &retval, (utf8proc_option_t)(UTF8PROC_NULLTERM | UTF8PROC_STABLE |
    UTF8PROC_COMPOSE));
  return retval;
}

UTF8PROC_DLLEXPORT utf8proc_uint8_t *utf8proc_NFKD(const utf8proc_uint8_t *str) {
  utf8proc_uint8_t *retval;
  utf8proc_map(str, 0, &retval, (utf8proc_option_t)(UTF8PROC_NULLTERM | UTF8PROC_STABLE |
    UTF8PROC_DECOMPOSE | UTF8PROC_COMPAT));
  return retval;
}

UTF8PROC_DLLEXPORT utf8proc_uint8_t *utf8proc_NFKC(const utf8proc_uint8_t *str) {
  utf8proc_uint8_t *retval;
  utf8proc_map(str, 0, &retval, (utf8proc_option_t)(UTF8PROC_NULLTERM | UTF8PROC_STABLE |
    UTF8PROC_COMPOSE | UTF8PROC_COMPAT));
  return retval;
}

UTF8PROC_DLLEXPORT utf8proc_uint8_t *utf8proc_NFKC_Casefold(const utf8proc_uint8_t *str) {
  utf8proc_uint8_t *retval;
  utf8proc_map(str, 0, &retval, (utf8proc_option_t)(UTF8PROC_NULLTERM | UTF8PROC_STABLE |
    UTF8PROC_COMPOSE | UTF8PROC_COMPAT | UTF8PROC_CASEFOLD | UTF8PROC_IGNORE));
  return retval;
}

UTF8PROC_DLLEXPORT void utf8proc_isequal_normalized_custom(utf8proc_processing_state_t *a, utf8proc_processing_state_t *b, utf8proc_option_t options,
  utf8proc_custom_func a_custom_func, void *a_custom_data, utf8proc_custom_func b_custom_func, void *b_custom_data
) {
  const utf8proc_bool a_len_terminated = (a->str.len >= 0);
  const utf8proc_bool b_len_terminated = (b->str.len >= 0);
  /* which source string(s) we need to read more from */
  utf8proc_bool a_consume = true;
  utf8proc_bool b_consume = true;
  /* structure to simplify rollback for combining char multipass processing */
  const utf8proc_ssize_t decomposed_max_len = 8;
  struct {
    /* results of utf8proc_iterate */
    utf8proc_int32_t codepoint;
    utf8proc_ssize_t consumed;
    /* results of utf8proc_decompose_char */
    utf8proc_int32_t decomposed[8];
    utf8proc_ssize_t decomposed_len;
    int last_boundclass;
    /* combing class tracking state */
    utf8proc_ssize_t decomposed_pos;
    utf8proc_propval_t combining_class;
  } a_decomposing_current = {0},
    b_decomposing_current = {0},
    a_decomposing_combining_start = {0},
    b_decomposing_combining_start = {0},
    a_decomposing_combining_end = {0},
    b_decomposing_combining_end = {0};
  /* combining class tracking state */
  utf8proc_ssize_t pos = 0;
  utf8proc_bool combining_initialized = false;
  utf8proc_propval_t combining_class_current = 0;
  utf8proc_propval_t combining_class_next = 0;
  utf8proc_string8_view_t a_combining_start = a->str;
  utf8proc_string8_view_t b_combining_start = b->str;
  utf8proc_string8_view_t a_combining_end = a->str;
  utf8proc_string8_view_t b_combining_end = b->str;
  utf8proc_uint8_t combining_classes_finished[(UTF8PROC_COMBINING_CLASS_MAX + 1 + CHAR_BIT)/CHAR_BIT] = {0};
  const utf8proc_ssize_t combining_classes_finished_len = sizeof(combining_classes_finished)/sizeof(combining_classes_finished[0]);
  /* initialize/clear error state */
  a->error = 0;
  b->error = 0;
  a->str_at_error.ptr = NULL;
  b->str_at_error.ptr = NULL;
  a->str_at_error.len = 0;
  b->str_at_error.len = 0;
  /* force compatible options:
     - must use UTF8PROC_DECOMPOSE, not UTF8PROC_COMPOSE.
     - we choose when to add UTF8PROC_NULLTERM on a case-by-case basis (not needed currently).
     - can't use UTF8PROC_CHARBOUND because it would break `unsafe_get_property`. */
  options = (utf8proc_option_t)((options & ~(unsigned int)(UTF8PROC_COMPOSE|UTF8PROC_NULLTERM|UTF8PROC_CHARBOUND))|UTF8PROC_DECOMPOSE);
  /* primary loop: each iteration pulls data from one or both strings */
  while (1) {
    /* read a code point from each - utf8proc_iterate handles null termination with negative length on its own */
    if (a_consume) a_decomposing_current.consumed = utf8proc_iterate(a->str.ptr, a->str.len, &a_decomposing_current.codepoint);
    if (b_consume) b_decomposing_current.consumed = utf8proc_iterate(b->str.ptr, b->str.len, &b_decomposing_current.codepoint);
    /* check for errors, roll back string views if needed */
    if (a_decomposing_current.consumed < 0) {
      a->error = a_decomposing_current.consumed;
      a->str_at_error = a->str;
    }
    if (b_decomposing_current.consumed < 0) {
      b->error = b_decomposing_current.consumed;
      b->str_at_error = b->str;
    }
    if (a->error || b->error) {
      if (combining_initialized) {
        a->str = a_combining_start;
        b->str = b_combining_start;
      }
      return;
    }
    /* if we reach the end of one string, we may still need to process more
       of the other due to ignorable sequences, and the combining class code
       needs to make a judgement upon reaching the end of a combining sequence.
       so from this point forward code must be guarded against this possibility. */
    if (!combining_initialized && a_decomposing_current.consumed == 0 && b_decomposing_current.consumed == 0) {
      /* true end of both strings, must be equal */
      return;
    }
    /* apply each code point filter */
    if (a_custom_func && a_consume && a_decomposing_current.consumed) a_decomposing_current.codepoint = a_custom_func(a_decomposing_current.codepoint, a_custom_data);
    if (b_custom_func && b_consume && b_decomposing_current.consumed) b_decomposing_current.codepoint = b_custom_func(b_decomposing_current.codepoint, b_custom_data);
    /* ASCII fast path is only suitable if we consumed both at once and not in combining mode */
    if (!combining_initialized && a_consume && b_consume && a_decomposing_current.consumed && b_decomposing_current.consumed
    && a_decomposing_current.codepoint < 0x80 && b_decomposing_current.codepoint < 0x80) {
      /* fast path for common ASCII case */
      if (options & UTF8PROC_CASEFOLD) {
        if (0x41 <= a_decomposing_current.codepoint && a_decomposing_current.codepoint <= 0x5A) a_decomposing_current.codepoint += 0x20;
        if (0x41 <= b_decomposing_current.codepoint && b_decomposing_current.codepoint <= 0x5A) b_decomposing_current.codepoint += 0x20;
      }
      if (a_decomposing_current.codepoint != b_decomposing_current.codepoint) {
        /* mismatch detected */
        return;
      }
      /* equal so far */
      a->str.ptr += a_decomposing_current.consumed;
      a->str.len -= a_decomposing_current.consumed * a_len_terminated;
      b->str.ptr += b_decomposing_current.consumed;
      b->str.len -= b_decomposing_current.consumed * b_len_terminated;
      a_consume = true;
      b_consume = true;
      continue;
    }
    /* now time to decompose */
    #define UTF8PROC_LAMBDA(ab) \
    if (ab##_consume && ab##_decomposing_current.consumed) { \
      /* we got a code point, decompose it */ \
      ab##_decomposing_current.decomposed_len = utf8proc_decompose_char(ab##_decomposing_current.codepoint, \
        ab##_decomposing_current.decomposed, decomposed_max_len, options, &ab##_decomposing_current.last_boundclass); \
      ab##_decomposing_current.decomposed_pos = 0; \
      ab##_consume = false; \
      /* check for errors */ \
      if (ab##_decomposing_current.decomposed_len < 0) { \
        ab->error = ab##_decomposing_current.decomposed_len; \
        ab->str_at_error = ab->str; \
      } else if (ab##_decomposing_current.decomposed_len > decomposed_max_len) { \
        /* should never happen in practice, just for static analysis. */ \
        ab->error = UTF8PROC_ERROR_OVERFLOW; \
        ab->str_at_error = ab->str; \
      } else if (ab##_decomposing_current.decomposed_len == 0) { \
        /* ignorable sequence, need to consume more */ \
        ab->str.ptr += ab##_decomposing_current.consumed; \
        ab->str.len -= ab##_decomposing_current.consumed * ab##_len_terminated; \
        ab##_consume = true; \
      } \
    } else { \
      ab##_consume = false; \
    }
    /* run the above for both strings */
    UTF8PROC_LAMBDA(a);
    UTF8PROC_LAMBDA(b);
    #undef UTF8PROC_LAMBDA
    /* check for errors, roll back string views if needed */
    if (a->error || b->error) {
      if (combining_initialized) {
        a->str = a_combining_start;
        b->str = b_combining_start;
      }
      return;
    }
    /* check for ignorable sequences */
    if (a_consume || b_consume) {
      continue;
    }
    /* now that ignorable sequences have been handled, check for end of either string */
    if (!combining_initialized && (a_decomposing_current.consumed == 0 || b_decomposing_current.consumed == 0)) {
      /* one or both strings ended, either equal or inequal */
      return;
    }
    /* at this point both decomposed buffers need to be compared. when the
       strings are fully normalized, the decomposed chars are sorted in
       order of combining class, which could mean having to sort the entire
       decomposed string in the worst case. since we only need to compare
       them as-if they are normalized, we can just go one combining class
       at a time. we have to be careful around ends of strings to make
       sure the string views are properly updated to NOT FURTHER THAN the
       first difference in the strings, which may be a large combining seq.
       */
    while (1) {
      /* do we need to decompose more? */
      if (a_decomposing_current.consumed && a_decomposing_current.decomposed_pos >= a_decomposing_current.decomposed_len) {
        a_consume = true;
        a->str.ptr += a_decomposing_current.consumed;
        a->str.len -= a_decomposing_current.consumed * a_len_terminated;
      }
      if (b_decomposing_current.consumed && b_decomposing_current.decomposed_pos >= b_decomposing_current.decomposed_len) {
        b_consume = true;
        b->str.ptr += b_decomposing_current.consumed;
        b->str.len -= b_decomposing_current.consumed * b_len_terminated;
      }
      if (a_consume || b_consume) {
        /* use outer loop to pull more data */
        break;
      }
      /* get the combining class of each current code point, or 0 for end of string */
      if (a_decomposing_current.consumed) {
        a_decomposing_current.combining_class = unsafe_get_property(a_decomposing_current.decomposed[a_decomposing_current.decomposed_pos])->combining_class;
      } else {
        a_decomposing_current.combining_class = 0;
      }
      if (b_decomposing_current.consumed) {
        b_decomposing_current.combining_class = unsafe_get_property(b_decomposing_current.decomposed[b_decomposing_current.decomposed_pos])->combining_class;
      } else {
        b_decomposing_current.combining_class = 0;
      }
      /* static analysis guards, always false in practice */
      if (a_decomposing_current.combining_class/CHAR_BIT >= combining_classes_finished_len) {
        a->error = UTF8PROC_ERROR_OVERFLOW;
        a->str_at_error = a->str;
      }
      if (b_decomposing_current.combining_class/CHAR_BIT >= combining_classes_finished_len) {
        b->error = UTF8PROC_ERROR_OVERFLOW;
        b->str_at_error = b->str;
      }
      if (a->error || b->error) {
        if (combining_initialized) {
          a->str = a_combining_start;
          b->str = b_combining_start;
        }
        return;
      }
      /* do either have a combining class of 0 (non-combining)? */
      if (a_decomposing_current.combining_class == 0 || b_decomposing_current.combining_class == 0) {
        if (combining_initialized) {
          /* we've reached the end of the combining sequence */
          if (a_decomposing_current.combining_class == 0) {
            a_combining_end = a->str;
            a_decomposing_combining_end = a_decomposing_current;
          }
          if (b_decomposing_current.combining_class == 0) {
            b_combining_end = b->str;
            b_decomposing_combining_end = b_decomposing_current;
          }
          if (combining_class_next != 0) {
            /* prepare for the next pass */
            utf8proc_uint8_t *elem = &(combining_classes_finished[combining_class_current/CHAR_BIT]);
            const utf8proc_uint8_t mask = (utf8proc_uint8_t)(1 << (combining_class_current % CHAR_BIT));
            *elem |= mask;
            combining_class_current = combining_class_next;
            combining_class_next = 0;
            /* roll back for next pass */
            a->str = a_combining_start;
            b->str = b_combining_start;
            a_decomposing_current = a_decomposing_combining_start;
            b_decomposing_current = b_decomposing_combining_start;
            continue;
          }
          /* else exit combining mode */
          if (a_combining_end.ptr == a_combining_start.ptr && a_decomposing_combining_end.decomposed_pos == a_decomposing_combining_start.decomposed_pos
          ||  b_combining_end.ptr == b_combining_start.ptr && b_decomposing_combining_end.decomposed_pos == b_decomposing_combining_start.decomposed_pos) {
            /* didn't reach the end of one of the sequences yet - mismatch detected */
            a->str = a_combining_start;
            b->str = b_combining_start;
            return;
          }
          /* roll forward to the ends of the combining sequence */
          a->str = a_combining_end;
          b->str = b_combining_end;
          a_decomposing_current = a_decomposing_combining_end;
          b_decomposing_current = b_decomposing_combining_end;
          /* resume normal processing in outer loop */
          combining_initialized = false;
          break;
        }
        /* else not in combining mode and at least one is non-combining */
        if (a_decomposing_current.combining_class != b_decomposing_current.combining_class) {
          /* mismatch detected */
          return;
        }
        /* both are non-combining,compare the decomposed buffers */
        if (a_decomposing_current.decomposed[a_decomposing_current.decomposed_pos] != b_decomposing_current.decomposed[b_decomposing_current.decomposed_pos]) {
          /* mismatch detected */
          return;
        }
        /* equal so far */
        ++a_decomposing_current.decomposed_pos;
        ++b_decomposing_current.decomposed_pos;
        continue;
      }
      /* both nonzero combining class, initialize combining mode:
         we go one combining class at a time, comparing the decomposed chars
         of that class in order while consuming more from the input strings
         as needed and noting the next class until we reach a non-combining
         char. then, if there's another combining class, we roll back and
         start from the beginning of the sequence again. */
      if (!combining_initialized) {
        combining_class_current = a_decomposing_current.combining_class;
        combining_class_next = ((a_decomposing_current.combining_class == b_decomposing_current.combining_class)? 0 : b_decomposing_current.combining_class);
        a_combining_start = a->str;
        b_combining_start = b->str;
        a_combining_end = a->str;
        b_combining_end = b->str;
        a_decomposing_combining_end = a_decomposing_combining_start = a_decomposing_current;
        b_decomposing_combining_end = b_decomposing_combining_start = b_decomposing_current;
        for (pos = 0; pos < combining_classes_finished_len; ++pos) {
          combining_classes_finished[pos] = 0;
        }
        combining_initialized = true;
      }
      /* pull more data from one or both until we get both to be current class */
      if (a_decomposing_current.combining_class != combining_class_current) {
        /* is this an unseen class we can target next? */
        if (combining_class_next == 0) {
          const utf8proc_uint8_t elem = combining_classes_finished[a_decomposing_current.combining_class/CHAR_BIT];
          const utf8proc_uint8_t mask = (utf8proc_uint8_t)(1 << (a_decomposing_current.combining_class % CHAR_BIT));
          if ((elem & mask) == 0) {
            combining_class_next = a_decomposing_current.combining_class;
          }
        }
        ++a_decomposing_current.decomposed_pos;
      }
      if (b_decomposing_current.combining_class != combining_class_current) {
        /* is this an unseen class we can target next? */
        if (combining_class_next == 0) {
          const utf8proc_uint8_t elem = combining_classes_finished[b_decomposing_current.combining_class/CHAR_BIT];
          const utf8proc_uint8_t mask = (utf8proc_uint8_t)(1 << (b_decomposing_current.combining_class % CHAR_BIT));
          if ((elem & mask) == 0) {
            combining_class_next = b_decomposing_current.combining_class;
          }
        }
        ++b_decomposing_current.decomposed_pos;
      }
      if (a_decomposing_current.combining_class != combining_class_current || b_decomposing_current.combining_class != combining_class_current) {
        continue;
      }
      /* both are the current combining class, compare the decomposed buffers */
      if (a_decomposing_current.decomposed[a_decomposing_current.decomposed_pos] != b_decomposing_current.decomposed[b_decomposing_current.decomposed_pos]) {
        /* mismatch detected, roll back string views and exit */
        a->str = a_combining_start;
        b->str = b_combining_start;
        return;
      }
      /* equal so far */
      ++a_decomposing_current.decomposed_pos;
      ++b_decomposing_current.decomposed_pos;
      continue;
    }
  }
}

UTF8PROC_DLLEXPORT void utf8proc_isequal_normalized(utf8proc_processing_state_t *a, utf8proc_processing_state_t *b, utf8proc_option_t options) {
  utf8proc_isequal_normalized_custom(a, b, options, NULL, NULL, NULL, NULL);
}
