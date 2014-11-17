#ifndef _AES_H_
#define _AES_H_

// Ÿ������
typedef unsigned int uint;
typedef unsigned char byte;

// �������
#define Nb 4		// AES ��ϱ���(word)
#define Nk 4		// AES Ű ����(word)

void AES_Cipher(byte* in, byte* out, byte* key);			//AES ��ȣȭ
void AES_Inverse_Cipher(byte* in, byte* out, byte* key);	//AES ��ȣȭ
void SubBytes(byte state[][4]);								//SubBytes �Լ�
void ShiftRows(byte state[][4]);							//ShiftRows �Լ�
void MixColumns(byte state[][4]);							//MixColumns �Լ�
void Inv_SubBytes(byte state[][4]);							//Inverse SubBytes �Լ�
void Inv_ShiftRows(byte state[][4]);						//Inverse ShiftRows �Լ�
void Inv_MixColumns(byte state[][4]);						//Inverse MixColumns �Լ�
void AddRoundKey(byte state[][4], uint*);					//AddRoundKey �Լ�
void KeyExpansion(byte* key, uint* W);						//AES Ű Ȯ�� �Լ�
void CirShiftRows(byte* row);								//state�� �� ���� 1ȸ �����U���� ��ȯ ����Ʈ
void Inv_CirShiftRows(byte* row);							//state�� �� ���� 1ȸ �������� ��ȯ ����Ʈ
uint SubWord(uint W);										//SubWord
uint RotWord(uint W);										//RotWord
byte x_time(byte n, byte b);								//GF(2^8)�󿡼� ���� ���� �Լ�

#endif