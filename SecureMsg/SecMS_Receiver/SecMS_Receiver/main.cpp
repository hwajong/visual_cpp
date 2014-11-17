#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cassert>
#include "socket_for_win.h"
#include "hmac_sha2.h"
#include "aes.h"

using namespace std;

#define DEFAULT_BUFLEN	1024
#define DEFAULT_PORT	"7777"

const char* fname_key = "key.bin";
const char* fname_plain = "plain.txt";
const char* fname_decoded_plain = "decoded.txt";
const char* fname_rehash = "rehash.bin";

// files to be received.
const char* fname_cipher = "cipher.bin";
const char* fname_hash = "hash.bin";

void remove_old_files()
{
	remove(fname_key);
	remove(fname_plain);
	remove(fname_decoded_plain);
	remove(fname_rehash);
	remove(fname_cipher);
	remove(fname_hash);
}

// �迭 ��� for �����
void print_byte_array(const char* desc, const byte* A, int size)
{
	printf("*%s\n", desc);
	for (int i = 0; i<size; i++)
	{
		if (i != 0 && i % 16 == 0) printf("\n");

		printf(" 0x%02x", A[i]);
	}

	printf("\n\n");
}

// �����б� - ȣ���ϴ� ������ �޸� ������ �ؾ��Ѵ�.
byte* read_file(const char* fname, int* size, bool is_binary = false)
{
	FILE* fp = NULL;
	if (is_binary)  fp = fopen(fname, "rb");
	else fp = fopen(fname, "r");

	if (fp == NULL)
	{
		printf("**Error - can't open file : %s\n", fname);
		exit(0);
	}

	fseek(fp, 0L, SEEK_END);
	int fsize = ftell(fp);

	//	printf("*Data File Size : %dbytes\n\n", fsize);
	fseek(fp, 0L, SEEK_SET);

	byte* data = (byte*)malloc(fsize);
	memset(data, 0, fsize);

	int nread = fread(data, 1, fsize, fp);
	if (nread <= 0)
	{
		printf("**Error - can't read file : %s\n", fname);
		exit(0);
	}

	fclose(fp);

	*size = nread;
	return data;
}

// ���Ͼ��� 
void write_file(byte* data, int size, const char* fname, bool is_binary = false)
{
	FILE* fp = NULL;
	if (is_binary)  fp = fopen(fname, "wb");
	else fp = fopen(fname, "w");

	int nwrite = fwrite(data, 1, size, fp);
	if (nwrite != size)
	{
		printf("**Error - can't write file : data_ww.txt\n");
		exit(0);
	}

	fclose(fp);
}

// HMAC_SHA2 �ؽð� ���� �� ���� write
void make_hash(const char* fname_key, const char* fname_in, const char* fname_out)
{
	int key_size = 0;
	byte* key = read_file(fname_key, &key_size, true);

	int fsize = 0;
	byte* data = read_file(fname_in, &fsize);

	byte hash[32] = { 0, }; // 256bit
	int ret = HMAC_SHA2(256, hash, key, 16, data, fsize);

	print_byte_array("HMAC_SHA2", hash, 32);

	write_file(hash, 32, fname_out, true);

	free(key);
	free(data);
}

// 1234�� srand �̿� 128bit Ű���� --> text file ����
void make_128bit_key(const char* fname_key)
{
	int user_key = 0;
	printf("*Ű���� �����ϱ� ��й�ȣ�� �Է��Ͻÿ� : ");
	scanf("%d", &user_key);

	printf("*user_key : %d\n", user_key);

	srand(user_key);

	// rand() �� 0 ~ 7FFF������ ��(2byte)�� �����ϹǷ� 
	// 128bit(16byte) Ű���� ����� ���� 8ȸ �����Ѵ�.
	byte key[16] = { 0, };
	for (int i = 0; i < 8; i++)
	{
		int n = rand();
		memcpy(key + i * 2, &n, 2);
		//		printf("n : %d\n", n);
	}

	print_byte_array("������ 128bit key ��", key, 16);

	write_file(key, 16, fname_key, true);
}

