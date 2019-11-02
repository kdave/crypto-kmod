#define _MM_MALLOC_H_INCLUDED

# ifdef __GNUC__
#  pragma GCC target("sse2")
#  pragma GCC target("ssse3")
#  pragma GCC target("sse4.1")
# endif

#include <linux/types.h>
#include <linux/string.h>
#include <linux/linkage.h>

#include "blake2.h"
#include "blake2-impl.h"

/* Config */
#define BLAKE2_CONFIG_H

#if defined(__SSE2__) || defined(__x86_64__) || defined(__amd64__)
#define HAVE_SSE2
#endif

#ifndef HAVE_SSE2
#error "This code requires at least SSE2."
#endif

#if defined(__SSE4_1__)
#define HAVE_SSE41
#endif

#if defined(__AVX__)
#define HAVE_AVX
#endif

#if defined(__AVX2__)
#define HAVE_AVX2
#endif

#define HAVE_SSSE3
#undef HAVE_XOP

#undef HAVE_AVX
#undef HAVE_AVX2

#include <emmintrin.h>
#if defined(HAVE_SSSE3)
#include <tmmintrin.h>
#endif
#if defined(HAVE_SSE41)
#include <smmintrin.h>
#endif
#if defined(HAVE_AVX)
#include <immintrin.h>
#endif
#if defined(HAVE_XOP)
#include <x86intrin.h>
#endif

/* Round */
#define LOADU(p)  _mm_loadu_si128( (const __m128i *)(p) )
#define STOREU(p,r) _mm_storeu_si128((__m128i *)(p), r)

#define TOF(reg) _mm_castsi128_ps((reg))
#define TOI(reg) _mm_castps_si128((reg))

#define LIKELY(x) __builtin_expect((x),1)

/* Microarchitecture-specific macros */
#ifndef HAVE_XOP
#ifdef HAVE_SSSE3
#define _mm_roti_epi64(x, c) \
    (-(c) == 32) ? _mm_shuffle_epi32((x), _MM_SHUFFLE(2,3,0,1))  \
    : (-(c) == 24) ? _mm_shuffle_epi8((x), r24) \
    : (-(c) == 16) ? _mm_shuffle_epi8((x), r16) \
    : (-(c) == 63) ? _mm_xor_si128(_mm_srli_epi64((x), -(c)), _mm_add_epi64((x), (x)))  \
    : _mm_xor_si128(_mm_srli_epi64((x), -(c)), _mm_slli_epi64((x), 64-(-(c))))
#else
#define _mm_roti_epi64(r, c) _mm_xor_si128(_mm_srli_epi64( (r), -(c) ),_mm_slli_epi64( (r), 64-(-(c)) ))
#endif
#else
/* ... */
#endif

#define G1(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h,b0,b1) \
  row1l = _mm_add_epi64(_mm_add_epi64(row1l, b0), row2l); \
  row1h = _mm_add_epi64(_mm_add_epi64(row1h, b1), row2h); \
  \
  row4l = _mm_xor_si128(row4l, row1l); \
  row4h = _mm_xor_si128(row4h, row1h); \
  \
  row4l = _mm_roti_epi64(row4l, -32); \
  row4h = _mm_roti_epi64(row4h, -32); \
  \
  row3l = _mm_add_epi64(row3l, row4l); \
  row3h = _mm_add_epi64(row3h, row4h); \
  \
  row2l = _mm_xor_si128(row2l, row3l); \
  row2h = _mm_xor_si128(row2h, row3h); \
  \
  row2l = _mm_roti_epi64(row2l, -24); \
  row2h = _mm_roti_epi64(row2h, -24); \

#define G2(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h,b0,b1) \
  row1l = _mm_add_epi64(_mm_add_epi64(row1l, b0), row2l); \
  row1h = _mm_add_epi64(_mm_add_epi64(row1h, b1), row2h); \
  \
  row4l = _mm_xor_si128(row4l, row1l); \
  row4h = _mm_xor_si128(row4h, row1h); \
  \
  row4l = _mm_roti_epi64(row4l, -16); \
  row4h = _mm_roti_epi64(row4h, -16); \
  \
  row3l = _mm_add_epi64(row3l, row4l); \
  row3h = _mm_add_epi64(row3h, row4h); \
  \
  row2l = _mm_xor_si128(row2l, row3l); \
  row2h = _mm_xor_si128(row2h, row3h); \
  \
  row2l = _mm_roti_epi64(row2l, -63); \
  row2h = _mm_roti_epi64(row2h, -63); \

