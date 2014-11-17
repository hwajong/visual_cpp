#include "hmac_sha2.h"
#include <cstring>

using namespace std;


#define _BIG_ENDIAN

typedef unsigned char U8;
typedef unsigned int U32;

#define ENABLE_SHA_256
#define ENABLE_SHA_512
/*
#define ENABLE_SHA_224
#define ENABLE_SHA_384
*/

typedef struct {	/* sha-224/256 uses half size of buffer */
	U32 bits[4];
	U32 input[32];
	U32 state[16];
	int hashbitlen;
} SHA2_CTX;

#define INIT224_H0 0xc1059ed8
#define INIT224_H1 0x367cd507
#define INIT224_H2 0x3070dd17
#define INIT224_H3 0xf70e5939
#define INIT224_H4 0xffc00b31
#define INIT224_H5 0x68581511
#define INIT224_H6 0x64f98fa7
#define INIT224_H7 0xbefa4fa4

#define INIT256_H0 0x6a09e667
#define INIT256_H1 0xbb67ae85
#define INIT256_H2 0x3c6ef372
#define INIT256_H3 0xa54ff53a
#define INIT256_H4 0x510e527f
#define INIT256_H5 0x9b05688c
#define INIT256_H6 0x1f83d9ab
#define INIT256_H7 0x5be0cd19

#define INIT384_H0h 0xcbbb9d5d
#define INIT384_H0l 0xc1059ed8
#define INIT384_H1h 0x629a292a
#define INIT384_H1l 0x367cd507
#define INIT384_H2h 0x9159015a
#define INIT384_H2l 0x3070dd17
#define INIT384_H3h 0x152fecd8
#define INIT384_H3l 0xf70e5939
#define INIT384_H4h 0x67332667
#define INIT384_H4l 0xffc00b31
#define INIT384_H5h 0x8eb44a87
#define INIT384_H5l 0x68581511
#define INIT384_H6h 0xdb0c2e0d
#define INIT384_H6l 0x64f98fa7
#define INIT384_H7h 0x47b5481d
#define INIT384_H7l 0xbefa4fa4

#define INIT512_H0h 0x6a09e667
#define INIT512_H0l 0xf3bcc908
#define INIT512_H1h 0xbb67ae85
#define INIT512_H1l 0x84caa73b
#define INIT512_H2h 0x3c6ef372
#define INIT512_H2l 0xfe94f82b
#define INIT512_H3h 0xa54ff53a
#define INIT512_H3l 0x5f1d36f1
#define INIT512_H4h 0x510e527f
#define INIT512_H4l 0xade682d1
#define INIT512_H5h 0x9b05688c
#define INIT512_H5l 0x2b3e6c1f
#define INIT512_H6h 0x1f83d9ab
#define INIT512_H6l 0xfb41bd6b
#define INIT512_H7h 0x5be0cd19
#define INIT512_H7l 0x137e2179

#define ROTL(x, n) (((x) << (n)) | ((x) >> (32-(n))))
#define ROTR(x, n) (((x) >> (n)) | ((x) << (32-(n))))
#define SHR(x,n) ((x)>>(n))

#define ROTRh(xh,xl,n) ( (n<32) ? ((xh>>n)|(xl<<(32-n))) : ((xh<<(64-n))|(xl>>(n-32))) )
#define ROTRl(xh,xl,n) ( (n<32) ? ((xh<<(32-n))|(xl>>n)) : ((xh>>(n-32))|(xl<<(64-n))) )
#define SHRh(xh,xl,n) ( (n<32) ? (xh>>n) : 0 )
#define SHRl(xh,xl,n) ( (n<32) ? ((xh<<(32-n))|(xl>>n)) : (xh>>(n-32)) )

#define CH(x,y,z)  ( ((x) & (y)) | (~(x) & (z)) )
#define MAJ(x,y,z) ( ( (x) & (y) ) | ( (x) & (z) ) | ( (y) & (z) ) )

#define SIGMA256_0(x)  ( ROTR(x, 2) ^ ROTR(x,13) ^ ROTR(x,22) )
#define SIGMA256_1(x)  ( ROTR(x, 6) ^ ROTR(x,11) ^ ROTR(x,25) )
#define sigma256_0(x)  ( ROTR(x, 7) ^ ROTR(x,18) ^ SHR(x, 3) )
#define sigma256_1(x)  ( ROTR(x,17) ^ ROTR(x,19) ^ SHR(x,10) )