// fname_in ������ �о� AES ��ȣȭ�� �� fname_out ���Ͽ� �����Ѵ�.
void aes_encrypt(const char* fname_key, const char* fname_in, const char* fname_out)
{
	int key_size = 0;
	byte* key = read_file(fname_key, &key_size, true);

	int msg_len = 0;
	byte* p_text_org = read_file(fname_in, &msg_len);
	int padding_len = 16 - (msg_len % 16);
	int cipher_len = msg_len + padding_len;

	byte* p_text = (byte*)malloc(cipher_len);
	memcpy(p_text, p_text_org, msg_len);

	// �е� ó��
	memset(p_text + msg_len, padding_len, padding_len);
	//	print_byte_array("p_text with padding", p_text, cipher_len);

	byte* c_text = (byte*)malloc(cipher_len);
	memset(c_text, 0, cipher_len);

	// ��ϼ� ���
	int block_count = (msg_len % (Nb * 4)) ? (msg_len / (Nb * 4) + 1) : (msg_len / (Nb * 4));

	printf("*msg_len : %d\n", msg_len);
	printf("*block_count : %d\n", block_count);
	printf("*padding_len : %d\n", padding_len);
	printf("*cipher_len : %d\n", cipher_len);

	for (int i = 0; i<block_count; i++)
	{
		AES_Cipher(&p_text[i*Nb * 4], &c_text[i*Nb * 4], key);	//��ȣȭ
	}

	print_byte_array("cipher", c_text, cipher_len);

	//	printf("\n��ȣ��: ");
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

// fnmae_cipher ������ �о� AES ��ȣȭ�� �� fname_decoded_plain ���Ͽ� �����Ѵ�.
void aes_decrypt(const char* fname_key, const char* fnmae_cipher, const char* fname_decoded_plain)
{
	int key_size = 0;
	byte* key = read_file(fname_key, &key_size, true);

	int msg_len = 0;
	byte* c_text = read_file(fnmae_cipher, &msg_len, true);
	assert(msg_len % 16 == 0);

	print_byte_array("read cipher", c_text, msg_len);

	byte* inv_c_text = (byte*)malloc(msg_len);
	memset(inv_c_text, 0, msg_len);

	// ��ϼ� ���
	int block_count = (msg_len % (Nb * 4)) ? (msg_len / (Nb * 4) + 1) : (msg_len / (Nb * 4));

	printf("*msg_len : %d\n", msg_len);
	printf("*block_count : %d\n", block_count);

	for (int i = 0; i<block_count; i++)
	{
		AES_Inverse_Cipher(&c_text[i*Nb * 4], &inv_c_text[i*Nb * 4], key); //��ȣȭ
	}

	int padding_len = inv_c_text[msg_len - 1];
	printf("*padding_len : %d\n", padding_len);

	printf("\n------------------------------\n");
	printf("\nDecryted Message: ");
	for(int i=0; i<msg_len-padding_len; i++)
		printf("%c", inv_c_text[i]);
	printf("\n------------------------------\n");

	write_file(inv_c_text, msg_len - padding_len, fname_decoded_plain);

	printf("*Decryption ... Success! --> %s\n\n", fname_decoded_plain);

	free(key);
	free(inv_c_text);
	free(c_text);
}

// Receive until the peer shuts down the connection
void recv_file(SOCKET socket_client, const char* fname)
{
	FILE* fp = fopen(fname, "wb");

	do
	{
		char recvbuf[DEFAULT_BUFLEN] = { 0, };
		int nrecv = recv(socket_client, recvbuf, sizeof(recvbuf), 0);
		if (nrecv < 0)
		{
			print_sys_error("recv failed");
			shutdown_connection(socket_client);
			closesocket(socket_client);
			WSACleanup();
			exit(1);
		}

		if (nrecv == 0)
		{
			//printf("*Connection closing...\n");
			break;
		}

		if (nrecv > 0)
		{
			printf("*Bytes received: %d\n", nrecv);
			print_byte_array("recv data", (const byte*)recvbuf, nrecv);

			int nwrite = fwrite(recvbuf, 1, nrecv, fp);
			if (nwrite != nrecv)
			{
				printf("**Error - can't write file : %s\n", fname);
				shutdown_connection(socket_client);
				closesocket(socket_client);
				WSACleanup();
				exit(1);
			}
		}
	} while (true);

	fclose(fp);
}

void compare_hash(const char* fname_hash_recv, const char* fname_hash_make)
{
	int size_hash_recv = 0;
	byte* hash_recv = read_file(fname_hash_recv, &size_hash_recv, true);

	int size_hash_make = 0;
	byte* hash_make = read_file(fname_hash_make, &size_hash_make, true);

	if (size_hash_recv != size_hash_make)
	{
		printf("** Error : hash values are not identical! - size(%d/%d)\n", size_hash_recv, size_hash_make);
		free(hash_recv);
		free(hash_make);
		return;
	}

	if (memcmp(hash_recv, hash_make, size_hash_recv) != 0)
	{
		printf("** Error : hash values are not identical!\n");
		free(hash_recv);
		free(hash_make);
		return;
	}

	printf("*Success - hash values are identical!\n");

	free(hash_recv);
	free(hash_make);
}

// TODO ������, ��Ʈ �Ķ���� ó��
void main()
{
	remove_old_files();

	init_winsock();

	addrinfo* addr_server = get_server_addr_info(NULL, DEFAULT_PORT);

	SOCKET socket_listen = create_socket(addr_server);

	bind_socket(socket_listen, addr_server);

	freeaddrinfo(addr_server);

	listen_socket(socket_listen);

	printf("*Server started!\n");
	printf("*Now wait for message ...\n");

	SOCKET socket_client = accept_socket(socket_listen);
	printf("*Connected!\n");

	recv_file(socket_client, fname_cipher);
	printf("*Success to recv file - %s\n", fname_cipher);
	shutdown_connection(socket_client);

	socket_client = accept_socket(socket_listen);
	printf("*Re-connected!\n");

	recv_file(socket_client, fname_hash);
	printf("*Success to recv file - %s\n", fname_hash);
	shutdown_connection(socket_client);

	closesocket(socket_listen);
	WSACleanup();

	// Decryption !!
	make_128bit_key(fname_key);

	// AES ��ȣȭ 
	aes_decrypt(fname_key, fname_cipher, fname_decoded_plain);

	// �ؽð� ����
	make_hash(fname_key, fname_decoded_plain, fname_rehash);

	// hash �� ��
	compare_hash(fname_hash, fname_rehash);

}