#if defined(HAVE_SSSE3)
#define DIAGONALIZE(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h) \
  t0 = _mm_alignr_epi8(row2h, row2l, 8); \
  t1 = _mm_alignr_epi8(row2l, row2h, 8); \
  row2l = t0; \
  row2h = t1; \
  \
  t0 = row3l; \
  row3l = row3h; \
  row3h = t0;    \
  \
  t0 = _mm_alignr_epi8(row4h, row4l, 8); \
  t1 = _mm_alignr_epi8(row4l, row4h, 8); \
  row4l = t1; \
  row4h = t0;

#define UNDIAGONALIZE(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h) \
  t0 = _mm_alignr_epi8(row2l, row2h, 8); \
  t1 = _mm_alignr_epi8(row2h, row2l, 8); \
  row2l = t0; \
  row2h = t1; \
  \
  t0 = row3l; \
  row3l = row3h; \
  row3h = t0; \
  \
  t0 = _mm_alignr_epi8(row4l, row4h, 8); \
  t1 = _mm_alignr_epi8(row4h, row4l, 8); \
  row4l = t1; \
  row4h = t0;
#else

#define DIAGONALIZE(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h) \
  t0 = row4l;\
  t1 = row2l;\
  row4l = row3l;\
  row3l = row3h;\
  row3h = row4l;\
  row4l = _mm_unpackhi_epi64(row4h, _mm_unpacklo_epi64(t0, t0)); \
  row4h = _mm_unpackhi_epi64(t0, _mm_unpacklo_epi64(row4h, row4h)); \
  row2l = _mm_unpackhi_epi64(row2l, _mm_unpacklo_epi64(row2h, row2h)); \
  row2h = _mm_unpackhi_epi64(row2h, _mm_unpacklo_epi64(t1, t1))

#define UNDIAGONALIZE(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h) \
  t0 = row3l;\
  row3l = row3h;\
  row3h = t0;\
  t0 = row2l;\
  t1 = row4l;\
  row2l = _mm_unpackhi_epi64(row2h, _mm_unpacklo_epi64(row2l, row2l)); \
  row2h = _mm_unpackhi_epi64(t0, _mm_unpacklo_epi64(row2h, row2h)); \
  row4l = _mm_unpackhi_epi64(row4l, _mm_unpacklo_epi64(row4h, row4h)); \
  row4h = _mm_unpackhi_epi64(row4h, _mm_unpacklo_epi64(t1, t1))

#endif

#define ROUND(r) \
  LOAD_MSG_ ##r ##_1(b0, b1); \
  G1(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h,b0,b1); \
  LOAD_MSG_ ##r ##_2(b0, b1); \
  G2(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h,b0,b1); \
  DIAGONALIZE(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h); \
  LOAD_MSG_ ##r ##_3(b0, b1); \
  G1(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h,b0,b1); \
  LOAD_MSG_ ##r ##_4(b0, b1); \
  G2(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h,b0,b1); \
  UNDIAGONALIZE(row1l,row2l,row3l,row4l,row1h,row2h,row3h,row4h);

/* Load */
#if 1
#define LOAD_MSG_0_1(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m0, m1); \
b1 = _mm_unpacklo_epi64(m2, m3); \
} while(0)


#define LOAD_MSG_0_2(b0, b1) \
do \
{ \
b0 = _mm_unpackhi_epi64(m0, m1); \
b1 = _mm_unpackhi_epi64(m2, m3); \
} while(0)


#define LOAD_MSG_0_3(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m4, m5); \
b1 = _mm_unpacklo_epi64(m6, m7); \
} while(0)


#define LOAD_MSG_0_4(b0, b1) \
do \
{ \
b0 = _mm_unpackhi_epi64(m4, m5); \
b1 = _mm_unpackhi_epi64(m6, m7); \
} while(0)


#define LOAD_MSG_1_1(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m7, m2); \
b1 = _mm_unpackhi_epi64(m4, m6); \
} while(0)


