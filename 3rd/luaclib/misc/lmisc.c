#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <lua.h>
#include <lauxlib.h>
#include <sys/time.h>
#include <stdint.h>
#include <assert.h>
#include "bresenham.h"

#if LUA_VERSION_NUM < 502 && (!defined(luaL_newlib))
#  define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))
#endif

#include "levenshtein.h"


#include "xxhash.h"

#define INVALID_TICK 0

/**
 * #define ENABLE_EXCEED_TIME_CHECK 1
 */

#define XPCALL_STACK_MAX_SZ 256
#define MAX_EVAL 200000000
#define TOO_MUCH_TIME (uint64_t)(0.5 * 1e9)
static uint64_t tooMuchTime = TOO_MUCH_TIME;
static int maxEvalCount = MAX_EVAL;

static int xpcallStackIndex = 0;
static uint64_t xpcallStack[XPCALL_STACK_MAX_SZ] = {0};

static uint64_t gettick();

static uint64_t getLastTick(int index)
{
	assert(index > 0);
	if (index > XPCALL_STACK_MAX_SZ)
		index = XPCALL_STACK_MAX_SZ;
	return xpcallStack[index - 1];
}

static void clearLastTick(int index)
{
	assert(index > 0);
	if (index > XPCALL_STACK_MAX_SZ)
		index = XPCALL_STACK_MAX_SZ;
	xpcallStack[index - 1] = INVALID_TICK;
}

static void enterFunc()
{
	xpcallStack[xpcallStackIndex] = gettick();
	xpcallStackIndex++;
}

static void exitFunc()
{
	xpcallStackIndex--;
}

struct bh_udata {
        lua_State *L; 
        int idx;
        int ref;
        size_t max;
};

static void init_bh_udata(struct bh_udata *udata,
                          lua_State *L, 
                          int max, int ref)
{
        udata->L = L;
        udata->max = max;
        udata->ref = ref;
        udata->idx = -1; 
}

static int pushpos2lua(void *data, int x, int y)
{
        struct bh_udata *udata = (struct bh_udata *)data;
        lua_State *L = udata->L;
        if (udata->max > 0 && udata->idx + 1 > udata->max) {
                return BH_STOP;
        }   
        if (udata->ref != LUA_NOREF) {
                int top = lua_gettop(L);
                lua_rawgeti(L, LUA_REGISTRYINDEX, udata->ref);
                lua_pushnumber(L, x);
                lua_pushnumber(L, y);
                if (lua_pcall(L, 2, 1, 0) == 0) {
                        if (!lua_toboolean(L, -1)) {
                                lua_settop(L, top);
                                return BH_STOP;
                        }
                } else {
                        return BH_STOP;
                }
                lua_settop(L, top);
        }
        udata->idx++;

        lua_newtable(L);
        lua_pushinteger(L, (lua_Integer)x);
        lua_rawseti(L, -2, 1);
        lua_pushinteger(L, (lua_Integer)y);
        lua_rawseti(L, -2, 2);
        lua_rawseti(L, -2, udata->idx + 1);

        return BH_CONTINUE;
}

static int lua__bresenham(lua_State *L)
{
        int ret;
        struct bh_udata udata;
        int ref = LUA_NOREF;
        int sx = luaL_checkinteger(L, 1);
        int sy = luaL_checkinteger(L, 2);
        int ex = luaL_checkinteger(L, 3);
        int ey = luaL_checkinteger(L, 4);
        int max = luaL_optinteger(L, 5, -1);
        if (lua_type(L, 6) == LUA_TFUNCTION) {
                ref = luaL_ref(L, LUA_REGISTRYINDEX);
        }
        init_bh_udata(&udata, L, max, ref);
        lua_newtable(L);
        ret = bresenham_line(sx, sy, ex, ey, pushpos2lua, &udata);
        lua_pushboolean(L, ret == 0);
        if (ref != LUA_NOREF) {
                luaL_unref(L, LUA_REGISTRYINDEX, ref);
        }
        return 2;
}



