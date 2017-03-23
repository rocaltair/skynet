#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>
#include <errno.h>
#include "lzf.h"

#define LIBNAME "lzf"
#define RETRY_MAX_CNT 10

static int lua_compress(lua_State *L)
{
	size_t size;
	unsigned int out_len;
	void *out_data;
	unsigned int clen;
	const char *in_data = luaL_checklstring(L, 1, &size);
	out_len = size * 1.04f + 1;
	out_data = malloc(sizeof(char) * out_len);
	if (out_data == NULL) {
		lua_pushnil(L);
		lua_pushfstring(L, "ENOMEM in %s", __FUNCTION__);
		return 0;
	}
	clen = lzf_compress(in_data, (unsigned int)size,
		     out_data, out_len);
	if (clen == 0) {
		free(out_data);
		lua_pushnil(L);
		lua_pushfstring(L, "compress failed in %s", __FUNCTION__);
		return 2;
	}
	lua_pushlstring(L, (const char *)out_data, clen);
	free(out_data);
	return 1;
}

static int lua_decompress(lua_State *L)
{
	const float ratio = 2.0f;
	size_t size;
	unsigned int out_len;
	void *out_data;
	unsigned int clen;
	const char *in_data = luaL_checklstring(L, 1, &size);
	int retrycnt = 0;
	out_len = size * 5.0f;
	do {
		out_len *= ratio;
		out_data = malloc(sizeof(char) * out_len);
		if (out_data == NULL) {
			lua_pushnil(L);
			lua_pushfstring(L, "ENOMEM in %s", __FUNCTION__);
			return 0;
		}
		clen = lzf_decompress(in_data, (unsigned int)size,
			     out_data, out_len);
		retrycnt++;
		if (clen == 0) {
			free(out_data);
			if (errno == E2BIG) {
				continue;
			}
			break;
		}
		lua_pushlstring(L, (const char *)out_data, clen);
		free(out_data);
		return 1;
	} while (1);
	lua_pushnil(L);
	lua_pushfstring(L, "ENOMEM in %s", __FUNCTION__);
	return 2;
}

int luaopen_lzf(lua_State *L)
{
	luaL_Reg lfuncs[] = {
		{"compress", lua_compress},
		{"decompress", lua_decompress},
		{NULL, NULL}
	};
	luaL_newlib(L, lfuncs);
	return 1;
}


