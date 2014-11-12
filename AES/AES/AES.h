#ifndef _AES_H_
#define _AES_H_

// 타입정의
typedef unsigned int WORD;
typedef unsigned char BYTE;

// 상수정의
#define Nb 4		// AES 블록길이(word)
#define Nk 4		// AES 키 길이(word)

void AES_Cipher(BYTE* in, BYTE* out, BYTE* key);			//AES 암호화
void AES_Inverse_Cipher(BYTE* in, BYTE* out, BYTE* key);	//AES 복호화
void SubBytes(BYTE state[][4]);								//SubBytes 함수
void ShiftRows(BYTE state[][4]);							//ShiftRows 함수
void MixColumns(BYTE state[][4]);							//MixColumns 함수
void Inv_SubBytes(BYTE state[][4]);							//Inverse SubBytes 함수
void Inv_ShiftRows(BYTE state[][4]);						//Inverse ShiftRows 함수
void Inv_MixColumns(BYTE state[][4]);						//Inverse MixColumns 함수
void AddRoundKey(BYTE state[][4], WORD*);					//AddRoundKey 함수
void KeyExpansion(BYTE* key, WORD* W);						//AES 키 확장 함수
void CirShiftRows(BYTE* row);								//state의 한 행을 1회 오른쪾으로 순환 시프트
void Inv_CirShiftRows(BYTE* row);							//state의 한 행을 1회 왼쪽으로 순환 시프트
WORD SubWord(WORD W);										//SubWord
WORD RotWord(WORD W);										//RotWord
BYTE x_time(BYTE n, BYTE b);								//GF(2^8)상에서 곱셈 연산 함수

#endif