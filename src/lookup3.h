#ifdef __cplusplus
extern "C" {
#endif

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

uint32_t hashword(
const uint32_t *k,                   /* the key, an array of uint32_t values */
size_t          length,               /* the length of the key, in uint32_ts */
uint32_t        initval);         /* the previous hash, or an arbitrary value */


void hashword2 (
const uint32_t *k,                   /* the key, an array of uint32_t values */
size_t          length,               /* the length of the key, in uint32_ts */
uint32_t       *pc,                      /* IN: seed OUT: primary hash value */
uint32_t       *pb);               /* IN: more seed OUT: secondary hash value */



uint32_t hashlittle( const void *key, size_t length, uint32_t initval);


void hashlittle2( 
  const void *key,       /* the key to hash */
  size_t      length,    /* length of the key */
  uint32_t   *pc,        /* IN: primary initval, OUT: primary hash */
  uint32_t   *pb);        /* IN: secondary initval, OUT: secondary hash */
  
uint32_t hashbig( const void *key, size_t length, uint32_t initval);

#ifdef __cplusplus
}
#endif

