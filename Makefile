EXTENSION   = postgrurl
MODULES     = postgrurl
DATA        = postgrurl--1.0.sql postgrurl.control

LDFLAGS=-lrt

PG_CONFIG ?= pg_config
PGXS = $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)