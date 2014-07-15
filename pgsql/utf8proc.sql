CREATE OR REPLACE FUNCTION unifold (text) RETURNS text
  LANGUAGE 'c' IMMUTABLE STRICT AS '$libdir/utf8proc_pgsql.so',
  'utf8proc_pgsql_unifold';
CREATE OR REPLACE FUNCTION unistrip (text) RETURNS text
  LANGUAGE 'c' IMMUTABLE STRICT AS '$libdir/utf8proc_pgsql.so',
  'utf8proc_pgsql_unistrip';
