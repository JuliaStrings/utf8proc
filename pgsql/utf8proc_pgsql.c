/*
 *  Copyright (c) Public Software Group e. V., Berlin, Germany
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
 *  File name:    pgsql/utf8proc_pgsql.c
 *
 *  Description:
 *  PostgreSQL extension to provide two functions 'unifold' and 'unistrip',
 *  which can be used to case-fold and normalize index fields and
 *  optionally strip marks (e.g. accents) from strings.
 */


#include "../utf8proc.c"

#include <postgres.h>
#include <utils/elog.h>
#include <fmgr.h>
#include <string.h>
#include <unistd.h>
#include <utils/builtins.h>

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

#define UTF8PROC_PGSQL_FOLD_OPTS ( UTF8PROC_REJECTNA | UTF8PROC_COMPAT | \
  UTF8PROC_COMPOSE | UTF8PROC_STABLE | UTF8PROC_IGNORE | UTF8PROC_STRIPCC | \
  UTF8PROC_NLF2LF | UTF8PROC_CASEFOLD | UTF8PROC_LUMP )
#define UTF8PROC_PGSQL_STRIP_OPTS ( UTF8PROC_REJECTNA | UTF8PROC_COMPAT | \
  UTF8PROC_COMPOSE | UTF8PROC_STABLE | UTF8PROC_IGNORE | UTF8PROC_STRIPCC | \
  UTF8PROC_NLF2LF | UTF8PROC_CASEFOLD | UTF8PROC_LUMP | UTF8PROC_STRIPMARK )

ssize_t utf8proc_pgsql_utf8map(
  text *input_string, text **output_string_ptr, int options
) {
  ssize_t result;
  text *output_string;
  result = utf8proc_decompose(
    VARDATA(input_string), VARSIZE(input_string) - VARHDRSZ,
    NULL, 0, options
  );
  if (result < 0) return result;
  if (result > (SIZE_MAX-1-VARHDRSZ)/sizeof(int32_t))
    return UTF8PROC_ERROR_OVERFLOW;
  /* reserve one extra byte for termination */
  *output_string_ptr = palloc(result * sizeof(int32_t) + 1 + VARHDRSZ);
  output_string = *output_string_ptr;
  if (!output_string) return UTF8PROC_ERROR_NOMEM;
  result = utf8proc_decompose(
    VARDATA(input_string), VARSIZE(input_string) - VARHDRSZ,
    (int32_t *)VARDATA(output_string), result, options
  );
  if (result < 0) return result;
  result = utf8proc_reencode(
    (int32_t *)VARDATA(output_string), result, options
  );
  if (result >= 0) SET_VARSIZE(output_string, result + VARHDRSZ);
  return result;
}

void utf8proc_pgsql_utf8map_errchk(ssize_t result, text *output_string) {
  if (result < 0) {
    int sqlerrcode;
    if (output_string) pfree(output_string);
    switch(result) {
      case UTF8PROC_ERROR_NOMEM:
      sqlerrcode = ERRCODE_OUT_OF_MEMORY; break;
      case UTF8PROC_ERROR_OVERFLOW:
      sqlerrcode = ERRCODE_PROGRAM_LIMIT_EXCEEDED; break;
      case UTF8PROC_ERROR_INVALIDUTF8:
      case UTF8PROC_ERROR_NOTASSIGNED:
      return;
      default:
      sqlerrcode = ERRCODE_INTERNAL_ERROR;
    }
    ereport(ERROR, (
      errcode(sqlerrcode),
      errmsg("%s", utf8proc_errmsg(result))
    ));
  }
}

PG_FUNCTION_INFO_V1(utf8proc_pgsql_unifold);
Datum utf8proc_pgsql_unifold(PG_FUNCTION_ARGS) {
  text *input_string;
  text *output_string = NULL;
  ssize_t result;
  input_string = PG_GETARG_TEXT_P(0);
  result = utf8proc_pgsql_utf8map(
    input_string, &output_string, UTF8PROC_PGSQL_FOLD_OPTS
  );
  PG_FREE_IF_COPY(input_string, 0);
  utf8proc_pgsql_utf8map_errchk(result, output_string);
  if (result >= 0) {
    PG_RETURN_TEXT_P(output_string);
  } else {
    PG_RETURN_NULL();
  }
}

PG_FUNCTION_INFO_V1(utf8proc_pgsql_unistrip);
Datum utf8proc_pgsql_unistrip(PG_FUNCTION_ARGS) {
  text *input_string;
  text *output_string = NULL;
  ssize_t result;
  input_string = PG_GETARG_TEXT_P(0);
  result = utf8proc_pgsql_utf8map(
    input_string, &output_string, UTF8PROC_PGSQL_STRIP_OPTS
  );
  PG_FREE_IF_COPY(input_string, 0);
  utf8proc_pgsql_utf8map_errchk(result, output_string);
  if (result >= 0) {
    PG_RETURN_TEXT_P(output_string);
  } else {
    PG_RETURN_NULL();
  }
}

