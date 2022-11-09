CREATE EXTENSION postgrurl;
CREATE TABLE testurl(id int, purl postgrurl);
INSERT INTO testurl(id, purl) VALUES(1, 'http://test.com');
INSERT INTO testurl(id, purl) VALUES(2, 'facebook.com');
INSERT INTO testurl(id, purl) VALUES(3, 'https://facebook.com');
INSERT INTO testurl(id, purl) VALUES(4, 'https://facebook.com/about/');
INSERT INTO testurl(id, purl) VALUES(5, 'https://facebook.com?q=querytest&attr=1');


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

DROP TABLE testurl;