static inline void itimeofday(long *sec, long *usec)
{
#if defined(WIN32) || defined(_WIN32)
# define IINT64 __int64;
	static long mode = 0, addsec = 0;
	BOOL retval;
	static IINT64 freq = 1;
	IINT64 qpc;
	if (mode == 0) {
		retval = QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		freq = (freq == 0)? 1 : freq;
		retval = QueryPerformanceCounter((LARGE_INTEGER*)&qpc);
		addsec = (long)time(NULL);
		addsec = addsec - (long)((qpc / freq) & 0x7fffffff);
		mode = 1;
	}   
	retval = QueryPerformanceCounter((LARGE_INTEGER*)&qpc);
	retval = retval * 2;
	if (sec) *sec = (long)(qpc / freq) + addsec;
	if (usec) *usec = (long)((qpc % freq) * 1000000 / freq);
#else
	struct timeval time;
	gettimeofday(&time, NULL);
	if (sec) *sec = time.tv_sec;
	if (usec) *usec = time.tv_usec;
#endif
}

/**
 * levenshtein distance
 */
static int lua__levenshtein_dis(lua_State *L)
{
	unsigned int ret;
	int case_sen = 1;
	int top = lua_gettop(L);
	const char *src = luaL_checkstring(L, 1);
	const char *dst = luaL_checkstring(L, 2);
	if (top > 2) {
		case_sen = !lua_toboolean(L, 3);
	}
	ret = levenshtein(src, dst, case_sen);
	lua_pushnumber(L, (lua_Number)ret);
	return 0;
}

static int lua__gettimeofday(lua_State *L)
{
	long sec;
	long usec;
	itimeofday(&sec, &usec);
	lua_pushnumber(L, (lua_Number)sec);
	lua_pushnumber(L, (lua_Number)usec);
	return 2;
}

static int lua__printEx(lua_State *L, FILE *file, const char *spliter)
{
	int n = lua_gettop(L);  /* number of arguments */
	int i;
	lua_getglobal(L, "tostring");
	for (i=1; i<=n; i++) {
		const char *s; 
		lua_pushvalue(L, -1);  /* function to be called */
		lua_pushvalue(L, i);   /* value to print */
		lua_call(L, 1, 1); 
		s = lua_tostring(L, -1);  /* get result */
		if (s == NULL)
			return luaL_error(L, "tostring failed in lua__print");
		if (i > 1) fputs(spliter, file);
		fputs(s, file);
		lua_pop(L, 1);  /* pop result */
	}
	fputs("\n", file);
	fflush(file);
	lua_settop(L, n);
	return 0;
}

static int lua__print(lua_State *L)
{
	return lua__printEx(L, stdout, "\t");
}

static int lua__xxhash(lua_State *L)
{
	size_t size = 0;
	unsigned int seed = 0;
	int argCnt =  lua_gettop(L);
	const void* s = luaL_checklstring(L, 1, &size);
	unsigned int hashv;

	if (argCnt == 2) {
		seed = (unsigned int)luaL_checkinteger(L, 2); 
	}   

	hashv = XXH32(s, (int)size, seed);
	lua_pushinteger(L, (lua_Integer)hashv);
	return 1;
}

static uint64_t gettick()
{ 
	uint32_t low; 
	uint32_t high; 
	uint64_t tick; 

	asm volatile ( 
			"rdtsc" 
			: "=a"(low), "=d"(high) /* output */ 
			/* : input */ 
			/* : list of clobbered registers */ 
	);  
	tick = high; 
	tick = (tick<<32)+low; 
	return tick; 
}

static int isInvokeExceedTime()
{
	uint64_t now;
	uint64_t last_tick;
	if (xpcallStackIndex <= 0)
		return 0;
       	last_tick = getLastTick(xpcallStackIndex);
	if (last_tick == INVALID_TICK)
		return 0;
	now = gettick();
	if (now - last_tick > tooMuchTime) {
		return 1;
	}
	return 0;
}

static int hook_check_enter(lua_State *L, int event)
{
        if (event == LUA_HOOKCOUNT) {
		return 1;
	}    

	if (event == LUA_HOOKRET) {
		return isInvokeExceedTime();
	}    

	return 0;
}

static void handle_time_limit(lua_State *L)
{
	int top = lua_gettop(L);
	lua_getglobal(L, "EInvokeTimeLimit");
	if (lua_isfunction(L, -1)) {
		uint64_t now = gettick();
		uint64_t last_tick = getLastTick(xpcallStackIndex);
		double diff = (double)now - (double)last_tick;
		lua_pushnumber(L, diff / 1e6);
		lua_pcall(L, 1, 0, 0);
	}
	lua_settop(L, top);
}

