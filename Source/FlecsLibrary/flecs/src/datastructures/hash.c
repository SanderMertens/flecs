// This is free and unencumbered software released into the public domain under The Unlicense (http://unlicense.org/)
// main repo: https://github.com/wangyi-fudan/wyhash
// author: 王一 Wang Yi
// contributors: Reini Urban, Dietrich Epp, Joshua Haberman, Tommy Ettinger, 
//               Daniel Lemire, Otmar Ertl, cocowalla, leo-yuriev, 
//               Diego Barrios Romero, paulie-g, dumblob, Yann Collet, ivte-ms, 
//               hyb, James Z.M. Gao, easyaspi314 (Devin), TheOneric

/* quick example:
   string s="fjsakfdsjkf";
   uint64_t hash=wyhash(s.c_str(), s.size(), 0, wyp_);
*/

#include "../private_api.h"

#ifndef WYHASH_CONDOM
//protections that produce different results:
//1: normal valid behavior
//2: extra protection against entropy loss (probability=2^-63), aka. "blind multiplication"
#define WYHASH_CONDOM 1
#endif

#ifndef WYHASH_32BIT_MUM
//0: normal version, slow on 32 bit systems
//1: faster on 32 bit systems but produces different results, incompatible with wy2u0k function
#define WYHASH_32BIT_MUM 0  
#endif

//includes
#include <stdint.h>
#include <string.h>
#if defined(_MSC_VER) && defined(_M_X64)
  #include <intrin.h>
  #pragma intrinsic(_umul128)
#endif

//likely and unlikely macros
#if defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__clang__)
  #define likely_(x)  __builtin_expect(x,1)
  #define unlikely_(x)  __builtin_expect(x,0)
#else
  #define likely_(x) (x)
  #define unlikely_(x) (x)
#endif

//128bit multiply function
static inline void wymum_(uint64_t *A, uint64_t *B){
#if(WYHASH_32BIT_MUM)
  uint64_t hh=(*A>>32)*(*B>>32), hl=(*A>>32)*(uint32_t)*B, lh=(uint32_t)*A*(*B>>32), ll=(uint64_t)(uint32_t)*A*(uint32_t)*B;
  #if(WYHASH_CONDOM>1)
  *A^=_wyrot(hl)^hh; *B^=_wyrot(lh)^ll;
  #else
  *A=_wyrot(hl)^hh; *B=_wyrot(lh)^ll;
  #endif
#elif defined(__SIZEOF_INT128__)
  __uint128_t r=*A; r*=*B; 
  #if(WYHASH_CONDOM>1)
  *A^=(uint64_t)r; *B^=(uint64_t)(r>>64);
  #else
  *A=(uint64_t)r; *B=(uint64_t)(r>>64);
  #endif
#elif defined(_MSC_VER) && defined(_M_X64)
  #if(WYHASH_CONDOM>1)
  uint64_t  a,  b;
  a=_umul128(*A,*B,&b);
  *A^=a;  *B^=b;
  #else
  *A=_umul128(*A,*B,B);
  #endif
#else
  uint64_t ha=*A>>32, hb=*B>>32, la=(uint32_t)*A, lb=(uint32_t)*B, hi, lo;
  uint64_t rh=ha*hb, rm0=ha*lb, rm1=hb*la, rl=la*lb, t=rl+(rm0<<32), c=t<rl;
  lo=t+(rm1<<32); c+=lo<t; hi=rh+(rm0>>32)+(rm1>>32)+c;
  #if(WYHASH_CONDOM>1)
  *A^=lo;  *B^=hi;
  #else
  *A=lo;  *B=hi;
  #endif
#endif
}

//multiply and xor mix function, aka MUM
static inline uint64_t wymix_(uint64_t A, uint64_t B){ wymum_(&A,&B); return A^B; }

