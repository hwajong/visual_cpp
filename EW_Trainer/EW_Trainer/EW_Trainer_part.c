#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <conio.h>   // for _getche()
#include <stdlib.h>  // for system(), exit() 
#include <Windows.h> // for Sleep()
#include <time.h>    // for time()

#define MaxWord 1000
#define MaxWLen 20
#define MaxMLen 50
#define MaxSLen 160
#define DelayTime 3000 // �޴� ���� �����ð� (����: 1/1000��) 
#define DictName "eng.dic"

typedef struct {
	char word[MaxWLen];     // �ܾ�
	char mean[MaxMLen];     // �ѱ��ؼ� 
	char synonym[MaxWLen];  // ���Ǿ�
	char antonym[MaxWLen];  // ���Ǿ�  
	char sentence[MaxSLen]; // ����
} EWord;

int  openWord(EWord *);
int  addWord(EWord *, int);
void listWord(EWord *, int);
void sortWord(EWord *, int);
void printWord(EWord *, int);
void searchWord(EWord *, int);
void memoryWord(EWord *, int);
void saveWord(EWord *, int);

int main(void)
{
	EWord Dict[MaxWord] = { 0 };    // Main Dictionary
	char choice;					// Input character
	int line = 0;                   // line counter
	int WCount = 0;					// Total Number of EWord
	char *FBanner = "*************** ����ܾ���� v0.1 **************\n";

	puts(FBanner);

	WCount = openWord(Dict);     // �ܾ��� ���� ����

	do {
		system("cls");  // ȭ�������
		puts(FBanner);
		printf("*** ���� �޴� �� �ϳ��� �����ϼ���.\n\n");
		printf("1.�Է�(A), 2.�˻�(S), 3.���(L), 4.�ϱ�(M), 5.����(Q) : ");

		choice = _getch();
		fflush(stdin);

		switch (choice) {
		case '1':
		case 'A':
		case 'a':
			if (WCount < MaxWord)
				WCount = addWord(Dict, WCount);
			else {
				printf("\a\a** �ܾ����� ����á���ϴ�!");
				Sleep(DelayTime);
			}
			break;

		case '2':
		case 'S':
		case 's':
			if (WCount > 0)
				searchWord(Dict, WCount);
			else {
				printf("\a\a** �ܾ����� ����ֽ��ϴ�!");
				Sleep(DelayTime);
			}
			break;

		case '3':
		case 'L':
		case 'l':
			if (WCount > 0)
				listWord(Dict, WCount);
			else {
				printf("\a\a** �ܾ����� ����ֽ��ϴ�!");
				Sleep(DelayTime);
			}
			break;

		case '4':
		case 'M':
		case 'm':
			if (WCount > 0)
				memoryWord(Dict, WCount);
			else {
				printf("\a\a** �ܾ����� ����ֽ��ϴ�!");
				Sleep(DelayTime);
			}
			break;

		case '5':
		case 'Q':
		case 'q':
			if (WCount > 0) {
				saveWord(Dict, WCount);
				printf("\n\n ** %d���� �ܾ �ܾ��忡 �����߽��ϴ�.\n", WCount);
			}
			else
				printf("\n\n ** �ܾ��忡 ������ �ܾ �����ϴ�.\n");

			printf("\n\a*** ���α׷��� �����ϰڽ��ϴ�. ***\n");
			Sleep(DelayTime);
			break;

		default:
			printf("\a�߸� �Է��ϼ̽��ϴ�.\n");
			Sleep(DelayTime);
			break;
		}

	} while ((choice != 'q') && (choice != 'Q') && (choice != '5'));

	return 0;
}