#define SIGMA512_0h(xh,xl) ( ROTRh(xh,xl,28) ^ ROTRh(xh,xl,34) ^ ROTRh(xh,xl,39) )
#define SIGMA512_0l(xh,xl) ( ROTRl(xh,xl,28) ^ ROTRl(xh,xl,34) ^ ROTRl(xh,xl,39) )
#define SIGMA512_1h(xh,xl) ( ROTRh(xh,xl,14) ^ ROTRh(xh,xl,18) ^ ROTRh(xh,xl,41) )
#define SIGMA512_1l(xh,xl) ( ROTRl(xh,xl,14) ^ ROTRl(xh,xl,18) ^ ROTRl(xh,xl,41) )
#define sigma512_0h(xh,xl) ( ROTRh(xh,xl, 1) ^ ROTRh(xh,xl, 8) ^ SHRh(xh,xl, 7) )
#define sigma512_0l(xh,xl) ( ROTRl(xh,xl, 1) ^ ROTRl(xh,xl, 8) ^ SHRl(xh,xl, 7) )
#define sigma512_1h(xh,xl) ( ROTRh(xh,xl,19) ^ ROTRh(xh,xl,61) ^ SHRh(xh,xl, 6) )
#define sigma512_1l(xh,xl) ( ROTRl(xh,xl,19) ^ ROTRl(xh,xl,61) ^ SHRl(xh,xl, 6) )

#define ADDC(xh,xl,yh,yl) { xl += (yl); xh += (yh) + (xl < (yl)); }

#ifndef _BIG_ENDIAN
#define Reverse1(n, d) \
{ \
	l2=d[n]; \
	d[n]=((ROTL(l2,8)&0x00FF00FF)|(ROTL(l2,24)&0xFF00FF00)); \
}

#define Reverse(n, d) \
{ \
	for(l1 = 0; l1 < n; l1 ++) \
	Reverse1(l1, d); \
}

#endif

#define F256(A,B,C,D,E,F,G,H,j) { \
	T1 = H + SIGMA256_1(E) + CH(E,F,G) + K256[j] + WW[j]; \
	D += T1; \
	H = T1 + SIGMA256_0(A) + MAJ(A,B,C); }

