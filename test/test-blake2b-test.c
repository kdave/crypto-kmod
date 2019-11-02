#include <stdio.h>
#include <string.h>
#include <asm/types.h>

#include "test-blake2.h"
#include "blake2-impl.h"

static const u64 blake2b_IV[8] =
{
	0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL,
	0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL,
	0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL,
	0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL
};

static const u8 blake2b_sigma[12][16] =
{
	{  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 },
	{ 14, 10,  4,  8,  9, 15, 13,  6,  1, 12,  0,  2, 11,  7,  5,  3 },
	{ 11,  8, 12,  0,  5,  2, 15, 13, 10, 14,  3,  6,  7,  1,  9,  4 },
	{  7,  9,  3,  1, 13, 12, 11, 14,  2,  6,  5, 10,  4,  0, 15,  8 },
	{  9,  0,  5,  7,  2,  4, 10, 15, 14,  1, 11, 12,  6,  8,  3, 13 },
	{  2, 12,  6, 10,  0, 11,  8,  3,  4, 13,  7,  5, 15, 14,  1,  9 },
	{ 12,  5,  1, 15, 14, 13,  4, 10,  0,  7,  6,  3,  9,  2,  8, 11 },
	{ 13, 11,  7, 14, 12,  1,  3,  9,  5,  0, 15,  4,  8,  6,  2, 10 },
	{  6, 15, 14,  9, 11,  3,  0,  8, 12,  2, 13,  7,  1,  4, 10,  5 },
	{ 10,  2,  8,  4,  7,  6,  1,  5, 15, 11,  9, 14,  3, 12, 13 , 0 },
	{  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 },
	{ 14, 10,  4,  8,  9, 15, 13,  6,  1, 12,  0,  2, 11,  7,  5,  3 }
};

static void blake2b_set_lastnode(struct blake2b_state *S)
{
	S->f[1] = (u64)-1;
}

/* Some helper functions, not necessarily useful */
static int blake2b_is_lastblock(const struct blake2b_state *S)
{
	return S->f[0] != 0;
}

static void blake2b_set_lastblock(struct blake2b_state *S)
{
	if (S->last_node)
		blake2b_set_lastnode(S);

	S->f[0] = (u64)-1;
}

static void blake2b_increment_counter(struct blake2b_state *S, const u64 inc)
{
	S->t[0] += inc;
	S->t[1] += (S->t[0] < inc);
}

static void blake2b_init0(struct blake2b_state *S)
{
	size_t i;

	memset(S, 0, sizeof(struct blake2b_state));

	for (i = 0; i < 8; ++i)
		S->h[i] = blake2b_IV[i];
}

/* init xors IV with input parameter block */
int blake2b_init_param(struct blake2b_state *S, const struct blake2b_param *P)
{
	const u8 *p = (const u8 *)(P);
	size_t i;

	blake2b_init0(S);

	/* IV XOR ParamBlock */
	for (i = 0; i < 8; ++i)
		S->h[i] ^= load64(p + sizeof(S->h[i]) * i);

	S->outlen = P->digest_length;
	return 0;
}

int blake2b_init(struct blake2b_state *S, size_t outlen)
{
	struct blake2b_param P[1];

	if ((!outlen) || (outlen > BLAKE2B_OUTBYTES))
		return -1;

	P->digest_length = (u8)outlen;
	P->key_length    = 0;
	P->fanout        = 1;
	P->depth         = 1;
	store32(&P->leaf_length, 0);
	store32(&P->node_offset, 0);
	store32(&P->xof_length, 0);
	P->node_depth    = 0;
	P->inner_length  = 0;
	memset(P->reserved, 0, sizeof(P->reserved));
	memset(P->salt,     0, sizeof(P->salt));
	memset(P->personal, 0, sizeof(P->personal));
	return blake2b_init_param(S, P);
}

int blake2b_init_key(struct blake2b_state *S, size_t outlen, const void *key,
		     size_t keylen)
{
	struct blake2b_param P[1];

	if ((!outlen) || (outlen > BLAKE2B_OUTBYTES))
		return -1;

	if (!key || !keylen || keylen > BLAKE2B_KEYBYTES)
		return -1;

	P->digest_length = (u8)outlen;
	P->key_length    = (u8)keylen;
	P->fanout        = 1;
	P->depth         = 1;
	store32(&P->leaf_length, 0);
	store32(&P->node_offset, 0);
	store32(&P->xof_length, 0);
	P->node_depth    = 0;
	P->inner_length  = 0;
	memset(P->reserved, 0, sizeof(P->reserved));
	memset(P->salt,     0, sizeof(P->salt));
	memset(P->personal, 0, sizeof(P->personal));

	if (blake2b_init_param(S, P) < 0)
		return -1;

	{
		u8 block[BLAKE2B_BLOCKBYTES];

		memset(block, 0, BLAKE2B_BLOCKBYTES);
		memcpy(block, key, keylen);
		blake2b_update(S, block, BLAKE2B_BLOCKBYTES);
		memset(block, 0, BLAKE2B_BLOCKBYTES);
	}
	return 0;
}

#define G(r,i,a,b,c,d)                                  \
	do {                                            \
		a = a + b + m[blake2b_sigma[r][2*i+0]]; \
		d = rotr64(d ^ a, 32);                  \
		c = c + d;                              \
		b = rotr64(b ^ c, 24);                  \
		a = a + b + m[blake2b_sigma[r][2*i+1]]; \
		d = rotr64(d ^ a, 16);                  \
		c = c + d;                              \
		b = rotr64(b ^ c, 63);                  \
	} while(0)