int openWord(EWord *pDict)
{
	FILE *file;
	int rCount = 0, line = 0;
	int choice;
	char temp[MaxSLen];			// read buffer


	if ((file = fopen(DictName, "rt")) == NULL) {
		printf("*** �ܾ��� ����(eng.dic)�� ���� �� �����ϴ�.\n");
		printf("  - �ܾ��� ������ ���α׷��� ���� ��ġ�� �־�� �մϴ�.\n");
		printf("  - �����ϼ��� - (1) �ܾ��� ���� �����  (2) �����ϱ� : ");
		while (1) {
			choice = _getche();
			if (choice == '1') {
				printf("\n�ܾ����� ���� ��������ϴ�.\n");
				Sleep(2000);
				return rCount;  // 0
			}
			else if (choice == '2') {
				printf("\n���α׷��� �����մϴ�!\n");
				Sleep(2000);
				exit(0);
			}
			else
				continue;
		}
	}
	else {
		while (fgets(temp, sizeof(temp), file) != NULL) {
			temp[strlen(temp) - 1] = '\0';
			switch (line) {
			case 0:
				strcpy(pDict[rCount].word, temp);
				line++;
				break;
			case 1:
				strcpy(pDict[rCount].mean, temp);
				line++;
				break;
			case 2:
				strcpy(pDict[rCount].synonym, temp);
				line++;
				break;
			case 3:
				strcpy(pDict[rCount].antonym, temp);
				line++;
				break;
			case 4:
				strcpy(pDict[rCount].sentence, temp);
				line = 0;
				rCount++;
				break;
			}
		}
		printf("\a�ܾ��忡�� %d���� �ܾ �о�鿴���ϴ�.\n", rCount);
		Sleep(DelayTime);
		fclose(file);
		return rCount;
	}
}

void saveWord(EWord *pDict, int WNum)
{
	FILE *file;
	int i;

	if ((file = fopen(DictName, "wt")) == NULL) {
		printf(" \a**�ܾ��� ���� ���� ����!\n");
		return;
	}

	for (i = 0; i < WNum; i++) {
		fprintf(file, "%s\n", pDict[i].word);
		fprintf(file, "%s\n", pDict[i].mean);
		fprintf(file, "%s\n", pDict[i].synonym);
		fprintf(file, "%s\n", pDict[i].antonym);
		fprintf(file, "%s\n", pDict[i].sentence);
	}

	fclose(file);
}

// ���ڿ��� �յ� ������ �����Ѵ�.
void trim(char* s)
{
	char *p = s;
	int l = (int)strlen(p);

	while (l - 1 >= 0 && !(p[l - 1] & 0x80) && isspace(p[l - 1])) p[--l] = 0;
	while (*p && !(*p & 0x80) && isspace(*p)) ++p, --l;

	memmove(s, p, l + 1);
}

// �ܾ��忡 �ܾ� ������ �Է��Ѵ�.
int addWord(EWord *pDict, int WNum)
{
	int retry = 1;

	do
	{
		system("cls");  // ȭ�������
		printf("\n*** ������ �ܾ��� ������ ���� �䱸���׿� �°� �Է��ϼ���.\n\n");
		printf("- ��  ��: ");
		fgets(pDict[WNum].word, MaxWLen, stdin);
		trim(pDict[WNum].word);

		printf("- ��  ��: ");
		fgets(pDict[WNum].mean, MaxMLen, stdin);
		trim(pDict[WNum].mean);

		printf("- ���Ǿ�: ");
		fgets(pDict[WNum].synonym, MaxWLen, stdin);
		trim(pDict[WNum].synonym);

		printf("- ���Ǿ�: ");
		fgets(pDict[WNum].antonym, MaxWLen, stdin);
		trim(pDict[WNum].antonym);

		printf("- ��  ��: ");
		fgets(pDict[WNum].sentence, MaxSLen, stdin);
		trim(pDict[WNum].sentence);

		WNum++;

		printf("\n** �Է��� �����÷��� 'Q'�� ����Ͻ÷��� �ƹ�Ű�� ��������.\n");

		char choice = _getch();
		fflush(stdin);

		switch (choice) {
		case 'Q':
		case 'q':
			printf("\n** �ܾ��� �Է��� �Ϸ�Ǿ����ϴ�.\n");
			Sleep(DelayTime);
			retry = 0;
			break;
		}
	} while (retry);


	return WNum;
}

