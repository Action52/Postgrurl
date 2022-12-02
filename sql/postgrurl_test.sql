CREATE EXTENSION postgrurl;
CREATE TABLE testurl(id int, purl postgrurl);
INSERT INTO testurl(id, purl) VALUES(1, 'test');
INSERT INTO testurl(id, purl) VALUES(2, 'http://test.com');
INSERT INTO testurl(id, purl) VALUES(3, 'http://test.com/file.txt');
INSERT INTO testurl(id, purl) VALUES(4, 'http://test.com/random/path/');
INSERT INTO testurl(id, purl) VALUES(5, 'http://test.com:8162');
INSERT INTO testurl(id, purl) VALUES(6, 'http://test.com:8239/hola?query=6');


SELECT * FROM testurl;

SELECT URL('http://test.com/hola?query=6'::postgrurl, 'http://test.com/hola');
SELECT URL('http://test.com/hola?query=6'::postgrurl, '/halamadrid');
SELECT URL('http://test.com/hola/hahahaha'::postgrurl, 'hola');
SELECT URL('http://test.com/hola?query=6'::postgrurl, 'halamadrid');


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

SELECT purl, sameFile(purl, 'http://test.com/file.txt'::postgrurl), sameHost(purl, 'http://test.com/file.txt'::postgrurl) from testurl;

SELECT getFile('http://test.com/file.txt'::postgrurl);
SELECT getHost('http://test.com:8239/hola?query=6'::postgrurl);
SELECT getPort('http://test.com:8239/hola?query=6'::postgrurl);
SELECT getProtocol('http://test.com:8239/hola?query=6'::postgrurl);
SELECT getQuery('http://test.com:8239/hola?query=6'::postgrurl);

SELECT getRef('http://test.com:8239/hola#somewhere'::postgrurl);
SELECT getRef('http://test.com:8239/hola?query=6'::postgrurl);

SELECT sameFile('http://test.com/file.txt'::postgrurl, 'http://facebook.com/8239/hola/file.txt'::postgrurl);
SELECT sameFile('http://test.com/file1.txt'::postgrurl, 'http://facebook.com/8239/hola/file.txt'::postgrurl);

SELECT sameHost('http://facebook.com/8239/hola/file.txt'::postgrurl, 'http://acebook.com/8239/hola/file.txt'::postgrurl);
SELECT sameHost('http://facebook.com/8239/hola/file.txt'::postgrurl, 'https://facebook.com/80/holu/#somewhere'::postgrurl);

SELECT toString('http://test.com'::postgrurl);

DROP TABLE testurl;
DROP EXTENSION postgrurl;