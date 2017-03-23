include ../../platform.mk

LUACLIB_DIR = ../../luaclib

LUACLIB_NAMES = cjson laoi ldump\
		lunix lfs proc bitstring crypto \
		lastar msgpack lzf liconv lkazmath misc #lsqlite3 

CFLAGS = -g -O2 -Wall -I$(LUA_INC) $(MYCFLAGS)
LUA_INC ?= ../lua

ALL_LIBS = $(foreach v, $(LUACLIB_NAMES), $(LUACLIB_DIR)/$(v).so) 

all: $(ALL_LIBS)

$(LUACLIB_DIR)/cjson.so : lcjson/lcjson.c lcjson/strbuf.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@	

$(LUACLIB_DIR)/crypto.so : crypto/lcrypto.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@	

$(LUACLIB_DIR)/laoi.so : aoi/aoi.c aoi/laoi.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@	

$(LUACLIB_DIR)/lunix.so : lunix.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@	

$(LUACLIB_DIR)/ldump.so : ldump.c
	$(CC) $(CFLAGS) -Wno-unused-function $(SHARED) $^ -o $@	

$(LUACLIB_DIR)/lfs.so : lfs.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@	

$(LUACLIB_DIR)/liconv.so : liconv.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@	

$(LUACLIB_DIR)/bitstring.so : bitstring/lbitstring.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@	

$(LUACLIB_DIR)/lsqlite3.so : sqlite/lsqlite3.c sqlite/sqlite3.c
	$(CC) $(CFLAGS) -Wno-unused-const-variable -Wno-deprecated-declarations $(SHARED) $^ -o $@	

$(LUACLIB_DIR)/proc.so : proc/lproc.c proc/proc.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@	

$(LUACLIB_DIR)/lastar.so : astar/astar.c astar/lastar.c
	$(CC) $(CFLAGS) -Wno-unused-function $(SHARED) $^ -o $@	

$(LUACLIB_DIR)/msgpack.so : lmsgpack.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@	

$(LUACLIB_DIR)/lzf.so : lzf/llzf.c lzf/lzf_c.c lzf/lzf_d.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@	

$(LUACLIB_DIR)/misc.so : misc/lmisc.c misc/algo.c misc/xxhash.c misc/bresenham.c misc/levenshtein.c
	$(CC) $(CFLAGS) -Wno-unused-function $(SHARED) $^ -o $@	

$(LUACLIB_DIR)/lkazmath.so : kazmath/lkazmath.c kazmath/kazmath/*.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@	

clean:
	@rm -f $(ALL_LIBS)
