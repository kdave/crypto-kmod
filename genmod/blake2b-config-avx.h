/*
   BLAKE2 reference source code package - optimized C implementations

   Copyright 2012, Samuel Neves <sneves@dei.uc.pt>.  You may use this under the
   terms of the CC0, the OpenSSL Licence, or the Apache Public License 2.0, at
   your option.  The terms of these licenses can be found at:

   - CC0 1.0 Universal : http://creativecommons.org/publicdomain/zero/1.0
   - OpenSSL license   : https://www.openssl.org/source/license.html
   - Apache 2.0        : http://www.apache.org/licenses/LICENSE-2.0

   More information about the BLAKE2 hash function can be found at
   https://blake2.net.
*/
#ifndef BLAKE2_CONFIG_H
#define BLAKE2_CONFIG_H

/* These don't work everywhere */
#if defined(__SSE2__) || defined(__x86_64__) || defined(__amd64__)
#define HAVE_SSE2
#endif

#if defined(__SSE4_1__)
#define HAVE_SSE41
#endif

#if defined(__AVX__)
#define HAVE_AVX
#endif

#if !defined(HAVE_SSE2)
#error "This code requires at least SSE2."
#endif

#ifndef HAVE_SSE41
#error "SSE41 not detected"
#endif

#ifndef HAVE_AVX
#error "AVX not detected"
#endif

#define HAVE_SSSE3

#undef HAVE_AVX2
#undef HAVE_XOP

#endif