#define LOAD_MSG_1_2(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m5, m4); \
b1 = _mm_alignr_epi8(m3, m7, 8); \
} while(0)


#define LOAD_MSG_1_3(b0, b1) \
do \
{ \
b0 = _mm_shuffle_epi32(m0, _MM_SHUFFLE(1,0,3,2)); \
b1 = _mm_unpackhi_epi64(m5, m2); \
} while(0)


#define LOAD_MSG_1_4(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m6, m1); \
b1 = _mm_unpackhi_epi64(m3, m1); \
} while(0)


#define LOAD_MSG_2_1(b0, b1) \
do \
{ \
b0 = _mm_alignr_epi8(m6, m5, 8); \
b1 = _mm_unpackhi_epi64(m2, m7); \
} while(0)


#define LOAD_MSG_2_2(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m4, m0); \
b1 = _mm_blend_epi16(m1, m6, 0xF0); \
} while(0)


#define LOAD_MSG_2_3(b0, b1) \
do \
{ \
b0 = _mm_blend_epi16(m5, m1, 0xF0); \
b1 = _mm_unpackhi_epi64(m3, m4); \
} while(0)


#define LOAD_MSG_2_4(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m7, m3); \
b1 = _mm_alignr_epi8(m2, m0, 8); \
} while(0)


#define LOAD_MSG_3_1(b0, b1) \
do \
{ \
b0 = _mm_unpackhi_epi64(m3, m1); \
b1 = _mm_unpackhi_epi64(m6, m5); \
} while(0)


#define LOAD_MSG_3_2(b0, b1) \
do \
{ \
b0 = _mm_unpackhi_epi64(m4, m0); \
b1 = _mm_unpacklo_epi64(m6, m7); \
} while(0)


#define LOAD_MSG_3_3(b0, b1) \
do \
{ \
b0 = _mm_blend_epi16(m1, m2, 0xF0); \
b1 = _mm_blend_epi16(m2, m7, 0xF0); \
} while(0)


#define LOAD_MSG_3_4(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m3, m5); \
b1 = _mm_unpacklo_epi64(m0, m4); \
} while(0)


#define LOAD_MSG_4_1(b0, b1) \
do \
{ \
b0 = _mm_unpackhi_epi64(m4, m2); \
b1 = _mm_unpacklo_epi64(m1, m5); \
} while(0)


#define LOAD_MSG_4_2(b0, b1) \
do \
{ \
b0 = _mm_blend_epi16(m0, m3, 0xF0); \
b1 = _mm_blend_epi16(m2, m7, 0xF0); \
} while(0)


#define LOAD_MSG_4_3(b0, b1) \
do \
{ \
b0 = _mm_blend_epi16(m7, m5, 0xF0); \
b1 = _mm_blend_epi16(m3, m1, 0xF0); \
} while(0)


#define LOAD_MSG_4_4(b0, b1) \
do \
{ \
b0 = _mm_alignr_epi8(m6, m0, 8); \
b1 = _mm_blend_epi16(m4, m6, 0xF0); \
} while(0)


#define LOAD_MSG_5_1(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m1, m3); \
b1 = _mm_unpacklo_epi64(m0, m4); \
} while(0)


#define LOAD_MSG_5_2(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m6, m5); \
b1 = _mm_unpackhi_epi64(m5, m1); \
} while(0)


#define LOAD_MSG_5_3(b0, b1) \
do \
{ \
b0 = _mm_blend_epi16(m2, m3, 0xF0); \
b1 = _mm_unpackhi_epi64(m7, m0); \
} while(0)


#define LOAD_MSG_5_4(b0, b1) \
do \
{ \
b0 = _mm_unpackhi_epi64(m6, m2); \
b1 = _mm_blend_epi16(m7, m4, 0xF0); \
} while(0)


#define LOAD_MSG_6_1(b0, b1) \
do \
{ \
b0 = _mm_blend_epi16(m6, m0, 0xF0); \
b1 = _mm_unpacklo_epi64(m7, m2); \
} while(0)


#define LOAD_MSG_6_2(b0, b1) \
do \
{ \
b0 = _mm_unpackhi_epi64(m2, m7); \
b1 = _mm_alignr_epi8(m5, m6, 8); \
} while(0)


