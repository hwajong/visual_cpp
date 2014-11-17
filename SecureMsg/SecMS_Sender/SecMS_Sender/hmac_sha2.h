#ifndef _HMAC_SHA2_H_
#define _HMAC_SHA2_H_

int HMAC_SHA2(int bit,
	unsigned char *out,
	unsigned char *key, int keybytes,
	unsigned char *in, int inbytes);

#endif
