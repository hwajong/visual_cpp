#ifndef _AES_H_
#define _AES_H_

// Ÿ������
typedef unsigned int WORD;
typedef unsigned char BYTE;

// �������
#define Nb 4		// AES ��ϱ���(word)
#define Nk 4		// AES Ű ����(word)

void AES_Cipher(BYTE* in, BYTE* out, BYTE* key);			//AES ��ȣȭ
void AES_Inverse_Cipher(BYTE* in, BYTE* out, BYTE* key);	//AES ��ȣȭ
void SubBytes(BYTE state[][4]);								//SubBytes �Լ�
void ShiftRows(BYTE state[][4]);							//ShiftRows �Լ�
void MixColumns(BYTE state[][4]);							//MixColumns �Լ�
void Inv_SubBytes(BYTE state[][4]);							//Inverse SubBytes �Լ�
void Inv_ShiftRows(BYTE state[][4]);						//Inverse ShiftRows �Լ�
void Inv_MixColumns(BYTE state[][4]);						//Inverse MixColumns �Լ�
void AddRoundKey(BYTE state[][4], WORD*);					//AddRoundKey �Լ�
void KeyExpansion(BYTE* key, WORD* W);						//AES Ű Ȯ�� �Լ�
void CirShiftRows(BYTE* row);								//state�� �� ���� 1ȸ �����U���� ��ȯ ����Ʈ
void Inv_CirShiftRows(BYTE* row);							//state�� �� ���� 1ȸ �������� ��ȯ ����Ʈ
WORD SubWord(WORD W);										//SubWord
WORD RotWord(WORD W);										//RotWord
BYTE x_time(BYTE n, BYTE b);								//GF(2^8)�󿡼� ���� ���� �Լ�

#endif