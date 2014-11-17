#include <iostream>
#include <cassert>
#include "HMAC_SHA2.h"
#include "AES.h"


using namespace std;

// 배열 출력 for 디버깅
void print_byte_array(const char* desc, const BYTE* A, int size)
{
	printf("*%s\n", desc);
	for(int i=0; i<size; i++) 
	{
		if(i != 0 && i % 16 == 0) printf("\n");

		printf(" 0x%02x", A[i]);
	}

	printf("\n\n");
}

// 파일읽기 - 호출하는 곳에서 메모리 해제를 해야한다.
BYTE* read_file(const char* fname, int* size, bool is_binary=false)
{
	FILE* fp = NULL;
	if(is_binary)  fp = fopen(fname, "rb");
	else fp = fopen(fname, "r");

	if(fp == NULL)
	{
		printf("**Error - can't open file : %s\n", fname);
		exit(0);
	}

	fseek(fp, 0L, SEEK_END);
	int fsize = ftell(fp);

//	printf("*Data File Size : %dbytes\n\n", fsize);
	fseek(fp, 0L, SEEK_SET);

	BYTE* data = (BYTE*)malloc(fsize);
	memset(data, 0, fsize);

	int nread = fread(data, 1, fsize, fp);
	if(nread <= 0)
	{
		printf("**Error - can't read file : %s\n", fname);
		exit(0);
	}

	fclose(fp);

	*size = nread;
	return data;
}

// 파일쓰기 
void write_file(BYTE* data, int size, const char* fname, bool is_binary=false)
{
	FILE* fp = NULL;
	if(is_binary)  fp = fopen(fname, "wb");
	else fp = fopen(fname, "w");

	int nwrite = fwrite(data, 1, size, fp);
	if(nwrite != size)
	{
		printf("**Error - can't write file : data_ww.txt\n");
		exit(0);
	}

	fclose(fp);
}

// HMAC_SHA2 해시값 생성 후 파일 write
void make_hash(const char* fname_key, const char* fname_in, const char* fname_out)
{
	int key_size = 0;
	BYTE* key = read_file(fname_key, &key_size, true);

	int fsize = 0;
	BYTE* data = read_file(fname_in, &fsize);

	BYTE hash[32] = {0,}; // 256bit
	int ret = HMAC_SHA2(256, hash, key, 16, data, fsize);

	print_byte_array("HMAC_SHA2", hash, 32);

	write_file(hash, 32, fname_out, true);

	free(key);
	free(data);
}

// 1234로 srand 이용 128bit 키생성 --> text file 저장
void make_128bit_key(const char* fname_key)
{
	int user_key = 0;
	printf("*키값을 생성하기 비밀번호를 입력하시요 : ");
	scanf("%d", &user_key);

	printf("*user_key : %d\n", user_key);

	srand(user_key);

	// rand() 가 0 ~ 7FFF까지의 값(2byte)을 생성하므로 
	// 128bit(16byte) 키값을 만들기 위해 8회 생성한다.
	BYTE key[16] = {0,};
	for(int i = 0; i < 8; i++) 
	{
		int n = rand();
		memcpy(key + i*2, &n, 2);
//		printf("n : %d\n", n);
	}

	print_byte_array("생성된 128bit key 값", key, 16);

	write_file(key, 16, fname_key, true);
}