#define ROUND(r)                                \
	do {                                    \
		G(r,0,v[ 0],v[ 4],v[ 8],v[12]); \
		G(r,1,v[ 1],v[ 5],v[ 9],v[13]); \
		G(r,2,v[ 2],v[ 6],v[10],v[14]); \
		G(r,3,v[ 3],v[ 7],v[11],v[15]); \
		G(r,4,v[ 0],v[ 5],v[10],v[15]); \
		G(r,5,v[ 1],v[ 6],v[11],v[12]); \
		G(r,6,v[ 2],v[ 7],v[ 8],v[13]); \
		G(r,7,v[ 3],v[ 4],v[ 9],v[14]); \
	} while(0)

static void blake2b_compress(struct blake2b_state *S,
			     const u8 block[BLAKE2B_BLOCKBYTES])
{
	u64 m[16];
	u64 v[16];
	size_t i;

	for (i = 0; i < 16; ++i)
		m[i] = load64(block + i * sizeof(m[i]));

	for (i = 0; i < 8; ++i)
		v[i] = S->h[i];

	v[ 8] = blake2b_IV[0];
	v[ 9] = blake2b_IV[1];
	v[10] = blake2b_IV[2];
	v[11] = blake2b_IV[3];
	v[12] = blake2b_IV[4] ^ S->t[0];
	v[13] = blake2b_IV[5] ^ S->t[1];
	v[14] = blake2b_IV[6] ^ S->f[0];
	v[15] = blake2b_IV[7] ^ S->f[1];

	ROUND(0);
	ROUND(1);
	ROUND(2);
	ROUND(3);
	ROUND(4);
	ROUND(5);
	ROUND(6);
	ROUND(7);
	ROUND(8);
	ROUND(9);
	ROUND(10);
	ROUND(11);

	for (i = 0; i < 8; ++i)
		S->h[i] = S->h[i] ^ v[i] ^ v[i + 8];
}

#undef G
#undef ROUND

int blake2b_update(struct blake2b_state *S, const void *pin, size_t inlen)
{
	const unsigned char *in = (const unsigned char *)pin;

	if (inlen > 0) {
		size_t left = S->buflen;
		size_t fill = BLAKE2B_BLOCKBYTES - left;

		if (inlen > fill) {
			S->buflen = 0;
			/* Fill buffer */
			memcpy(S->buf + left, in, fill);
			blake2b_increment_counter(S, BLAKE2B_BLOCKBYTES);
			/* Compress */
			blake2b_compress(S, S->buf);
			in += fill;
			inlen -= fill;
			while (inlen > BLAKE2B_BLOCKBYTES) {
				blake2b_increment_counter(S, BLAKE2B_BLOCKBYTES);
				blake2b_compress(S, in);
				in += BLAKE2B_BLOCKBYTES;
				inlen -= BLAKE2B_BLOCKBYTES;
			}
		}
		memcpy(S->buf + S->buflen, in, inlen);
		S->buflen += inlen;
	}
	return 0;
}

int blake2b_final(struct blake2b_state *S, void *out, size_t outlen)
{
	u8 buffer[BLAKE2B_OUTBYTES] = {0};
	size_t i;

	if (out == NULL || outlen < S->outlen)
		return -1;

	if (blake2b_is_lastblock(S))
		return -1;

	blake2b_increment_counter(S, S->buflen);
	blake2b_set_lastblock(S);
	/* Padding */
	memset(S->buf + S->buflen, 0, BLAKE2B_BLOCKBYTES - S->buflen);
	blake2b_compress(S, S->buf);

	/* Output full hash to temp buffer */
	for (i = 0; i < 8; ++i)
		store64(buffer + sizeof(S->h[i]) * i, S->h[i]);

	memcpy(out, buffer, S->outlen);
	memset(buffer, 0, sizeof(buffer));
	return 0;
}

int main(void) {
	/*
{

in:     00
key:    000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f
hash:   961f6dd1e4dd30f63901690c512e78e4b45e4742ed197c3c5e45c549fd25f2e4187b0bc9fe30492b16b0d0bc4ef9b0f34c7003fac09a5ef1532e69430234cebd
},
*/
	{
		struct blake2b_state S[1];
		u8 key[BLAKE2B_KEYBYTES];
		int ret;
		char data[] = { 0, 1, 2, 3, 4};
		u8 out[BLAKE2B_OUTBYTES];
		int length = 0;
		int i;

		for (i = 0; i < BLAKE2B_KEYBYTES; i++)
			key[i] = (u8)i;

		ret = blake2b_init_key(S, BLAKE2B_OUTBYTES, key, BLAKE2B_KEYBYTES);
		ret = blake2b_update(S, data, length);
		ret = blake2b_final(S, out, BLAKE2B_OUTBYTES);

		for (i = 0; i < BLAKE2B_OUTBYTES; i++)
			printf("%02x", out[i]);
		printf("\n");
	}

	{
		struct blake2b_state S[1];
		u8 key[BLAKE2B_KEYBYTES];
		int ret;
		char data[] = { 0, 1, 2, 3, 4};
		u8 out[BLAKE2B_OUTBYTES];
		int length = 0;
		int i;

		for (i = 0; i < BLAKE2B_KEYBYTES; i++)
			key[i] = (u8)i;

		ret = blake2b_init(S, BLAKE2B_OUTBYTES);
		ret = blake2b_update(S, data, length);
		ret = blake2b_final(S, out, BLAKE2B_OUTBYTES);

		for (i = 0; i < BLAKE2B_OUTBYTES; i++)
			printf("%02x", out[i]);
		printf("\n");
	}

	return 0;
}
