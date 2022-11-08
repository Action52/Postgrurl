EXTENSION   = postgrurl
MODULES     = postgrurl
DATA        = postgrurl--1.0.sql postgrurl.control
REGRESS		= postgrurl_test

LDFLAGS=-lrt

PG_CONFIG ?= pg_config
PGXS = $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)