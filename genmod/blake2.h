/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * BLAKE2 reference source code package - reference C implementations
 *
 * Copyright 2012, Samuel Neves <sneves@dei.uc.pt>.  You may use this under the
 * terms of the CC0, the OpenSSL Licence, or the Apache Public License 2.0, at
 * your option.  The terms of these licenses can be found at:
 *
 * - CC0 1.0 Universal : http://creativecommons.org/publicdomain/zero/1.0
 * - OpenSSL license   : https://www.openssl.org/source/license.html
 * - Apache 2.0        : http://www.apache.org/licenses/LICENSE-2.0
 *
 * More information about the BLAKE2 hash function can be found at
 * https://blake2.net.
*/

#ifndef BLAKE2_H
#define BLAKE2_H

#include <linux/compiler.h>
#include <stddef.h>

enum blake2s_constant
{
	BLAKE2S_BLOCKBYTES = 64,
	BLAKE2S_OUTBYTES   = 32,
	BLAKE2S_KEYBYTES   = 32,
	BLAKE2S_SALTBYTES  = 8,
	BLAKE2S_PERSONALBYTES = 8
};

enum blake2b_constant
{
	BLAKE2B_BLOCKBYTES = 128,
	BLAKE2B_OUTBYTES   = 64,
	BLAKE2B_KEYBYTES   = 64,
	BLAKE2B_SALTBYTES  = 16,
	BLAKE2B_PERSONALBYTES = 16
};

struct blake2s_state
{
	u32      h[8];
	u32      t[2];
	u32      f[2];
	u8       buf[BLAKE2S_BLOCKBYTES];
	size_t   buflen;
	size_t   outlen;
	u8  last_node;
};

struct blake2b_state
{
	u64      h[8];
	u64      t[2];
	u64      f[2];
	u8       buf[BLAKE2B_BLOCKBYTES];
	size_t   buflen;
	size_t   outlen;
	u8       last_node;
};
typedef struct blake2b_state blake2b_state;

struct blake2s_param
{
	u8  digest_length; /* 1 */
	u8  key_length;    /* 2 */
	u8  fanout;        /* 3 */
	u8  depth;         /* 4 */
	u32 leaf_length;   /* 8 */
	u32 node_offset;   /* 12 */
	u16 xof_length;    /* 14 */
	u8  node_depth;    /* 15 */
	u8  inner_length;  /* 16 */
	u8  salt[BLAKE2S_SALTBYTES]; /* 24 */
	u8  personal[BLAKE2S_PERSONALBYTES];  /* 32 */
} __packed;

struct blake2b_param
{
	u8  digest_length; /* 1 */
	u8  key_length;    /* 2 */
	u8  fanout;        /* 3 */
	u8  depth;         /* 4 */
	u32 leaf_length;   /* 8 */
	u32 node_offset;   /* 12 */
	u32 xof_length;    /* 16 */
	u8  node_depth;    /* 17 */
	u8  inner_length;  /* 18 */
	u8  reserved[14];  /* 32 */
	u8  salt[BLAKE2B_SALTBYTES]; /* 48 */
	u8  personal[BLAKE2B_PERSONALBYTES];  /* 64 */
} __packed;

/* Streaming API */
int blake2s_init(struct blake2s_state *S, size_t outlen);
int blake2s_init_key(struct blake2s_state *S, size_t outlen, const void *key, size_t keylen);
int blake2s_init_param(struct blake2s_state *S, const struct blake2s_param *P);
int blake2s_update(struct blake2s_state *S, const void *in, size_t inlen);
int blake2s_final(struct blake2s_state *S, void *out, size_t outlen);

int blake2b_init(struct blake2b_state *S, size_t outlen);
int blake2b_init_key(struct blake2b_state *S, size_t outlen, const void *key, size_t keylen);
int blake2b_init_param(struct blake2b_state *S, const struct blake2b_param *P);
int blake2b_update(struct blake2b_state *S, const void *in, size_t inlen);
int blake2b_final(struct blake2b_state *S, void *out, size_t outlen);

#endif
