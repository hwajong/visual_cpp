#ifndef _AES_H_
#define _AES_H_

// 타입정의
typedef unsigned int uint;
typedef unsigned char byte;

// 상수정의
#define Nb 4		// AES 블록길이(word)
#define Nk 4		// AES 키 길이(word)

void AES_Cipher(byte* in, byte* out, byte* key);			//AES 암호화
void AES_Inverse_Cipher(byte* in, byte* out, byte* key);	//AES 복호화
void SubBytes(byte state[][4]);								//SubBytes 함수
void ShiftRows(byte state[][4]);							//ShiftRows 함수
void MixColumns(byte state[][4]);							//MixColumns 함수
void Inv_SubBytes(byte state[][4]);							//Inverse SubBytes 함수
void Inv_ShiftRows(byte state[][4]);						//Inverse ShiftRows 함수
void Inv_MixColumns(byte state[][4]);						//Inverse MixColumns 함수
void AddRoundKey(byte state[][4], uint*);					//AddRoundKey 함수
void KeyExpansion(byte* key, uint* W);						//AES 키 확장 함수
void CirShiftRows(byte* row);								//state의 한 행을 1회 오른쪾으로 순환 시프트
void Inv_CirShiftRows(byte* row);							//state의 한 행을 1회 왼쪽으로 순환 시프트
uint SubWord(uint W);										//SubWord
uint RotWord(uint W);										//RotWord
byte x_time(byte n, byte b);								//GF(2^8)상에서 곱셈 연산 함수

#endif