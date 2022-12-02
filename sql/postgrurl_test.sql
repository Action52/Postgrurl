CREATE EXTENSION postgrurl;
CREATE TABLE testurl(id int, purl postgrurl);
INSERT INTO testurl(id, purl) VALUES(1, 'test');
INSERT INTO testurl(id, purl) VALUES(2, 'http://test.com');
INSERT INTO testurl(id, purl) VALUES(3, 'http://test.com/file.txt');
INSERT INTO testurl(id, purl) VALUES(4, 'http://test.com/random/path/');
INSERT INTO testurl(id, purl) VALUES(5, 'http://test.com:8162');
--INSERT INTO testurl(id, purl) VALUES(6, 'http://test.com:8239/hola?query=6');


SELECT * FROM testurl;

SELECT postgrurl_eq('12345'::postgrurl, '123456'::postgrurl);
SELECT postgrurl_eq('facebook.com'::postgrurl, 'facebook.com'::postgrurl);

SELECT postgrurl_gt('facebook.com'::postgrurl, 'facebook.com'::postgrurl);
SELECT postgrurl_gt('facebook.com'::postgrurl, 'eacebook.com'::postgrurl);

SELECT postgrurl_gte('facebook.com'::postgrurl, 'facebook.com'::postgrurl);
SELECT postgrurl_gte('facebook.com'::postgrurl, 'eacebook.com'::postgrurl);
SELECT postgrurl_gte('eacebook.com'::postgrurl, 'facebook.com'::postgrurl);

SELECT postgrurl_lt('facebook.com'::postgrurl, 'facebook.com'::postgrurl);
SELECT postgrurl_lt('facebook.com'::postgrurl, 'eacebook.com'::postgrurl);

SELECT postgrurl_lte('facebook.com'::postgrurl, 'facebook.com'::postgrurl);
SELECT postgrurl_lte('facebook.com'::postgrurl, 'eacebook.com'::postgrurl);
SELECT postgrurl_lte('eacebook.com'::postgrurl, 'facebook.com'::postgrurl);

SELECT 'eacebook.com'::postgrurl = 'facebook.com'::postgrurl;
SELECT 'eacebook.com'::postgrurl <> 'facebook.com'::postgrurl;

SELECT 'eacebook.com'::postgrurl < 'facebook.com'::postgrurl;
SELECT 'eacebook.com'::postgrurl > 'facebook.com'::postgrurl;

SELECT 'eacebook.com'::postgrurl <= 'facebook.com'::postgrurl;
SELECT 'eacebook.com'::postgrurl >= 'facebook.com'::postgrurl;


SELECT purl, sameHost(purl, 'test.com/file.txt'), sameFile(purl, 'test.com/file.txt') FROM testurl;

CREATE INDEX postgrurl_idx ON testurl USING BTREE(purl);

EXPLAIN SELECT * FROM testurl ORDER BY purl;

DROP INDEX IF EXISTS postgrurl_idx;
DROP TABLE testurl;