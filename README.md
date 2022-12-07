# Postgrurl
Repository for the final project for the Databases Architecture course of the ULB. The aim of the project is to create a Postgresql extension to handle URL data, emulating Java's .net.URL class and implementing those functionalities into postgres.

* * *

## How to install

Download the repository and run the following command on a terminal inside said folder:

```bash
git clone https://github.com/Action52/Postgrurl
cd Postgrurl
make clean && make && make install
```

This will compile the code inside the repository and generate the appropiate executables.

Now log into your postgres installation and run the command

```sql
CREATE EXTENSION postgrurl;
```

This will create the extension on the database of your choice. Now you can use the postgrurl data type as desired.

## Usage

Once you have implemented the extension, you will have access to the data type, constructors, functions, operators optimized for URL handling.
We can then also create a btree index for this data type.

Some examples for implementation:

```sql
-- Create a table with an ID and postgrurl data type, using some of the available constructors and the default in function 
-- which will try to parse a string and change it into a postgrurl data type.
CREATE TABLE testurl(id int, purl postgrurl);
INSERT INTO testurl(id, purl) VALUES(1, 'test');
INSERT INTO testurl(id, purl) VALUES(2, 'http://test.com');
INSERT INTO testurl(id, purl) VALUES(3, 'http://test.com/file.txt');
INSERT INTO testurl(id, purl) VALUES(4, 'http://test.com/random/path/');
INSERT INTO testurl(id, purl) VALUES(5, 'http://test.com:8162');
INSERT INTO testurl(id, purl) VALUES(6, URL('http', 'test.com', 10, 'about/file.txt'));
INSERT INTO testurl(id, purl) VALUES(7, URL('http', 'test.com', 'about/file.txt'));
INSERT INTO testurl(id, purl) VALUES(8, URL('http', 'facebook.com', 'about/file.txt'));
INSERT INTO testurl(id, purl) VALUES(9, URL('http', 'facebook.com', 'profile'));
INSERT INTO testurl(id, purl) VALUES(10, 'https://facebook.com:8888/random/path');
INSERT INTO testurl(id, purl) VALUES(11, URL('http://facebook.com'::postgrurl, 'newsubpath');
```

To test the index usage, let's turn seq scan off for a while to enforce postgres query planner to pick the index.
We have defined 3 functions to be index supported: equals(postgrurl, postgrurl), sameHost(postgrurl, postgrurl) and sameFile(postgrurl, postgrurl).

```sql
-- Create a table with an ID and postgrurl data type, using some of the available constructors and the default in function 
-- which will try to parse a string and change it into a postgrurl data type.
CREATE INDEX postgrurl_idx ON testurl(purl);
SET enable_seqscan TO off;

EXPLAIN ANALYSE SELECT purl FROM testurl WHERE sameFile(purl, 'http://test.com/file.txt'::postgrurl);
SELECT purl FROM testurl WHERE sameFile(purl, 'http://test.com/file.txt'::postgrurl);

EXPLAIN ANALYSE SELECT purl FROM testurl WHERE sameHost(purl, 'http://test.com/file.txt'::postgrurl);
SELECT purl FROM testurl WHERE sameHost(purl, 'http://test.com/file.txt'::postgrurl);

EXPLAIN ANALYSE SELECT purl FROM testurl WHERE equals(purl, 'http://test.com/file.txt'::postgrurl);
SELECT purl FROM testurl WHERE equals(purl, 'http://test.com/file.txt'::postgrurl);

EXPLAIN ANALYSE SELECT * FROM testurl WHERE purl = URL('http', 'test.com', 'about/file.txt');
SELECT * FROM testurl WHERE purl = URL('http', 'test.com', 'about/file.txt');

EXPLAIN ANALYSE SELECT * FROM testurl WHERE purl >= 'http://test.com'::postgrurl AND purl <= 'https://test.com/random/path/about/'::postgrurl;
SELECT * FROM testurl WHERE purl >= 'http://test.com'::postgrurl AND purl <= 'https://test.com/random/path/about/'::postgrurl;

EXPLAIN ANALYSE SELECT purl FROM testurl WHERE sameHost(purl, 'http://test.com/file.txt'::postgrurl) OR sameFile(purl, 'http://facebook.com/file.txt'::postgrurl);
SELECT purl FROM testurl WHERE sameHost(purl, 'http://test.com/file.txt'::postgrurl) OR sameFile(purl, 'http://facebook.com/file.txt'::postgrurl);

SET enable_seqscan TO on;
```

We can also use the `COPY` command to insert in bulk, for example:

```sql
COPY testurl TO '/tmp/testurl.in' WITH BINARY;
DELETE FROM testurl;
COPY testurl FROM '/tmp/testurl.in' WITH BINARY;
SELECT * FROM testurl;
```

Finally delete the test data if necessary.

```sql
DROP INDEX IF EXISTS postgrurl_idx;
DROP TABLE testurl;
DROP EXTENSION postgrurl;
```

You can find more usage examples on the test file included in the repository.