//endian macros
#ifndef WYHASH_LITTLE_ENDIAN
  #if defined(_WIN32) || defined(__LITTLE_ENDIAN__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    #define WYHASH_LITTLE_ENDIAN 1
  #elif defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    #define WYHASH_LITTLE_ENDIAN 0
  #else
    #warning could not determine endianness! Falling back to little endian.
    #define WYHASH_LITTLE_ENDIAN 1
  #endif
#endif

//read functions
#if (WYHASH_LITTLE_ENDIAN)
static inline uint64_t wyr8_(const uint8_t *p) { uint64_t v; memcpy(&v, p, 8); return v;}
static inline uint64_t wyr4_(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return v;}
#elif defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__clang__)
static inline uint64_t wyr8_(const uint8_t *p) { uint64_t v; memcpy(&v, p, 8); return __builtin_bswap64(v);}
static inline uint64_t wyr4_(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return __builtin_bswap32(v);}
#elif defined(_MSC_VER)
static inline uint64_t wyr8_(const uint8_t *p) { uint64_t v; memcpy(&v, p, 8); return _byteswap_uint64(v);}
static inline uint64_t wyr4_(const uint8_t *p) { uint32_t v; memcpy(&v, p, 4); return _byteswap_ulong(v);}
#else
static inline uint64_t wyr8_(const uint8_t *p) {
  uint64_t v; memcpy(&v, p, 8);
  return (((v >> 56) & 0xff)| ((v >> 40) & 0xff00)| ((v >> 24) & 0xff0000)| ((v >>  8) & 0xff000000)| ((v <<  8) & 0xff00000000)| ((v << 24) & 0xff0000000000)| ((v << 40) & 0xff000000000000)| ((v << 56) & 0xff00000000000000));
}
static inline uint64_t wyr4_(const uint8_t *p) {
  uint32_t v; memcpy(&v, p, 4);
  return (((v >> 24) & 0xff)| ((v >>  8) & 0xff00)| ((v <<  8) & 0xff0000)| ((v << 24) & 0xff000000));
}
#endif
static inline uint64_t wyr3_(const uint8_t *p, size_t k) { return (((uint64_t)p[0])<<16)|(((uint64_t)p[k>>1])<<8)|p[k-1];}

//wyhash main function
static inline uint64_t wyhash(const void *key, size_t len, uint64_t seed, const uint64_t *secret){
  const uint8_t *p=(const uint8_t *)key; seed^=wymix_(seed^secret[0],secret[1]);	uint64_t	a,	b;
  if(likely_(len<=16)){
    if(likely_(len>=4)){ a=(wyr4_(p)<<32)|wyr4_(p+((len>>3)<<2)); b=(wyr4_(p+len-4)<<32)|wyr4_(p+len-4-((len>>3)<<2)); }
    else if(likely_(len>0)){ a=wyr3_(p,len); b=0;}
    else a=b=0;
  }
  else{
    size_t i=len; 
    if(unlikely_(i>48)){
      uint64_t see1=seed, see2=seed;
      do{
        seed=wymix_(wyr8_(p)^secret[1],wyr8_(p+8)^seed);
        see1=wymix_(wyr8_(p+16)^secret[2],wyr8_(p+24)^see1);
        see2=wymix_(wyr8_(p+32)^secret[3],wyr8_(p+40)^see2);
        p+=48; i-=48;
      }while(likely_(i>48));
      seed^=see1^see2;
    }
    while(unlikely_(i>16)){  seed=wymix_(wyr8_(p)^secret[1],wyr8_(p+8)^seed);  i-=16; p+=16;  }
    a=wyr8_(p+i-16);  b=wyr8_(p+i-8);
  }
  a^=secret[1]; b^=seed;  wymum_(&a,&b);
  return  wymix_(a^secret[0]^len,b^secret[1]);
}

//the default secret parameters
static const uint64_t wyp_[4] = {0xa0761d6478bd642full, 0xe7037ed1a0b428dbull, 0x8ebc6af09c88c6e3ull, 0x589965cc75374cc3ull};

uint64_t flecs_hash(
    const void *data,
    ecs_size_t length)
{
    return wyhash(data, flecs_ito(size_t, length), 0, wyp_);
}
