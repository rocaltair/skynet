include ../../platform.mk

LUACLIB_DIR = ../../luaclib

LUACLIB_NAMES = ldump \
		cjson laoi \
		lunix lfs proc bitstring lcrypto \
		lhttpparser \
		lastar msgpack lzf lkazmath misc iconv #lsqlite3

CFLAGS = -g -O2 -Wall -I$(LUA_INC) $(MYCFLAGS)
LUA_INC ?= ../lua

ifeq ($(PLAT), linux)
LDFLAGS = -Wl,-R/usr/local/lib
else
LDFLAGS = -L/usr/local/lib
endif

ALL_LIBS = $(foreach v, $(LUACLIB_NAMES), $(LUACLIB_DIR)/$(v).so) 

all: $(ALL_LIBS)

$(LUACLIB_DIR)/cjson.so : lcjson/lcjson.c lcjson/strbuf.c | $(LUACLIB_DIR)
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

$(LUACLIB_DIR)/lhttpparser.so : http-parser/lua-http-parser.c http-parser/http-parser/http_parser.c | $(LUACLIB_DIR)
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

$(LUACLIB_DIR)/lcrypto.so : crypto/lcrypto.c | $(LUACLIB_DIR)
	$(CC) $(CFLAGS) $(SHARED) $(LDFLAGS) -lcrypto $^ -o $@

$(LUACLIB_DIR)/laoi.so : aoi/aoi.c aoi/laoi.c | $(LUACLIB_DIR)
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

$(LUACLIB_DIR)/lunix.so : lunix.c| $(LUACLIB_DIR)
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

$(LUACLIB_DIR)/ldump.so : ldump.c| $(LUACLIB_DIR)
	$(CC) $(CFLAGS) -Wno-unused-function $(SHARED) $^ -o $@

$(LUACLIB_DIR)/lfs.so : lfs.c| $(LUACLIB_DIR)
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

$(LUACLIB_DIR)/iconv.so : liconv.c |  $(LUACLIB_DIR)
	$(CC) $(CFLAGS) $(SHARED) $(LDFLAGS) -liconv $^ -o $@

$(LUACLIB_DIR)/bitstring.so : bitstring/lbitstring.c | $(LUACLIB_DIR)
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

$(LUACLIB_DIR)/lsqlite3.so : sqlite/lsqlite3.c sqlite/sqlite3.c| $(LUACLIB_DIR)
	$(CC) $(CFLAGS) -Wno-unused-const-variable -Wno-deprecated-declarations $(SHARED) $^ -o $@

$(LUACLIB_DIR)/proc.so : proc/lproc.c proc/proc.c| $(LUACLIB_DIR)
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

$(LUACLIB_DIR)/lastar.so : astar/astar.c astar/lastar.c| $(LUACLIB_DIR)
	$(CC) $(CFLAGS) -Wno-unused-function $(SHARED) $^ -o $@

$(LUACLIB_DIR)/msgpack.so : lmsgpack.c| $(LUACLIB_DIR)
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

$(LUACLIB_DIR)/lzf.so : lzf/llzf.c lzf/lzf_c.c lzf/lzf_d.c| $(LUACLIB_DIR)
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

$(LUACLIB_DIR)/misc.so : misc/lmisc.c misc/algo.c misc/xxhash.c misc/bresenham.c misc/levenshtein.c | $(LUACLIB_DIR)
	$(CC) $(CFLAGS) -Wno-unused-function $(SHARED) $^ -o $@

$(LUACLIB_DIR)/lkazmath.so : kazmath/lkazmath.c kazmath/kazmath/*.c| $(LUACLIB_DIR)
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

$(LUACLIB_DIR):
	mkdir -p $@

clean:
	@rm -f $(ALL_LIBS)
