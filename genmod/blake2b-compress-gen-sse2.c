#define _MM_MALLOC_H_INCLUDED

# ifdef __GNUC__
#  pragma GCC target("sse2")
#endif

#include "blake2b-config-sse2.h"
#include "blake2b-round-sse2.h"
#include "blake2b-load-sse2.h"
#include "blake2b-compress-gen.c"
