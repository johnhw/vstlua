#ifdef _MSC_VER
  #if (_MSC_VER < 1600)
    #ifndef __uint32_t_defined
      typedef unsigned long uint32_t;
      #define __uint32_t_defined
    #endif
  #else
    // Visual Studio 2010 finally has it.
    #include <stdint.h>
  #endif
#else
  // Loonix. Presumably gcc, which has it.
  #include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
int hashLua(lua_State *state);
void hashLuaRecurse(lua_State *state, int depth, int ref, uint32_t *a,uint32_t *b);
double int64todouble(uint32_t *a, uint32_t *b);
void doubletoint64(double d, uint32_t *a, uint32_t *b);

#ifdef __cplusplus
}
#endif