#define F512(A,B,C,D,E,F,G,H,j) {	\
	T1##h = H##h; T1##l = H##l; \
	ADDC( T1##h, T1##l, SIGMA512_1h(E##h,E##l), SIGMA512_1l(E##h,E##l) ); \
	ADDC( T1##h, T1##l, CH(E##h,F##h,G##h), CH(E##l,F##l,G##l)); \
	ADDC( T1##h, T1##l, K512[2*(j)], K512[2*(j)+1] ); \
	ADDC( T1##h, T1##l, WW[2*(j)], WW[2*(j)+1] ); \
	ADDC( D##h, D##l, T1##h, T1##l ); \
	H##h = T1##h; H##l = T1##l; \
	ADDC( H##h, H##l, SIGMA512_0h(A##h,A##l), SIGMA512_0l(A##h,A##l) ); \
	ADDC( H##h, H##l, MAJ(A##h,B##h,C##h), MAJ(A##l,B##l,C##l) ); }

#if defined(ENABLE_SHA_224) || defined(ENABLE_SHA_256)
static U32 K256[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static void SHA256_Round(SHA2_CTX *ctx, U32 *X)
{
	U32	A, B, C, D, E, F, G, H, WW[64];
	U32	i, T1;

	for (i = 0; i<16; i++)
		WW[i] = X[i];

	for (i = 16; i<64; i++)
		WW[i] = sigma256_1(WW[i - 2]) + WW[i - 7] + sigma256_0(WW[i - 15]) + WW[i - 16];

	A = ctx->state[0];
	B = ctx->state[1];
	C = ctx->state[2];
	D = ctx->state[3];
	E = ctx->state[4];
	F = ctx->state[5];
	G = ctx->state[6];
	H = ctx->state[7];

	for (i = 0; i<64; i += 8)
	{
		F256(A, B, C, D, E, F, G, H, i + 0);
		F256(H, A, B, C, D, E, F, G, i + 1);
		F256(G, H, A, B, C, D, E, F, i + 2);
		F256(F, G, H, A, B, C, D, E, i + 3);
		F256(E, F, G, H, A, B, C, D, i + 4);
		F256(D, E, F, G, H, A, B, C, i + 5);
		F256(C, D, E, F, G, H, A, B, i + 6);
		F256(B, C, D, E, F, G, H, A, i + 7);
	}

	ctx->state[0] = (ctx->state[0] + A) & 0xffffffff;
	ctx->state[1] = (ctx->state[1] + B) & 0xffffffff;
	ctx->state[2] = (ctx->state[2] + C) & 0xffffffff;
	ctx->state[3] = (ctx->state[3] + D) & 0xffffffff;
	ctx->state[4] = (ctx->state[4] + E) & 0xffffffff;
	ctx->state[5] = (ctx->state[5] + F) & 0xffffffff;
	ctx->state[6] = (ctx->state[6] + G) & 0xffffffff;
	ctx->state[7] = (ctx->state[7] + H) & 0xffffffff;
}
#endif

#if defined(ENABLE_SHA_384) || defined(ENABLE_SHA_512)
static U32 K512[160] = {
	0x428a2f98, 0xd728ae22, 0x71374491, 0x23ef65cd, 0xb5c0fbcf, 0xec4d3b2f, 0xe9b5dba5, 0x8189dbbc,
	0x3956c25b, 0xf348b538, 0x59f111f1, 0xb605d019, 0x923f82a4, 0xaf194f9b, 0xab1c5ed5, 0xda6d8118,
	0xd807aa98, 0xa3030242, 0x12835b01, 0x45706fbe, 0x243185be, 0x4ee4b28c, 0x550c7dc3, 0xd5ffb4e2,
	0x72be5d74, 0xf27b896f, 0x80deb1fe, 0x3b1696b1, 0x9bdc06a7, 0x25c71235, 0xc19bf174, 0xcf692694,
	0xe49b69c1, 0x9ef14ad2, 0xefbe4786, 0x384f25e3, 0x0fc19dc6, 0x8b8cd5b5, 0x240ca1cc, 0x77ac9c65,
	0x2de92c6f, 0x592b0275, 0x4a7484aa, 0x6ea6e483, 0x5cb0a9dc, 0xbd41fbd4, 0x76f988da, 0x831153b5,
	0x983e5152, 0xee66dfab, 0xa831c66d, 0x2db43210, 0xb00327c8, 0x98fb213f, 0xbf597fc7, 0xbeef0ee4,
	0xc6e00bf3, 0x3da88fc2, 0xd5a79147, 0x930aa725, 0x06ca6351, 0xe003826f, 0x14292967, 0x0a0e6e70,
	0x27b70a85, 0x46d22ffc, 0x2e1b2138, 0x5c26c926, 0x4d2c6dfc, 0x5ac42aed, 0x53380d13, 0x9d95b3df,
	0x650a7354, 0x8baf63de, 0x766a0abb, 0x3c77b2a8, 0x81c2c92e, 0x47edaee6, 0x92722c85, 0x1482353b,
	0xa2bfe8a1, 0x4cf10364, 0xa81a664b, 0xbc423001, 0xc24b8b70, 0xd0f89791, 0xc76c51a3, 0x0654be30,
	0xd192e819, 0xd6ef5218, 0xd6990624, 0x5565a910, 0xf40e3585, 0x5771202a, 0x106aa070, 0x32bbd1b8,
	0x19a4c116, 0xb8d2d0c8, 0x1e376c08, 0x5141ab53, 0x2748774c, 0xdf8eeb99, 0x34b0bcb5, 0xe19b48a8,
	0x391c0cb3, 0xc5c95a63, 0x4ed8aa4a, 0xe3418acb, 0x5b9cca4f, 0x7763e373, 0x682e6ff3, 0xd6b2b8a3,
	0x748f82ee, 0x5defb2fc, 0x78a5636f, 0x43172f60, 0x84c87814, 0xa1f0ab72, 0x8cc70208, 0x1a6439ec,
	0x90befffa, 0x23631e28, 0xa4506ceb, 0xde82bde9, 0xbef9a3f7, 0xb2c67915, 0xc67178f2, 0xe372532b,
	0xca273ece, 0xea26619c, 0xd186b8c7, 0x21c0c207, 0xeada7dd6, 0xcde0eb1e, 0xf57d4f7f, 0xee6ed178,
	0x06f067aa, 0x72176fba, 0x0a637dc5, 0xa2c898a6, 0x113f9804, 0xbef90dae, 0x1b710b35, 0x131c471b,
	0x28db77f5, 0x23047d84, 0x32caab7b, 0x40c72493, 0x3c9ebe0a, 0x15c9bebc, 0x431d67c4, 0x9c100d4c,
	0x4cc5d4be, 0xcb3e42b6, 0x597f299c, 0xfc657e2a, 0x5fcb6fab, 0x3ad6faec, 0x6c44198c, 0x4a475817
};

static void SHA512_Round(SHA2_CTX *ctx, U32 *X)
{
	U32	Ah, Bh, Ch, Dh, Eh, Fh, Gh, Hh, T1h, WW[160];
	U32	Al, Bl, Cl, Dl, El, Fl, Gl, Hl, T1l;
	U32	i;

	for (i = 0; i<16; i++) {
		WW[2 * i] = X[2 * i]; WW[2 * i + 1] = X[2 * i + 1];
	}

	for (i = 16; i<80; i++) {
		T1h = sigma512_1h(WW[2 * (i - 2)], WW[2 * (i - 2) + 1]); T1l = sigma512_1l(WW[2 * (i - 2)], WW[2 * (i - 2) + 1]);
		ADDC(T1h, T1l, WW[2 * (i - 7)], WW[2 * (i - 7) + 1]);
		ADDC(T1h, T1l, sigma512_0h(WW[2 * (i - 15)], WW[2 * (i - 15) + 1]), sigma512_0l(WW[2 * (i - 15)], WW[2 * (i - 15) + 1]));
		ADDC(T1h, T1l, WW[2 * (i - 16)], WW[2 * (i - 16) + 1]);
		WW[2 * i] = T1h; WW[2 * i + 1] = T1l;
	}

	Ah = ctx->state[0];  Al = ctx->state[1];
	Bh = ctx->state[2];  Bl = ctx->state[3];
	Ch = ctx->state[4];  Cl = ctx->state[5];
	Dh = ctx->state[6];  Dl = ctx->state[7];
	Eh = ctx->state[8];  El = ctx->state[9];
	Fh = ctx->state[10]; Fl = ctx->state[11];
	Gh = ctx->state[12]; Gl = ctx->state[13];
	Hh = ctx->state[14]; Hl = ctx->state[15];

	for (i = 0; i<80; i += 8)
	{
		F512(A, B, C, D, E, F, G, H, i + 0);
		F512(H, A, B, C, D, E, F, G, i + 1);
		F512(G, H, A, B, C, D, E, F, i + 2);
		F512(F, G, H, A, B, C, D, E, i + 3);
		F512(E, F, G, H, A, B, C, D, i + 4);
		F512(D, E, F, G, H, A, B, C, i + 5);
		F512(C, D, E, F, G, H, A, B, i + 6);
		F512(B, C, D, E, F, G, H, A, i + 7);
	}

	ADDC(ctx->state[0], ctx->state[1], Ah, Al);
	ADDC(ctx->state[2], ctx->state[3], Bh, Bl);
	ADDC(ctx->state[4], ctx->state[5], Ch, Cl);
	ADDC(ctx->state[6], ctx->state[7], Dh, Dl);
	ADDC(ctx->state[8], ctx->state[9], Eh, El);
	ADDC(ctx->state[10], ctx->state[11], Fh, Fl);
	ADDC(ctx->state[12], ctx->state[13], Gh, Gl);
	ADDC(ctx->state[14], ctx->state[15], Hh, Hl);
}
#endif

/************************************************************************/
/* SHA-224                                                              */
/************************************************************************/
#ifdef ENABLE_SHA_224
static void E_SHA224_Init(SHA2_CTX *ctx)
{
	ctx->state[0] = INIT224_H0;
	ctx->state[1] = INIT224_H1;
	ctx->state[2] = INIT224_H2;
	ctx->state[3] = INIT224_H3;
	ctx->state[4] = INIT224_H4;
	ctx->state[5] = INIT224_H5;
	ctx->state[6] = INIT224_H6;
	ctx->state[7] = INIT224_H7;
	ctx->bits[0] = ctx->bits[1] = 0;
	ctx->hashbitlen = 224;
}

static void E_SHA224_Update(SHA2_CTX *ctx, U8 *in, int inbytes)
{
	U32 tmp, t, l1, l2, *ip, l = inbytes;
	U8 *tp;

	if (l == 0)
		return;

	ip = ctx->input;

	tmp = ctx->bits[0];
	if ((ctx->bits[0] = tmp + ((U32)l << 3)) < tmp)
		ctx->bits[1] ++;
	ctx->bits[1] += l >> 29;

	t = (int)(tmp >> 3) & 0x3f;

	if (t)
	{
		tp = (U8 *)ip + t;

		t = 64 - t;
		if (l < t)
		{
			memcpy(tp, in, l);
			return;
		}
		memcpy(tp, in, t);
#ifndef _BIG_ENDIAN
		Reverse(16, ip);
#endif
		SHA256_Round(ctx, ip);
		in += t;
		l -= t;
	}

	while (l >= 64)
	{
		memcpy(ip, in, 64);
#ifndef _BIG_ENDIAN
		Reverse(16, ip);
#endif
		SHA256_Round(ctx, ip);
		in += 64;
		l -= 64;
	}
	memcpy(ip, in, l);
}

static int E_SHA224_Final(SHA2_CTX *ctx, U8 *out)
{
	int i;
	U8 *tp;
	U32 l1, l2, *ip;

	i = (int)ctx->bits[0];
	i = (i >> 3) & 0x3f;

	ip = ctx->input;
	tp = (U8 *)ip + i;
	*tp++ = 0x80;

	i = 64 - 1 - i;

	if (i < 8)
	{
		memset(tp, 0, i);
#ifndef _BIG_ENDIAN
		Reverse(16, ip);
#endif
		SHA256_Round(ctx, ip);

		memset(ip, 0, 64 - 8);
	}
	else
	{
		memset(tp, 0, i - 8);
#ifndef _BIG_ENDIAN
		Reverse(14, ip);
#endif
	}

	ip[14] = ctx->bits[1];
	ip[15] = ctx->bits[0];

	SHA256_Round(ctx, ip);
#ifndef _BIG_ENDIAN
	Reverse(8, ctx->state);
#endif
	memcpy(out, ctx->state, 28);

	return ctx->hashbitlen / 8;
}

static int E_SHA224(U8 *out, U8 *in, int inbytes)
{
	SHA2_CTX ctx;

	E_SHA224_Init(&ctx);
	E_SHA224_Update(&ctx, in, inbytes);
	return E_SHA224_Final(&ctx, out);
}
#endif

/************************************************************************/
/* SHA-256                                                              */
/************************************************************************/
#ifdef ENABLE_SHA_256
static void E_SHA256_Init(SHA2_CTX *ctx)
{
	ctx->state[0] = INIT256_H0;
	ctx->state[1] = INIT256_H1;
	ctx->state[2] = INIT256_H2;
	ctx->state[3] = INIT256_H3;
	ctx->state[4] = INIT256_H4;
	ctx->state[5] = INIT256_H5;
	ctx->state[6] = INIT256_H6;
	ctx->state[7] = INIT256_H7;
	ctx->bits[0] = ctx->bits[1] = 0;
	ctx->hashbitlen = 256;
}

static void E_SHA256_Update(SHA2_CTX *ctx, U8 *in, int inbytes)
{
	U32 tmp, t, l1, l2, *ip, l = inbytes;
	U8 *tp;

	if (l == 0)
		return;

	ip = ctx->input;

	tmp = ctx->bits[0];
	if ((ctx->bits[0] = tmp + ((U32)l << 3)) < tmp)
		ctx->bits[1] ++;
	ctx->bits[1] += l >> 29;

	t = (int)(tmp >> 3) & 0x3f;

	if (t)
	{
		tp = (U8 *)ip + t;

		t = 64 - t;
		if (l < t)
		{
			memcpy(tp, in, l);
			return;
		}
		memcpy(tp, in, t);
#ifndef _BIG_ENDIAN
		Reverse(16, ip);
#endif
		SHA256_Round(ctx, ip);
		in += t;
		l -= t;
	}

	while (l >= 64)
	{
		memcpy(ip, in, 64);
#ifndef _BIG_ENDIAN
		Reverse(16, ip);
#endif
		SHA256_Round(ctx, ip);
		in += 64;
		l -= 64;
	}
	memcpy(ip, in, l);
}

static int E_SHA256_Final(SHA2_CTX *ctx, U8 *out)
{
	int i;
	U8 *tp;
	U32 l1, l2, *ip;

	i = (int)ctx->bits[0];
	i = (i >> 3) & 0x3f;

	ip = ctx->input;
	tp = (U8 *)ip + i;
	*tp++ = 0x80;

	i = 64 - 1 - i;

	if (i < 8)
	{
		memset(tp, 0, i);
#ifndef _BIG_ENDIAN
		Reverse(16, ip);
#endif
		SHA256_Round(ctx, ip);

		memset(ip, 0, 64 - 8);
	}
	else
	{
		memset(tp, 0, i - 8);
#ifndef _BIG_ENDIAN
		Reverse(14, ip);
#endif
	}

	ip[14] = ctx->bits[1];
	ip[15] = ctx->bits[0];

	SHA256_Round(ctx, ip);
#ifndef _BIG_ENDIAN
	Reverse(8, ctx->state);
#endif
	memcpy(out, ctx->state, 32);

	return ctx->hashbitlen / 8;
}

int E_SHA256(U8 *out, U8 *in, int inbytes)
{
	SHA2_CTX ctx;

	E_SHA256_Init(&ctx);
	E_SHA256_Update(&ctx, in, inbytes);
	return E_SHA256_Final(&ctx, out);
}
#endif

/************************************************************************/
/* SHA-384                                                              */
/************************************************************************/
#ifdef ENABLE_SHA_384
static void E_SHA384_Init(SHA2_CTX *ctx)
{
	ctx->state[0] = INIT384_H0h; ctx->state[1] = INIT384_H0l;
	ctx->state[2] = INIT384_H1h; ctx->state[3] = INIT384_H1l;
	ctx->state[4] = INIT384_H2h; ctx->state[5] = INIT384_H2l;
	ctx->state[6] = INIT384_H3h; ctx->state[7] = INIT384_H3l;
	ctx->state[8] = INIT384_H4h; ctx->state[9] = INIT384_H4l;
	ctx->state[10] = INIT384_H5h; ctx->state[11] = INIT384_H5l;
	ctx->state[12] = INIT384_H6h; ctx->state[13] = INIT384_H6l;
	ctx->state[14] = INIT384_H7h; ctx->state[15] = INIT384_H7l;
	ctx->bits[0] = ctx->bits[1] = ctx->bits[2] = ctx->bits[3] = 0;
	ctx->hashbitlen = 384;
}

static void E_SHA384_Update(SHA2_CTX *ctx, U8 *in, int inbytes)
{
	U32 tmp, t, l1, l2, *ip, l = inbytes;
	U8 *tp;

	if (l == 0)
		return;

	ip = ctx->input;

	tmp = ctx->bits[0];
	if ((ctx->bits[0] = tmp + ((U32)l << 3)) < tmp)
		if (ctx->bits[1]++ == 0)
			if (ctx->bits[2]++ == 0)
				ctx->bits[3]++;
	if ((ctx->bits[1] += l >> 29) < (l >> 29))
		if (ctx->bits[2]++ == 0)
			ctx->bits[3]++;

	t = (int)(tmp >> 3) & 0x7f;

	if (t)
	{
		tp = (U8 *)ip + t;

		t = 128 - t;
		if (l < t)
		{
			memcpy(tp, in, l);
			return;
		}
		memcpy(tp, in, t);
#ifndef _BIG_ENDIAN
		Reverse(32, ip);
#endif
		SHA512_Round(ctx, ip);
		in += t;
		l -= t;
	}

	while (l >= 128)
	{
		memcpy(ip, in, 128);
#ifndef _BIG_ENDIAN
		Reverse(32, ip);
#endif
		SHA512_Round(ctx, ip);
		in += 128;
		l -= 128;
	}
	memcpy(ip, in, l);
}

static int E_SHA384_Final(SHA2_CTX *ctx, U8 *out)
{
	int i;
	U8 *tp;
	U32 l1, l2, *ip;

	i = (int)ctx->bits[0];
	i = (i >> 3) & 0x7f;

	ip = ctx->input;
	tp = (U8 *)ip + i;
	*tp++ = 0x80;

	i = 128 - 1 - i;

	if (i < 16)
	{
		memset(tp, 0, i);
#ifndef _BIG_ENDIAN
		Reverse(32, ip);
#endif
		SHA512_Round(ctx, ip);

		memset(ip, 0, 128 - 16);
	}
	else
	{
		memset(tp, 0, i - 16);
#ifndef _BIG_ENDIAN
		Reverse(28, ip);
#endif
	}

	ip[28] = ctx->bits[3];
	ip[29] = ctx->bits[2];
	ip[30] = ctx->bits[1];
	ip[31] = ctx->bits[0];

	SHA512_Round(ctx, ip);
#ifndef _BIG_ENDIAN
	Reverse(16, ctx->state);
#endif
	memcpy(out, ctx->state, 48);

	return ctx->hashbitlen / 8;
}

static int E_SHA384(U8 *out, U8 *in, int inbytes)
{
	SHA2_CTX ctx;

	E_SHA384_Init(&ctx);
	E_SHA384_Update(&ctx, in, inbytes);
	return E_SHA384_Final(&ctx, out);
}
#endif

/************************************************************************/
/* SHA-512                                                              */
/************************************************************************/
#ifdef ENABLE_SHA_512
static void E_SHA512_Init(SHA2_CTX *ctx)
{
	ctx->state[0] = INIT512_H0h; ctx->state[1] = INIT512_H0l;
	ctx->state[2] = INIT512_H1h; ctx->state[3] = INIT512_H1l;
	ctx->state[4] = INIT512_H2h; ctx->state[5] = INIT512_H2l;
	ctx->state[6] = INIT512_H3h; ctx->state[7] = INIT512_H3l;
	ctx->state[8] = INIT512_H4h; ctx->state[9] = INIT512_H4l;
	ctx->state[10] = INIT512_H5h; ctx->state[11] = INIT512_H5l;
	ctx->state[12] = INIT512_H6h; ctx->state[13] = INIT512_H6l;
	ctx->state[14] = INIT512_H7h; ctx->state[15] = INIT512_H7l;
	ctx->bits[0] = ctx->bits[1] = ctx->bits[2] = ctx->bits[3] = 0;
	ctx->hashbitlen = 512;
}

static void E_SHA512_Update(SHA2_CTX *ctx, U8 *in, int inbytes)
{
	U32 tmp, t, l1, l2, *ip, l = inbytes;
	U8 *tp;

	if (l == 0)
		return;

	ip = ctx->input;

	tmp = ctx->bits[0];
	if ((ctx->bits[0] = tmp + ((U32)l << 3)) < tmp)
		if (ctx->bits[1]++ == 0)
			if (ctx->bits[2]++ == 0)
				ctx->bits[3]++;
	if ((ctx->bits[1] += l >> 29) < (l >> 29))
		if (ctx->bits[2]++ == 0)
			ctx->bits[3]++;

	t = (int)(tmp >> 3) & 0x7f;

	if (t)
	{
		tp = (U8 *)ip + t;

		t = 128 - t;
		if (l < t)
		{
			memcpy(tp, in, l);
			return;
		}
		memcpy(tp, in, t);
#ifndef _BIG_ENDIAN
		Reverse(32, ip);
#endif
		SHA512_Round(ctx, ip);
		in += t;
		l -= t;
	}

	while (l >= 128)
	{
		memcpy(ip, in, 128);
#ifndef _BIG_ENDIAN
		Reverse(32, ip);
#endif
		SHA512_Round(ctx, ip);
		in += 128;
		l -= 128;
	}
	memcpy(ip, in, l);
}

static int E_SHA512_Final(SHA2_CTX *ctx, U8 *out)
{
	int i;
	U8 *tp;
	U32 l1, l2, *ip;

	i = (int)ctx->bits[0];
	i = (i >> 3) & 0x7f;

	ip = ctx->input;
	tp = (U8 *)ip + i;
	*tp++ = 0x80;

	i = 128 - 1 - i;

	if (i < 16)
	{
		memset(tp, 0, i);
#ifndef _BIG_ENDIAN
		Reverse(32, ip);
#endif
		SHA512_Round(ctx, ip);

		memset(ip, 0, 128 - 16);
	}
	else
	{
		memset(tp, 0, i - 16);
#ifndef _BIG_ENDIAN
		Reverse(28, ip);
#endif
	}

	ip[28] = ctx->bits[3];
	ip[29] = ctx->bits[2];
	ip[30] = ctx->bits[1];
	ip[31] = ctx->bits[0];

	SHA512_Round(ctx, ip);
#ifndef _BIG_ENDIAN
	Reverse(16, ctx->state);
#endif
	memcpy(out, ctx->state, 64);

	return ctx->hashbitlen / 8;
}

int E_SHA512(U8 *out, U8 *in, int inbytes)
{
	SHA2_CTX ctx;

	E_SHA512_Init(&ctx);
	E_SHA512_Update(&ctx, in, inbytes);
	return E_SHA512_Final(&ctx, out);
}
#endif

static void E_SHA2_Init(SHA2_CTX *ctx, int hashbitlen)
{
	switch (hashbitlen)
	{
#ifdef ENABLE_SHA_224
	case 224:	E_SHA224_Init(ctx);	break;
#endif
#ifdef ENABLE_SHA_256
	case 256:	E_SHA256_Init(ctx);	break;
#endif
#ifdef ENABLE_SHA_384
	case 384:	E_SHA384_Init(ctx);	break;
#endif
#ifdef ENABLE_SHA_512
	case 512:	E_SHA512_Init(ctx);	break;
#endif
	default:	E_SHA256_Init(ctx);	break;
	}
}

static void E_SHA2_Update(SHA2_CTX *ctx, U8 *in, int inbytes)
{
	switch (ctx->hashbitlen)
	{
#ifdef ENABLE_SHA_224
	case 224:	E_SHA224_Update(ctx, in, inbytes);	break;
#endif
#ifdef ENABLE_SHA_256
	case 256:	E_SHA256_Update(ctx, in, inbytes);	break;
#endif
#ifdef ENABLE_SHA_384
	case 384:	E_SHA384_Update(ctx, in, inbytes);	break;
#endif
#ifdef ENABLE_SHA_512
	case 512:	E_SHA512_Update(ctx, in, inbytes);	break;
#endif
	default:	E_SHA256_Update(ctx, in, inbytes);	break;
	}
}

static int E_SHA2_Final(SHA2_CTX *ctx, U8 *out)
{
	switch (ctx->hashbitlen)
	{
#ifdef ENABLE_SHA_224
	case 224:	return E_SHA224_Final(ctx, out);
#endif
#ifdef ENABLE_SHA_256
	case 256:	return E_SHA256_Final(ctx, out);
#endif
#ifdef ENABLE_SHA_384
	case 384:	return E_SHA384_Final(ctx, out);
#endif
#ifdef ENABLE_SHA_512
	case 512:	return E_SHA512_Final(ctx, out);
#endif
	default:	return E_SHA256_Final(ctx, out);
	}
}

static int E_SHA2(U8 *out, int hashbitlen, U8 *in, int inbytes)
{
	SHA2_CTX ctx;

	E_SHA2_Init(&ctx, hashbitlen);
	E_SHA2_Update(&ctx, in, inbytes);
	return E_SHA2_Final(&ctx, out);
}

int HMAC_SHA2(int bit, U8 *out, U8 *key, int keybytes, U8 *in, int inbytes)
{
	SHA2_CTX ctx;
	U8 k[128], tmp[64], ipad[128], opad[128];
	int i, bs = 64;

	if (bit>256)
		bs = 128;

	if (keybytes > bs)
	{
		keybytes = E_SHA2(k, bit, key, keybytes);
	}
	else
		memcpy(k, key, keybytes);
	memset(&k[keybytes], 0, bs - keybytes);

	for (i = 0; i < bs; i++)
	{
		ipad[i] = k[i] ^ 0x36;
		opad[i] = k[i] ^ 0x5c;
	}

	E_SHA2_Init(&ctx, bit);
	E_SHA2_Update(&ctx, ipad, bs);
	E_SHA2_Update(&ctx, in, inbytes);
	i = E_SHA2_Final(&ctx, tmp);

	E_SHA2_Init(&ctx, bit);
	E_SHA2_Update(&ctx, opad, bs);
	E_SHA2_Update(&ctx, tmp, i);
	return E_SHA2_Final(&ctx, out);
}
