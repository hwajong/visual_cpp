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
#define DelayTime 3000 // 메뉴 복귀 지연시간 (단위: 1/1000초) 
#define DictName "eng.dic"

typedef struct {
	char word[MaxWLen];     // 단어
	char mean[MaxMLen];     // 한글해석 
	char synonym[MaxWLen];  // 유의어
	char antonym[MaxWLen];  // 반의어  
	char sentence[MaxSLen]; // 예문
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
	char *FBanner = "*************** 영어단어공부 v0.1 **************\n";

	puts(FBanner);

	WCount = openWord(Dict);     // 단어장 파일 열기

	do {
		system("cls");  // 화면지우기
		puts(FBanner);
		printf("*** 다음 메뉴 중 하나를 선택하세요.\n\n");
		printf("1.입력(A), 2.검색(S), 3.목록(L), 4.암기(M), 5.종료(Q) : ");

		choice = _getch();
		fflush(stdin);

		switch (choice) {
		case '1':
		case 'A':
		case 'a':
			if (WCount < MaxWord)
				WCount = addWord(Dict, WCount);
			else {
				printf("\a\a** 단어장이 가득찼습니다!");
				Sleep(DelayTime);
			}
			break;

		case '2':
		case 'S':
		case 's':
			if (WCount > 0)
				searchWord(Dict, WCount);
			else {
				printf("\a\a** 단어장이 비어있습니다!");
				Sleep(DelayTime);
			}
			break;

		case '3':
		case 'L':
		case 'l':
			if (WCount > 0)
				listWord(Dict, WCount);
			else {
				printf("\a\a** 단어장이 비어있습니다!");
				Sleep(DelayTime);
			}
			break;

		case '4':
		case 'M':
		case 'm':
			if (WCount > 0)
				memoryWord(Dict, WCount);
			else {
				printf("\a\a** 단어장이 비어있습니다!");
				Sleep(DelayTime);
			}
			break;

		case '5':
		case 'Q':
		case 'q':
			if (WCount > 0) {
				saveWord(Dict, WCount);
				printf("\n\n ** %d개의 단어를 단어장에 저장했습니다.\n", WCount);
			}
			else
				printf("\n\n ** 단어장에 저장할 단어가 없습니다.\n");

			printf("\n\a*** 프로그램을 종료하겠습니다. ***\n");
			Sleep(DelayTime);
			break;

		default:
			printf("\a잘못 입력하셨습니다.\n");
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
		printf("*** 단어장 파일(eng.dic)을 읽을 수 없습니다.\n");
		printf("  - 단어장 파일은 프로그램과 같은 위치에 있어야 합니다.\n");
		printf("  - 선택하세요 - (1) 단어장 새로 만들기  (2) 종료하기 : ");
		while (1) {
			choice = _getche();
			if (choice == '1') {
				printf("\n단어장을 새로 만들었습니다.\n");
				Sleep(2000);
				return rCount;  // 0
			}
			else if (choice == '2') {
				printf("\n프로그램을 종료합니다!\n");
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
		printf("\a단어장에서 %d개의 단어를 읽어들였습니다.\n", rCount);
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
		printf(" \a**단어장 파일 쓰기 오류!\n");
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

// 문자열의 앞뒤 공백을 제거한다.
void trim(char* s)
{
	char *p = s;
	int l = (int)strlen(p);

	while (l - 1 >= 0 && !(p[l - 1] & 0x80) && isspace(p[l - 1])) p[--l] = 0;
	while (*p && !(*p & 0x80) && isspace(*p)) ++p, --l;

	memmove(s, p, l + 1);
}

// 단어장에 단어 정보를 입력한다.
int addWord(EWord *pDict, int WNum)
{
	int retry = 1;

	do
	{
		system("cls");  // 화면지우기
		printf("\n*** 공부할 단어의 정보를 다음 요구사항에 맞게 입력하세요.\n\n");
		printf("- 단  어: ");
		fgets(pDict[WNum].word, MaxWLen, stdin);
		trim(pDict[WNum].word);

		printf("- 의  미: ");
		fgets(pDict[WNum].mean, MaxMLen, stdin);
		trim(pDict[WNum].mean);

		printf("- 동의어: ");
		fgets(pDict[WNum].synonym, MaxWLen, stdin);
		trim(pDict[WNum].synonym);

		printf("- 반의어: ");
		fgets(pDict[WNum].antonym, MaxWLen, stdin);
		trim(pDict[WNum].antonym);

		printf("- 예  문: ");
		fgets(pDict[WNum].sentence, MaxSLen, stdin);
		trim(pDict[WNum].sentence);

		WNum++;

		printf("\n** 입력을 끝내시려면 'Q'를 계속하시려면 아무키나 누르세요.\n");

		char choice = _getch();
		fflush(stdin);

		switch (choice) {
		case 'Q':
		case 'q':
			printf("\n** 단어의 입력이 완료되었습니다.\n");
			Sleep(DelayTime);
			retry = 0;
			break;
		}
	} while (retry);


	return WNum;
}

// 모든 단어 출력 
void listWord(EWord *pDict, int WNum)
{
	system("cls");  // 화면지우기
	printf("\n*** 입력한 단어 출력하기.\n");
	printf(" - 입력된 단어 수: %d\n", WNum);

	for (int i = 0; i < WNum; i++) {
		printf("\n == [%d]번째 단어 =====\n\n", i+1);
		printf("- 단  어: %s\n", pDict[i].word);
		printf("- 의  미: %s\n", pDict[i].mean);
		printf("- 동의어: %s\n", pDict[i].synonym);
		printf("- 반의어: %s\n", pDict[i].antonym);
		printf("- 예  문: %s\n", pDict[i].sentence);
	}

	printf("\n\n*** 단어장의 출력이 완료되었습니다. 메뉴로 가시려면 아무키나 누르세요\n");
	char choice = _getch();
	fflush(stdin);
}

// 단어 찾기 
void searchWord(EWord *pDict, int WNum)
{
	system("cls");  // 화면지우기
	printf("\n*** 검색할 단어를 입력하세요: ");

	char searchWord[MaxWLen] = { 0, };
	fgets(searchWord, MaxMLen, stdin);
	trim(searchWord);

	printf("\n - 현재 입력된 2개의 단어에서 '%s'를 찾습니다.\n", searchWord);

	int countRepWord = 0;
	for (int i = 0; i < WNum; i++) {
		if (strcmp(pDict[i].word, searchWord) == 0) {
			printf("\n ** 단어(대표단어)를 찾았습니다 **\n");
			printf("- 단  어: %s\n", pDict[i].word);
			printf("- 의  미: %s\n", pDict[i].mean);
			printf("- 동의어: %s\n", pDict[i].synonym);
			printf("- 반의어: %s\n", pDict[i].antonym);
			printf("- 예  문: %s\n", pDict[i].sentence);
			countRepWord = 1;
			break;
		}
	}
	
	int countIncludeWord = 0;
	for (int i = 0; i < WNum; i++) {
		if (strstr(pDict[i].word, searchWord) && strcmp(pDict[i].word, searchWord) != 0) {
			printf("\n ** 단어: %s **\n", pDict[i].word);
			printf("  - 예문 = %s\n", pDict[i].sentence);
			countIncludeWord++;
		}
	}

	if (countRepWord == 0) printf("\n** 입력한 단어와 같은 대표단어는 찾지 못했습니다.\n");

	if (countIncludeWord == 0) printf("\n ** 입력한 단어가 포함된 다른 단어는 찾지 못했습니다.\n");
	else printf("\n ** 입력한 단어가 포함된 다른 단어의 수: %d\n", countIncludeWord);


	printf("\n*** 단어의 검색이 완료되었습니다. 메뉴로 가시려면 아무키나 누르세요\n");
	char choice = _getch();
	fflush(stdin);
}

// 단어 기억 
void memoryWord(EWord *pDict, int WNum)
{
	for (int i = 0; i < WNum; i++) {

		system("cls");  // 화면지우기
		printf("\n*** 입력한 단어의 의미 떠올리기.\n");
		printf(" - 입력된 단어 수: %d\n", WNum);

		printf("\n ** 다음 단어의 의미를 떠올려보세요. 의미를 확인하려면 아무키나 누르세요.\n");

		printf("\n  (%d) %s -> ", i+1, pDict[i].word);

		char choice = _getch();
		fflush(stdin);

		printf("%s\n", pDict[i].mean);

		printf("\n ** 단어 암기 확인을 끝내시려면 'Q'를, 계속하시려면 아무키나 누르세요.\n");

		choice = _getch();
		fflush(stdin);

		switch (choice) {
		case 'Q':
		case 'q':
			printf("\n*** 단어 암기 확인을 종료합니다.\n");
			Sleep(DelayTime);
			return;
		}
	}

	printf("\n** 모든 단어에 대해 확인을 했습니다.\n");
	printf("\n*** 단어 암기 확인을 종료합니다.\n");
	Sleep(DelayTime);
}