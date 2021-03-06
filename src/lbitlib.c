/* Bitwise operations library */
/* Reuben Thomas   nov00-08dec06 */

#include "lauxlib.h"
#include "lua.h"

// MS C/C++ has no "inttypes.h", but in VS2010 it has stdint.h
#ifdef _MSC_VER
  #if (_MSC_VER < 1600)
    #ifndef __intmax_t_defined
      typedef long long intmax_t;
      typedef unsigned long long uintmax_t;
      #define __intmax_t_defined
    #endif
  #else
    // Visual Studio 2010 finally has it.
    #include <stdint.h>
  #endif
#else
  // Loonix. Presumably gcc, which has it.
  #include <inttypes.h>
#endif

typedef uintmax_t UInteger;
typedef intmax_t Integer;

#define TDYADIC(name, op, type1, type2) \
  static int bit_ ## name(lua_State* L) { \
    lua_pushnumber(L, \
      (type1)luaL_checknumber(L, 1) op (type2)luaL_checknumber(L, 2)); \
    return 1; \
  }

#define MONADIC(name, op, type) \
  static int bit_ ## name(lua_State* L) { \
    lua_pushnumber(L, op (type)luaL_checknumber(L, 1)); \
    return 1; \
  }

#define VARIADIC(name, op, type) \
  static int bit_ ## name(lua_State *L) { \
    int n = lua_gettop(L), i; \
    Integer w = (type)luaL_checknumber(L, 1); \
    for (i = 2; i <= n; i++) \
      w op (type)luaL_checknumber(L, i); \
    lua_pushnumber(L, w); \
    return 1; \
  }

MONADIC(bnot,    ~,  Integer)
VARIADIC(band,   &=, Integer)
VARIADIC(bor,    |=, Integer)
VARIADIC(bxor,   ^=, Integer)
TDYADIC(lshift,  <<, Integer, UInteger)
TDYADIC(rshift,  >>, UInteger, UInteger)
TDYADIC(arshift, >>, Integer, UInteger)
TDYADIC(mod,     %,  Integer, Integer)

static const struct luaL_reg bitlib[] = {
  {"bnot",    bit_bnot},
  {"band",    bit_band},
  {"bor",     bit_bor},
  {"bxor",    bit_bxor},
  {"lshift",  bit_lshift},
  {"rshift",  bit_rshift},
  {"arshift", bit_arshift},
  {"mod",     bit_mod},
  {NULL, NULL}
};

LUALIB_API int luaopen_bit (lua_State *L) {
  luaL_openlib(L, "bit", bitlib, 0);
  return 1;
}