// fname_in 파일을 읽어 AES 암호화한 후 fname_out 파일에 저장한다.
void aes_encrypt(const char* fname_key, const char* fname_in, const char* fname_out)
{
	int key_size = 0;
	BYTE* key = read_file(fname_key, &key_size, true);

	int msg_len = 0;
	BYTE* p_text_org = read_file(fname_in, &msg_len);
	int padding_len = 16 - (msg_len % 16);
	int cipher_len = msg_len + padding_len;

	BYTE* p_text = (BYTE*)malloc(cipher_len);
	memcpy(p_text, p_text_org, msg_len);

	// 패딩 처리
	memset(p_text+msg_len, padding_len, padding_len);
//	print_byte_array("p_text with padding", p_text, cipher_len);
	
	BYTE* c_text = (BYTE*)malloc(cipher_len); 
	memset(c_text, 0, cipher_len);

	// 블록수 계산
	int block_count = (msg_len % (Nb*4)) ? (msg_len / (Nb*4) + 1) : (msg_len / (Nb*4));

	printf("*msg_len : %d\n", msg_len);
	printf("*block_count : %d\n", block_count);
	printf("*padding_len : %d\n", padding_len);
	printf("*cipher_len : %d\n", cipher_len);

	for(int i=0; i<block_count; i++)
	{
		AES_Cipher(&p_text[i*Nb*4], &c_text[i*Nb*4], key);	//암호화
	}

	print_byte_array("cipher", c_text, cipher_len);

//	printf("\n암호문: ");
//	for(int i=0; i<block_count*Nb*4; i++) printf("%c", c_text[i]);
//	printf("\n");

	write_file(c_text, cipher_len, fname_out, true);

	printf("*Encryption ... Success! --> %s\n\n", fname_out);

//	print_byte_array("cipher", c_text, cipher_len);

	free(key);
	free(c_text);
	free(p_text);
	free(p_text_org);
}

// fnmae_cipher 파일을 읽어 AES 복호화한 후 fname_decoded_plain 파일에 저장한다.
void aes_decrypt(const char* fname_key, const char* fnmae_cipher, const char* fname_decoded_plain)
{
	int key_size = 0;
	BYTE* key = read_file(fname_key, &key_size, true);

	int msg_len = 0;
	BYTE* c_text = read_file(fnmae_cipher, &msg_len, true);
	assert(msg_len % 16 == 0);
	
	print_byte_array("read cipher", c_text, msg_len);

	BYTE* inv_c_text = (BYTE*)malloc(msg_len);
	memset(inv_c_text, 0, msg_len);

	// 블록수 계산
	int block_count = (msg_len % (Nb*4)) ? (msg_len / (Nb*4) + 1) : (msg_len / (Nb*4));

	printf("*msg_len : %d\n", msg_len);
	printf("*block_count : %d\n", block_count);

	for(int i=0; i<block_count; i++)
	{
		AES_Inverse_Cipher(&c_text[i*Nb*4], &inv_c_text[i*Nb*4], key); //복호화
	}

//	printf("\n복호문: ");
//	for(int i=0; i<msg_len-padding_len; i++)
//		printf("%c", inv_c_text[i]);
//	printf("\n-------------\n");

	int padding_len = inv_c_text[msg_len-1];
	printf("*padding_len : %d\n", padding_len);

	write_file(inv_c_text, msg_len-padding_len, fname_decoded_plain);

	printf("*Decryption ... Success! --> %s\n\n", fname_decoded_plain);

	free(key);
	free(inv_c_text);
	free(c_text);
}

void main()
{
	const char* fname_key  = "key.bin";
	const char* fname_plain  = "plain.txt";
	const char* fname_cipher = "cipher.bin";
	const char* fname_decoded_plain = "decoded.txt";
	const char* fname_hash = "hash.bin";
	const char* fname_rehash = "rehash.bin";

	make_128bit_key(fname_key);

	// 해시값 생성
	make_hash(fname_key, fname_plain, fname_hash);

	// TODO 패딩값을 알아야 디코딩 할 수 있게 되어있는데 ...
	// PKCS7 의 표준 패딩방식을 적용할것.
	// http://en.wikipedia.org/wiki/Padding_(cryptography)#Bit_padding
	// AES 암호화
	aes_encrypt(fname_key, fname_plain, fname_cipher);

	// AES 복호화 
	aes_decrypt(fname_key, fname_cipher, fname_decoded_plain);

	// 해시값 생성
	make_hash(fname_key, fname_decoded_plain, fname_rehash);
}