#define LOAD_MSG_6_3(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m0, m3); \
b1 = _mm_shuffle_epi32(m4, _MM_SHUFFLE(1,0,3,2)); \
} while(0)


#define LOAD_MSG_6_4(b0, b1) \
do \
{ \
b0 = _mm_unpackhi_epi64(m3, m1); \
b1 = _mm_blend_epi16(m1, m5, 0xF0); \
} while(0)


#define LOAD_MSG_7_1(b0, b1) \
do \
{ \
b0 = _mm_unpackhi_epi64(m6, m3); \
b1 = _mm_blend_epi16(m6, m1, 0xF0); \
} while(0)


#define LOAD_MSG_7_2(b0, b1) \
do \
{ \
b0 = _mm_alignr_epi8(m7, m5, 8); \
b1 = _mm_unpackhi_epi64(m0, m4); \
} while(0)


#define LOAD_MSG_7_3(b0, b1) \
do \
{ \
b0 = _mm_unpackhi_epi64(m2, m7); \
b1 = _mm_unpacklo_epi64(m4, m1); \
} while(0)


#define LOAD_MSG_7_4(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m0, m2); \
b1 = _mm_unpacklo_epi64(m3, m5); \
} while(0)


#define LOAD_MSG_8_1(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m3, m7); \
b1 = _mm_alignr_epi8(m0, m5, 8); \
} while(0)


#define LOAD_MSG_8_2(b0, b1) \
do \
{ \
b0 = _mm_unpackhi_epi64(m7, m4); \
b1 = _mm_alignr_epi8(m4, m1, 8); \
} while(0)


#define LOAD_MSG_8_3(b0, b1) \
do \
{ \
b0 = m6; \
b1 = _mm_alignr_epi8(m5, m0, 8); \
} while(0)


#define LOAD_MSG_8_4(b0, b1) \
do \
{ \
b0 = _mm_blend_epi16(m1, m3, 0xF0); \
b1 = m2; \
} while(0)


#define LOAD_MSG_9_1(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m5, m4); \
b1 = _mm_unpackhi_epi64(m3, m0); \
} while(0)


#define LOAD_MSG_9_2(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m1, m2); \
b1 = _mm_blend_epi16(m3, m2, 0xF0); \
} while(0)


#define LOAD_MSG_9_3(b0, b1) \
do \
{ \
b0 = _mm_unpackhi_epi64(m7, m4); \
b1 = _mm_unpackhi_epi64(m1, m6); \
} while(0)


#define LOAD_MSG_9_4(b0, b1) \
do \
{ \
b0 = _mm_alignr_epi8(m7, m5, 8); \
b1 = _mm_unpacklo_epi64(m6, m0); \
} while(0)


#define LOAD_MSG_10_1(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m0, m1); \
b1 = _mm_unpacklo_epi64(m2, m3); \
} while(0)


#define LOAD_MSG_10_2(b0, b1) \
do \
{ \
b0 = _mm_unpackhi_epi64(m0, m1); \
b1 = _mm_unpackhi_epi64(m2, m3); \
} while(0)


#define LOAD_MSG_10_3(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m4, m5); \
b1 = _mm_unpacklo_epi64(m6, m7); \
} while(0)


#define LOAD_MSG_10_4(b0, b1) \
do \
{ \
b0 = _mm_unpackhi_epi64(m4, m5); \
b1 = _mm_unpackhi_epi64(m6, m7); \
} while(0)


#define LOAD_MSG_11_1(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m7, m2); \
b1 = _mm_unpackhi_epi64(m4, m6); \
} while(0)


#define LOAD_MSG_11_2(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m5, m4); \
b1 = _mm_alignr_epi8(m3, m7, 8); \
} while(0)


#define LOAD_MSG_11_3(b0, b1) \
do \
{ \
b0 = _mm_shuffle_epi32(m0, _MM_SHUFFLE(1,0,3,2)); \
b1 = _mm_unpackhi_epi64(m5, m2); \
} while(0)


#define LOAD_MSG_11_4(b0, b1) \
do \
{ \
b0 = _mm_unpacklo_epi64(m6, m1); \
b1 = _mm_unpackhi_epi64(m3, m1); \
} while(0)
#endif

