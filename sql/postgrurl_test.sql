CREATE EXTENSION postgrurl;
CREATE TABLE testurl(id int, purl postgrurl);

SELECT URL('http', 'test.com', '/about/file.txt');
SELECT URL('http', 'test.com', 10, '/about/file.txt');

INSERT INTO testurl(id, purl) VALUES(1, 'test');
INSERT INTO testurl(id, purl) VALUES(2, 'http://test.com');
INSERT INTO testurl(id, purl) VALUES(3, 'http://test.com/file.txt');
INSERT INTO testurl(id, purl) VALUES(4, 'http://test.com/random/path/');
INSERT INTO testurl(id, purl) VALUES(5, 'http://test.com:8162');
INSERT INTO testurl(id, purl) VALUES(6, URL('http', 'test.com', 10, 'about/file.txt'));
INSERT INTO testurl(id, purl) VALUES(7, URL('http', 'test.com', 'about/file.txt'));
--INSERT INTO testurl(id, purl) VALUES(6, 'http://test.com:8239/hola?query=6');


SELECT * FROM testurl;

--SELECT URL('http://test.com/hola?query=6'::postgrurl, 'http://test.com/hola');
--SELECT URL('http://test.com/hola?query=6'::postgrurl, '/halamadrid');
--SELECT URL('http://test.com/hola/hahahaha'::postgrurl, 'hola');
--SELECT URL('http://test.com/hola?query=6'::postgrurl, 'halamadrid');


SELECT _postgrurl_eq('12345'::postgrurl, '123456'::postgrurl);
SELECT _postgrurl_eq('facebook.com'::postgrurl, 'facebook.com'::postgrurl);

SELECT _postgrurl_gt('facebook.com'::postgrurl, 'facebook.com'::postgrurl);
SELECT _postgrurl_gt('facebook.com'::postgrurl, 'eacebook.com'::postgrurl);

SELECT _postgrurl_gte('facebook.com'::postgrurl, 'facebook.com'::postgrurl);
SELECT _postgrurl_gte('facebook.com'::postgrurl, 'eacebook.com'::postgrurl);
SELECT _postgrurl_gte('eacebook.com'::postgrurl, 'facebook.com'::postgrurl);

SELECT _postgrurl_lt('facebook.com'::postgrurl, 'facebook.com'::postgrurl);
SELECT _postgrurl_lt('facebook.com'::postgrurl, 'eacebook.com'::postgrurl);

SELECT _postgrurl_lte('facebook.com'::postgrurl, 'facebook.com'::postgrurl);
SELECT _postgrurl_lte('facebook.com'::postgrurl, 'eacebook.com'::postgrurl);
SELECT _postgrurl_lte('eacebook.com'::postgrurl, 'facebook.com'::postgrurl);

SELECT 'eacebook.com'::postgrurl = 'facebook.com'::postgrurl;
SELECT 'eacebook.com'::postgrurl <> 'facebook.com'::postgrurl;

SELECT 'eacebook.com'::postgrurl < 'facebook.com'::postgrurl;
SELECT 'eacebook.com'::postgrurl > 'facebook.com'::postgrurl;

SELECT 'eacebook.com'::postgrurl <= 'facebook.com'::postgrurl;
SELECT 'eacebook.com'::postgrurl >= 'facebook.com'::postgrurl;


SELECT purl, sameHost(purl, 'test.com/file.txt'), sameFile(purl, 'test.com/file.txt') FROM testurl;


CREATE INDEX postgrurl_idx ON testurl(purl);
SET enable_seqscan TO off;

EXPLAIN ANALYSE SELECT purl FROM testurl WHERE sameFile(purl, 'http://test.com/file.txt'::postgrurl);
SELECT purl FROM testurl WHERE sameFile(purl, 'http://test.com/file.txt'::postgrurl);

EXPLAIN ANALYSE SELECT sameHost(purl, 'test.com/file.txt'::postgrurl) FROM testurl;
SELECT sameHost(purl, 'test.com/file.txt'::postgrurl) FROM testurl;


EXPLAIN ANALYSE SELECT * FROM testurl WHERE purl = URL('http', 'test.com', 'about/file.txt');
SELECT * FROM testurl WHERE purl = URL('http', 'test.com', 'about/file.txt');

EXPLAIN ANALYSE SELECT * FROM testurl WHERE purl >= 'http://test.com'::postgrurl;
SELECT * FROM testurl WHERE purl >= 'http://test.com'::postgrurl AND purl <= 'https://test.com/random/path/about/';


EXPLAIN ANALYSE SELECT sameFile('test.com/about/file.txt', 'test.com/file.txt');
SELECT sameFile('test.com/about/file.txt', 'test.com/file.txt');

SET enable_seqscan TO on;

DROP INDEX IF EXISTS postgrurl_idx;
DROP TABLE testurl;
