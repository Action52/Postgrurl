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
    INTERNALLENGTH  =   306
);

COMMENT ON TYPE postgrurl IS 'Type to handle URL strings. Implements useful functions that mimic java.net.URL class.';

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

CREATE OR REPLACE FUNCTION postgrurl_cmp(postgrurl, postgrurl)
RETURNS INTEGER
AS '$libdir/postgrurl', 'cmp'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION postgrurl_ne(postgrurl, postgrurl)
RETURNS BOOLEAN
AS '$libdir/postgrurl', 'not_equals'
LANGUAGE C IMMUTABLE STRICT;

-- Operators

CREATE OPERATOR = (
    LEFTARG = postgrurl,
    RIGHTARG = postgrurl,
    PROCEDURE = postgrurl_eq,
    COMMUTATOR = '=',
    NEGATOR = '<>',
    RESTRICT = eqsel,
    JOIN = eqjoinsel
);
COMMENT ON OPERATOR =(postgrurl, postgrurl) IS 'equals?';

CREATE OPERATOR <> (
    LEFTARG = postgrurl,
    RIGHTARG = postgrurl,
    PROCEDURE = postgrurl_ne,
    COMMUTATOR = '<>',
    NEGATOR = '=',
    RESTRICT = neqsel,
    JOIN = neqjoinsel
);
COMMENT ON OPERATOR <>(postgrurl, postgrurl) IS 'not equals?';

CREATE OPERATOR <(
    LEFTARG = postgrurl,
    RIGHTARG = postgrurl,
    PROCEDURE = postgrurl_lt,
    COMMUTATOR = '<',
    NEGATOR = '>=',
    RESTRICT = scalarltsel,
    JOIN = scalarltjoinsel
);
COMMENT ON OPERATOR <(postgrurl, postgrurl) IS 'less than?';

CREATE OPERATOR <=(
    LEFTARG = postgrurl,
    RIGHTARG = postgrurl,
    PROCEDURE = postgrurl_lte,
    COMMUTATOR = '<=',
    NEGATOR = '>',
    RESTRICT = scalarltsel,
    JOIN = scalarltjoinsel
);
COMMENT ON OPERATOR <=(postgrurl, postgrurl) IS 'less than or equals?';

CREATE OPERATOR >(
    LEFTARG = postgrurl,
    RIGHTARG = postgrurl,
    PROCEDURE = postgrurl_gt,
    COMMUTATOR = '>',
    NEGATOR = '<=',
    RESTRICT = scalargtsel,
    JOIN = scalargtjoinsel
);
COMMENT ON OPERATOR >(postgrurl, postgrurl) IS 'greater than?';

CREATE OPERATOR >=(
    LEFTARG = postgrurl,
    RIGHTARG = postgrurl,
    PROCEDURE = postgrurl_gte,
    COMMUTATOR = '>=',
    NEGATOR = '<',
    RESTRICT = scalargtsel,
    JOIN = scalargtjoinsel
);
COMMENT ON OPERATOR >=(postgrurl, postgrurl) IS 'greater than or equals?';

CREATE OPERATOR CLASS btree_postgrurl_ops
DEFAULT FOR TYPE postgrurl USING btree
AS
    OPERATOR    1   <,
    OPERATOR    2   <=,
    OPERATOR    3   =,
    OPERATOR    4   >=,
    OPERATOR    5   >,
    FUNCTION    1   postgrurl_cmp(postgrurl, postgrurl);