static const uint64_t blake2b_IV[8] =
{
  0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL,
  0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL,
  0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL,
  0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL
};

asmlinkage
void blake2b_compress(struct blake2b_state *S, const uint8_t block[BLAKE2B_BLOCKBYTES] )
{
  __m128i row1l, row1h;
  __m128i row2l, row2h;
  __m128i row3l, row3h;
  __m128i row4l, row4h;
  __m128i b0, b1;
  __m128i t0, t1;
#if defined(HAVE_SSSE3) && !defined(HAVE_XOP)
  const __m128i r16 = _mm_setr_epi8( 2, 3, 4, 5, 6, 7, 0, 1, 10, 11, 12, 13, 14, 15, 8, 9 );
  const __m128i r24 = _mm_setr_epi8( 3, 4, 5, 6, 7, 0, 1, 2, 11, 12, 13, 14, 15, 8, 9, 10 );
#endif
#if defined(HAVE_SSE41)
  const __m128i m0 = LOADU( block + 00 );
  const __m128i m1 = LOADU( block + 16 );
  const __m128i m2 = LOADU( block + 32 );
  const __m128i m3 = LOADU( block + 48 );
  const __m128i m4 = LOADU( block + 64 );
  const __m128i m5 = LOADU( block + 80 );
  const __m128i m6 = LOADU( block + 96 );
  const __m128i m7 = LOADU( block + 112 );
#else
  const uint64_t  m0 = load64(block +  0 * sizeof(uint64_t));
  const uint64_t  m1 = load64(block +  1 * sizeof(uint64_t));
  const uint64_t  m2 = load64(block +  2 * sizeof(uint64_t));
  const uint64_t  m3 = load64(block +  3 * sizeof(uint64_t));
  const uint64_t  m4 = load64(block +  4 * sizeof(uint64_t));
  const uint64_t  m5 = load64(block +  5 * sizeof(uint64_t));
  const uint64_t  m6 = load64(block +  6 * sizeof(uint64_t));
  const uint64_t  m7 = load64(block +  7 * sizeof(uint64_t));
  const uint64_t  m8 = load64(block +  8 * sizeof(uint64_t));
  const uint64_t  m9 = load64(block +  9 * sizeof(uint64_t));
  const uint64_t m10 = load64(block + 10 * sizeof(uint64_t));
  const uint64_t m11 = load64(block + 11 * sizeof(uint64_t));
  const uint64_t m12 = load64(block + 12 * sizeof(uint64_t));
  const uint64_t m13 = load64(block + 13 * sizeof(uint64_t));
  const uint64_t m14 = load64(block + 14 * sizeof(uint64_t));
  const uint64_t m15 = load64(block + 15 * sizeof(uint64_t));
#endif
  row1l = LOADU( &S->h[0] );
  row1h = LOADU( &S->h[2] );
  row2l = LOADU( &S->h[4] );
  row2h = LOADU( &S->h[6] );
  row3l = LOADU( &blake2b_IV[0] );
  row3h = LOADU( &blake2b_IV[2] );
  row4l = _mm_xor_si128( LOADU( &blake2b_IV[4] ), LOADU( &S->t[0] ) );
  row4h = _mm_xor_si128( LOADU( &blake2b_IV[6] ), LOADU( &S->f[0] ) );

  ROUND( 0 );
  ROUND( 1 );
  ROUND( 2 );
  ROUND( 3 );
  ROUND( 4 );
  ROUND( 5 );
  ROUND( 6 );
  ROUND( 7 );
  ROUND( 8 );
  ROUND( 9 );
  ROUND( 10 );
  ROUND( 11 );

  row1l = _mm_xor_si128( row3l, row1l );
  row1h = _mm_xor_si128( row3h, row1h );
  STOREU( &S->h[0], _mm_xor_si128( LOADU( &S->h[0] ), row1l ) );
  STOREU( &S->h[2], _mm_xor_si128( LOADU( &S->h[2] ), row1h ) );
  row2l = _mm_xor_si128( row4l, row2l );
  row2h = _mm_xor_si128( row4h, row2h );
  STOREU( &S->h[4], _mm_xor_si128( LOADU( &S->h[4] ), row2l ) );
  STOREU( &S->h[6], _mm_xor_si128( LOADU( &S->h[6] ), row2h ) );
}
