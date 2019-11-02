#define _MM_MALLOC_H_INCLUDED

# ifdef __GNUC__
#  pragma GCC target("sse2")
#  pragma GCC target("sse4.1")
#  pragma GCC target("avx")
#endif

#include "blake2b-config-avx.h"
#include "blake2b-round-avx.h"
#include "blake2b-load-avx.h"
#include "blake2b-compress-gen.c"

#error "not implemented"
