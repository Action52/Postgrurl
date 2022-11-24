-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION postgrurl" to load this file. \quit

-- Function declarations

CREATE OR REPLACE FUNCTION url_in(cstring)
RETURNS postgrurl
AS '$libdir/postgrurl'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION url_out(postgrurl)
RETURNS cstring
AS '$libdir/postgrurl'
LANGUAGE C IMMUTABLE STRICT;

-- Type Creation
CREATE TYPE postgrurl(
    INPUT           =   url_in,
    OUTPUT          =   url_out,
--    RECEIVE     =   url_rcv,
--    SEND        =   url_send,
    INTERNALLENGTH  =   1024
);

COMMENT ON TYPE postgrurl IS 'Type to handle URL strings. Implements useful functions that mimic java.net.URL class.';

--Constructors
CREATE FUNCTION URL(cstring)
 RETURNS postgrurl
 AS 'MODULE_PATHNAME', 'URL_constructor_str'
 LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION URL(cstring,cstring,integer,cstring)
 RETURNS postgrurl
 AS 'MODULE_PATHNAME', 'URL_constructor1'
 LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION URL(cstring,cstring,cstring)
 RETURNS postgrurl
 AS 'MODULE_PATHNAME', 'URL_constructor2'
 LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
--------------------------------------------

CREATE OR REPLACE FUNCTION postgrurl_eq(postgrurl, postgrurl)
RETURNS BOOLEAN
AS '$libdir/postgrurl', 'equals'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION postgrurl_gt(postgrurl, postgrurl)
RETURNS BOOLEAN
AS '$libdir/postgrurl', 'greater_than'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION postgrurl_gte(postgrurl, postgrurl)
RETURNS BOOLEAN
AS '$libdir/postgrurl', 'greater_than_equals'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION postgrurl_lt(postgrurl, postgrurl)
RETURNS BOOLEAN
AS '$libdir/postgrurl', 'less_than'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION postgrurl_lte(postgrurl, postgrurl)
RETURNS BOOLEAN
AS '$libdir/postgrurl', 'less_than_equals'
LANGUAGE C IMMUTABLE STRICT;
