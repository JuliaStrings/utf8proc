/*
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
 *  File name:    ruby/utf8proc_native.c
 *
 *  Description:
 *  Native part of the ruby wrapper for libutf8proc.
 */


#include "../utf8proc.c"
#include "ruby.h"

#ifndef RSTRING_PTR
#define RSTRING_PTR(s) (RSTRING(s)->ptr)
#endif
#ifndef RSTRING_LEN
#define RSTRING_LEN(s) (RSTRING(s)->len)
#endif

typedef struct utf8proc_ruby_mapenv_struct {
  int32_t *buffer;
} utf8proc_ruby_mapenv_t;

void utf8proc_ruby_mapenv_free(utf8proc_ruby_mapenv_t *env) {
  free(env->buffer);
  free(env);
}

VALUE utf8proc_ruby_module;
VALUE utf8proc_ruby_options;
VALUE utf8proc_ruby_eUnicodeError;
VALUE utf8proc_ruby_eInvalidUtf8Error;
VALUE utf8proc_ruby_eCodeNotAssignedError;

VALUE utf8proc_ruby_map_error(ssize_t result) {
  VALUE excpt_class;
  switch (result) {
    case UTF8PROC_ERROR_NOMEM:
    excpt_class = rb_eNoMemError; break;
    case UTF8PROC_ERROR_OVERFLOW:
    case UTF8PROC_ERROR_INVALIDOPTS:
    excpt_class = rb_eArgError; break;
    case UTF8PROC_ERROR_INVALIDUTF8:
    excpt_class = utf8proc_ruby_eInvalidUtf8Error; break;
    case UTF8PROC_ERROR_NOTASSIGNED:
    excpt_class = utf8proc_ruby_eCodeNotAssignedError; break;
    default:
    excpt_class = rb_eRuntimeError;
  }
  rb_raise(excpt_class, "%s", utf8proc_errmsg(result));
  return Qnil;
}

VALUE utf8proc_ruby_map(VALUE self, VALUE str_param, VALUE options_param) {
  VALUE str;
  int options;
  VALUE env_obj;
  utf8proc_ruby_mapenv_t *env;
  ssize_t result;
  VALUE retval;
  str = StringValue(str_param);
  options = NUM2INT(options_param) & ~UTF8PROC_NULLTERM;
  env_obj = Data_Make_Struct(rb_cObject, utf8proc_ruby_mapenv_t, NULL,
    utf8proc_ruby_mapenv_free, env);
  result = utf8proc_decompose(RSTRING_PTR(str), RSTRING_LEN(str),
    NULL, 0, options);
  if (result < 0) {
    utf8proc_ruby_map_error(result);
    return Qnil;  /* needed to prevent problems with optimization */
  }
  env->buffer = ALLOC_N(int32_t, result+1);
  result = utf8proc_decompose(RSTRING_PTR(str), RSTRING_LEN(str),
    env->buffer, result, options);
  if (result < 0) {
    free(env->buffer);
    env->buffer = 0;
    utf8proc_ruby_map_error(result);
    return Qnil;  /* needed to prevent problems with optimization */
  }
  result = utf8proc_reencode(env->buffer, result, options);
  if (result < 0) {
    free(env->buffer);
    env->buffer = 0;
    utf8proc_ruby_map_error(result);
    return Qnil;  /* needed to prevent problems with optimization */
  }
  retval = rb_str_new((char *)env->buffer, result);
  free(env->buffer);
  env->buffer = 0;
  return retval;
}

static VALUE utf8proc_ruby_char(VALUE self, VALUE code_param) {
  char buffer[4];
  ssize_t result;
  int uc;
  uc = NUM2INT(code_param);
  if (!utf8proc_codepoint_valid(uc))
    rb_raise(rb_eArgError, "Invalid Unicode code point");
  result = utf8proc_encode_char(uc, buffer);
  return rb_str_new(buffer, result);
}

#define register_utf8proc_option(sym, field) \
  rb_hash_aset(utf8proc_ruby_options, ID2SYM(rb_intern(sym)), INT2FIX(field))

void Init_utf8proc_native() {
  utf8proc_ruby_module = rb_define_module("Utf8Proc");
  rb_define_module_function(utf8proc_ruby_module, "utf8map",
    utf8proc_ruby_map, 2);
  rb_define_module_function(utf8proc_ruby_module, "utf8char",
    utf8proc_ruby_char, 1);
  utf8proc_ruby_eUnicodeError = rb_define_class_under(utf8proc_ruby_module,
    "UnicodeError", rb_eStandardError);
  utf8proc_ruby_eInvalidUtf8Error = rb_define_class_under(
    utf8proc_ruby_module, "InvalidUtf8Error", utf8proc_ruby_eUnicodeError);
  utf8proc_ruby_eCodeNotAssignedError = rb_define_class_under(
    utf8proc_ruby_module, "CodeNotAssignedError",
    utf8proc_ruby_eUnicodeError);
  utf8proc_ruby_options = rb_hash_new();
  register_utf8proc_option("stable",    UTF8PROC_STABLE);
  register_utf8proc_option("compat",    UTF8PROC_COMPAT);
  register_utf8proc_option("compose",   UTF8PROC_COMPOSE);
  register_utf8proc_option("decompose", UTF8PROC_DECOMPOSE);
  register_utf8proc_option("ignore",    UTF8PROC_IGNORE);
  register_utf8proc_option("rejectna",  UTF8PROC_REJECTNA);
  register_utf8proc_option("nlf2ls",    UTF8PROC_NLF2LS);
  register_utf8proc_option("nlf2ps",    UTF8PROC_NLF2PS);
  register_utf8proc_option("nlf2lf",    UTF8PROC_NLF2LF);
  register_utf8proc_option("stripcc",   UTF8PROC_STRIPCC);
  register_utf8proc_option("casefold",  UTF8PROC_CASEFOLD);
  register_utf8proc_option("charbound", UTF8PROC_CHARBOUND);
  register_utf8proc_option("lump",      UTF8PROC_LUMP);
  register_utf8proc_option("stripmark", UTF8PROC_STRIPMARK);
  OBJ_FREEZE(utf8proc_ruby_options);
  rb_define_const(utf8proc_ruby_module, "Options", utf8proc_ruby_options);
}

