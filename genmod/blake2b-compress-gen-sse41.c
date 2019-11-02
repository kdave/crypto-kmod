#define _MM_MALLOC_H_INCLUDED

# ifdef __GNUC__
#  pragma GCC target("sse2")
#  pragma GCC target("sse4.1")
#endif

#include "blake2b-config-sse41.h"
#include "blake2b-round-sse41.h"
#include "blake2b-load-sse41.h"
#include "blake2b-compress-gen.c"