// ��� �ܾ� ��� 
void listWord(EWord *pDict, int WNum)
{
	system("cls");  // ȭ�������
	printf("\n*** �Է��� �ܾ� ����ϱ�.\n");
	printf(" - �Էµ� �ܾ� ��: %d\n", WNum);

	for (int i = 0; i < WNum; i++) {
		printf("\n == [%d]��° �ܾ� =====\n\n", i+1);
		printf("- ��  ��: %s\n", pDict[i].word);
		printf("- ��  ��: %s\n", pDict[i].mean);
		printf("- ���Ǿ�: %s\n", pDict[i].synonym);
		printf("- ���Ǿ�: %s\n", pDict[i].antonym);
		printf("- ��  ��: %s\n", pDict[i].sentence);
	}

	printf("\n\n*** �ܾ����� ����� �Ϸ�Ǿ����ϴ�. �޴��� ���÷��� �ƹ�Ű�� ��������\n");
	char choice = _getch();
	fflush(stdin);
}

// �ܾ� ã�� 
void searchWord(EWord *pDict, int WNum)
{
	system("cls");  // ȭ�������
	printf("\n*** �˻��� �ܾ �Է��ϼ���: ");

	char searchWord[MaxWLen] = { 0, };
	fgets(searchWord, MaxMLen, stdin);
	trim(searchWord);

	printf("\n - ���� �Էµ� 2���� �ܾ�� '%s'�� ã���ϴ�.\n", searchWord);

	int countRepWord = 0;
	for (int i = 0; i < WNum; i++) {
		if (strcmp(pDict[i].word, searchWord) == 0) {
			printf("\n ** �ܾ�(��ǥ�ܾ�)�� ã�ҽ��ϴ� **\n");
			printf("- ��  ��: %s\n", pDict[i].word);
			printf("- ��  ��: %s\n", pDict[i].mean);
			printf("- ���Ǿ�: %s\n", pDict[i].synonym);
			printf("- ���Ǿ�: %s\n", pDict[i].antonym);
			printf("- ��  ��: %s\n", pDict[i].sentence);
			countRepWord = 1;
			break;
		}
	}
	
	int countIncludeWord = 0;
	for (int i = 0; i < WNum; i++) {
		if (strstr(pDict[i].word, searchWord) && strcmp(pDict[i].word, searchWord) != 0) {
			printf("\n ** �ܾ�: %s **\n", pDict[i].word);
			printf("  - ���� = %s\n", pDict[i].sentence);
			countIncludeWord++;
		}
	}

	if (countRepWord == 0) printf("\n** �Է��� �ܾ�� ���� ��ǥ�ܾ�� ã�� ���߽��ϴ�.\n");

	if (countIncludeWord == 0) printf("\n ** �Է��� �ܾ ���Ե� �ٸ� �ܾ�� ã�� ���߽��ϴ�.\n");
	else printf("\n ** �Է��� �ܾ ���Ե� �ٸ� �ܾ��� ��: %d\n", countIncludeWord);


	printf("\n*** �ܾ��� �˻��� �Ϸ�Ǿ����ϴ�. �޴��� ���÷��� �ƹ�Ű�� ��������\n");
	char choice = _getch();
	fflush(stdin);
}

// �ܾ� ��� 
void memoryWord(EWord *pDict, int WNum)
{
	for (int i = 0; i < WNum; i++) {

		system("cls");  // ȭ�������
		printf("\n*** �Է��� �ܾ��� �ǹ� ���ø���.\n");
		printf(" - �Էµ� �ܾ� ��: %d\n", WNum);

		printf("\n ** ���� �ܾ��� �ǹ̸� ���÷�������. �ǹ̸� Ȯ���Ϸ��� �ƹ�Ű�� ��������.\n");

		printf("\n  (%d) %s -> ", i+1, pDict[i].word);

		char choice = _getch();
		fflush(stdin);

		printf("%s\n", pDict[i].mean);

		printf("\n ** �ܾ� �ϱ� Ȯ���� �����÷��� 'Q'��, ����Ͻ÷��� �ƹ�Ű�� ��������.\n");

		choice = _getch();
		fflush(stdin);

		switch (choice) {
		case 'Q':
		case 'q':
			printf("\n*** �ܾ� �ϱ� Ȯ���� �����մϴ�.\n");
			Sleep(DelayTime);
			return;
		}
	}

	printf("\n** ��� �ܾ ���� Ȯ���� �߽��ϴ�.\n");
	printf("\n*** �ܾ� �ϱ� Ȯ���� �����մϴ�.\n");
	Sleep(DelayTime);
}