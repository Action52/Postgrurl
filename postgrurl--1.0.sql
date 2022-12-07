/*************************************************************************************************************
*    SQL Code that implements a URL handler for Postgres extensions. This handler mimics java.net.URL class. *
*    Authors:                                                                                                *
*        Luis Alfredo Leon - luis.leon.villapun@ulb.be                                                       *
*        Maren Hoschek - maren.hoschek@ulb.be                                                                *
*        Satria Bagus - satria.wicaksono@ulb.be                                                              *
*        Sayyor Yusupov - sayyor.yusupov@ulb.be                                                              *
*************************************************************************************************************/

\echo Use "CREATE EXTENSION postgrurl" to load this file. \quit

/*********************************************************************************
*  Data type functions                                                           *
*********************************************************************************/

CREATE OR REPLACE FUNCTION url_in(cstring)
RETURNS postgrurl
AS '$libdir/postgrurl'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION url_out(postgrurl)
RETURNS cstring
AS '$libdir/postgrurl'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION url_recv(internal)
RETURNS postgrurl
AS '$libdir/postgrurl'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION url_send(postgrurl)
RETURNS bytea
AS '$libdir/postgrurl'
LANGUAGE C IMMUTABLE STRICT;

/********************************************************************************
*  Data type creation                                                           *
********************************************************************************/

CREATE TYPE postgrurl(
    INPUT           =   url_in,
    OUTPUT          =   url_out,
    RECEIVE         =   url_recv,
    SEND            =   url_send,
    INTERNALLENGTH  =   1024
);

COMMENT ON TYPE postgrurl IS 'Type to handle URL strings. Implements useful functions that mimic java.net.URL class.';

/*********************************************************************************
*  Constructors                                                                  *
*********************************************************************************/

CREATE FUNCTION URL(cstring)
 RETURNS postgrurl
 AS 'MODULE_PATHNAME', 'URLPostgresFromString'
 LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION URL(cstring,cstring,integer,cstring)
 RETURNS postgrurl
 AS 'MODULE_PATHNAME', 'URLPostgresFromProtocolHostPortFile'
 LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION URL(cstring,cstring,cstring)
 RETURNS postgrurl
 AS 'MODULE_PATHNAME', 'URLPostgresFromProtocolHostFile'
 LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION URL(postgrurl,cstring)
 RETURNS postgrurl
 AS 'MODULE_PATHNAME', 'URLPostgresFromContext'
 LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/********************************************************************************
*  Equality functions                                                           *
********************************************************************************/

CREATE OR REPLACE FUNCTION _postgrurl_eq(postgrurl, postgrurl)
RETURNS BOOLEAN
AS '$libdir/postgrurl', 'equals'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION _postgrurl_gt(postgrurl, postgrurl)
RETURNS BOOLEAN
AS '$libdir/postgrurl', 'greater_than'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION _postgrurl_gte(postgrurl, postgrurl)
RETURNS BOOLEAN
AS '$libdir/postgrurl', 'greater_than_equals'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION _postgrurl_lt(postgrurl, postgrurl)
RETURNS BOOLEAN
AS '$libdir/postgrurl', 'less_than'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION _postgrurl_lte(postgrurl, postgrurl)
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

/********************************************************************************
*  Operators and Operator Class for Btree                                       *
*********************************************************************************/

CREATE OPERATOR = (
    LEFTARG = postgrurl,
    RIGHTARG = postgrurl,
    PROCEDURE = _postgrurl_eq,
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
    PROCEDURE = _postgrurl_lt,
    COMMUTATOR = '<',
    NEGATOR = '>=',
    RESTRICT = scalarltsel,
    JOIN = scalarltjoinsel
);
COMMENT ON OPERATOR <(postgrurl, postgrurl) IS 'less than?';

CREATE OPERATOR <=(
    LEFTARG = postgrurl,
    RIGHTARG = postgrurl,
    PROCEDURE = _postgrurl_lte,
    COMMUTATOR = '<=',
    NEGATOR = '>',
    RESTRICT = scalarltsel,
    JOIN = scalarltjoinsel
);
COMMENT ON OPERATOR <=(postgrurl, postgrurl) IS 'less than or equals?';

CREATE OPERATOR >(
    LEFTARG = postgrurl,
    RIGHTARG = postgrurl,
    PROCEDURE = _postgrurl_gt,
    COMMUTATOR = '>',
    NEGATOR = '<=',
    RESTRICT = scalargtsel,
    JOIN = scalargtjoinsel
);
COMMENT ON OPERATOR >(postgrurl, postgrurl) IS 'greater than?';

CREATE OPERATOR >=(
    LEFTARG = postgrurl,
    RIGHTARG = postgrurl,
    PROCEDURE = _postgrurl_gte,
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


/*********************************************************************************
*  Helper functions                                                              *
*********************************************************************************/

#if POSTGRESQL_VERSION_NUMBER >= 120000
CREATE FUNCTION postgrurl_support_function(internal)
RETURNS internal
AS '$libdir/postgrurl', 'postgrurl_support_function'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
#endif //POSTGRESQL_VERSION_NUMBER >= 120000

CREATE OR REPLACE FUNCTION getFile(postgrurl)
RETURNS cstring
AS '$libdir/postgrurl', 'getFile'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION getHost(postgrurl)
RETURNS cstring
AS '$libdir/postgrurl', 'getHost'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION getPort(postgrurl)
RETURNS INTEGER
AS '$libdir/postgrurl', 'getPort'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION getDefaultPort(postgrurl)
RETURNS INTEGER
AS '$libdir/postgrurl', 'getDefaultPort'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION getProtocol(postgrurl)
RETURNS cstring
AS '$libdir/postgrurl', 'getProtocol'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION getQuery(postgrurl)
RETURNS cstring
AS '$libdir/postgrurl', 'getQuery'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION getRef(postgrurl)
RETURNS cstring
AS '$libdir/postgrurl', 'getRef'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION _sameHost(postgrurl, postgrurl)
RETURNS BOOLEAN
AS '$libdir/postgrurl', 'sameHost'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION sameHost(postgrurl, postgrurl)
RETURNS BOOLEAN
AS 'SELECT $1 >= $2 AND _sameHost($1, $2)'
LANGUAGE SQL IMMUTABLE STRICT PARALLEL SAFE;



CREATE OR REPLACE FUNCTION _sameFile(postgrurl, postgrurl)
RETURNS BOOLEAN
AS '$libdir/postgrurl', 'sameFile'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION sameFile(postgrurl, postgrurl)
RETURNS BOOLEAN
AS 'SELECT $1 >= $2 AND _sameFile($1, $2)'
LANGUAGE SQL IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION _equals(postgrurl, postgrurl)
RETURNS BOOLEAN
AS '$libdir/postgrurl', 'equals'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION equals(postgrurl, postgrurl)
RETURNS BOOLEAN
AS 'SELECT $1 = $2 AND _equals($1, $2)'
LANGUAGE SQL IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION toString(postgrurl)
RETURNS cstring
AS '$libdir/postgrurl', 'toString'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION getAuthority(postgrurl)
RETURNS cstring
AS '$libdir/postgrurl', 'getAuthority'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION getUserInfo(postgrurl)
RETURNS cstring
AS '$libdir/postgrurl', 'getUserInfo'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION getPath(postgrurl)
RETURNS cstring
AS '$libdir/postgrurl', 'getPath'
LANGUAGE C IMMUTABLE STRICT;