static void handle_eval_limit(lua_State *L)
{
	int top = lua_gettop(L);
	lua_getglobal(L, "EInvokeEvalLimit");
	if (lua_isfunction(L, -1)) {
		lua_pushnumber(L, maxEvalCount);
		lua_pcall(L, 1, 0, 0);
	}
	lua_settop(L, top);
}

static void invoke_hook(lua_State *L, lua_Debug *ar) 
{
	if (ar->event == LUA_HOOKCOUNT) {
		handle_eval_limit(L);
		clearLastTick(xpcallStackIndex);
		// exitFunc();
		luaL_error(L, "EInvokeEvalLimit");
		return;
	}    
	if(ar->event == LUA_HOOKRET) {
		if (!isInvokeExceedTime())
			return;
		handle_time_limit(L);
		clearLastTick(xpcallStackIndex);
		return;
	}    
}

static int lua_setInvokeLimit(lua_State *L)
{
	lua_Integer exceedTime = luaL_optinteger(L, 1, TOO_MUCH_TIME / 1e6) * 1e6;
	int evalLimit = luaL_optinteger(L, 2, MAX_EVAL);
	maxEvalCount = evalLimit;
	tooMuchTime = exceedTime;
	return 0;
}

static int lua__xpcall (lua_State *L)
{
	int status;
#ifdef ENABLE_EXCEED_TIME_CHECK
	int mask = LUA_MASKCOUNT|LUA_MASKRET;
#endif
	luaL_checktype(L, 1, LUA_TFUNCTION);
	luaL_checktype(L, 2, LUA_TFUNCTION);
#ifdef ENABLE_EXCEED_TIME_CHECK
	enterFunc();
	lua_sethook_adv(L, invoke_hook, hook_check_enter, mask, maxEvalCount);
#endif

	/* lua_settop(L, 2); */
	lua_pushvalue(L, 2); /*f, err, ..., err*/
	lua_insert(L, 1);  /* err, f, err, ...; put error function under function to be called */
	lua_remove(L, 3); /* err, f, ...*/
	status = lua_pcall(L, lua_gettop(L) - 2, LUA_MULTRET, 1); 
	lua_pushboolean(L, (status == 0));
	lua_replace(L, 1); 

#ifdef ENABLE_EXCEED_TIME_CHECK
	exitFunc();
#endif
	return lua_gettop(L);  /* return status + all results */
}

static unsigned short checksum(const char *str, int count)
{
	/**
	 * Compute Internet Checksum for "count" bytes
	 * beginning at location "addr".
	 */
	register long sum = 0;
	char *addr = (char *)str;

	while( count > 1 )  {
		/*  This is the inner loop */
		sum += * (unsigned short *) addr++;
		count -= 2;
	}

	/*  Add left-over byte, if any */
	if( count > 0 )
		sum += * (unsigned char *) addr;

	/*  Fold 32-bit sum to 16 bits */
	while (sum>>16)
		sum = (sum & 0xffff) + (sum >> 16);

	return ~sum;
}

static int lua__checksum(lua_State *L)
{
	size_t sz;
	const char *str = luaL_checklstring(L, 1, &sz);
	unsigned short csum = checksum(str, sz);
	lua_pushinteger(L, (lua_Integer)csum);
	return 1;
}

int luaopen_misc(lua_State* L)
{
	luaL_Reg lfuncs[] = {
		{"xxhash", lua__xxhash},
		{"checksum", lua__checksum},
		{"levenshtein", lua__levenshtein_dis},
		{"gettimeofday", lua__gettimeofday},
		{"bresenham", lua__bresenham},
		{NULL, NULL},
	};
	lua_pushcfunction(L, lua__print);
	lua_setglobal(L, "print");
	/*
	lua_pushcfunction(L, lua__xpcall);
	lua_setglobal(L, "xpcall");
	lua_getglobal(L, "debug");
	lua_pushcfunction(L, lua_setInvokeLimit);
	lua_setfield(L, -2, "setInvokeLimit");
	*/
	lua_pop(L, 1);
	luaL_newlib(L, lfuncs);
	return 1;
}
