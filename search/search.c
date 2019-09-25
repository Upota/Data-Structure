//#define DEBUG 1
#define AND			'&'
#define OR			'|'

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "trie.h"

// 역색인 헤더 정보에 대한 구조체
typedef struct {
	int		index;	// starting position in posting.idx
	int		df;		// document frequency
} tHEADER;

////////////////////////////////////////////////////////////////////////////////
// 헤더 정보가 저장된 파일(예) "header.idx")을 읽어 메모리에 저장한다.
// 헤더 구조체 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
tHEADER *load_header( char *filename);

// 포스팅 리스트가 저장된 파일(예) "posting.idx")를 읽어 메모리에 저장한다.
// 포스팅 리스트(int arrary) 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
int *load_posting( char *filename);

// 문서 집합을 화면에 출력한다.
void showDocuments( int *docs, int numdocs);

// 두 문서 집합의 교집합을 구한다.
// 교집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 교집합의 문서 수는 newnumdocs에 저장한다.
int *intersectDocuments( int *docs, int numdocs, int *docs2, int numdocs2, int *newnumdocs);

// 두 문서 집합의 합집합을 구한다.
// 합집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 합집합의 문서 수는 newnumdocs에 저장한다.
int *unionDocuments( int *docs, int numdocs, int *docs2, int numdocs2, int *newnumdocs);

// 단일 텀(single term)을 검색하여 문서를 찾는다.
// 문서 집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 검색된 문서 수는 newnumdocs에 저장한다.
int *getDocuments( tHEADER *header, int *posting, TRIE *trie, char *term, int *numdocs);

// 질의(query)를 검색하여 문서를 찾는다.
// 질의는 단일 텀 또는 하나 이상의 불린 연산자('&' 또는 '|')를 포함한 질의가 될 수 있다.
// 문서 집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 검색된 문서 수는 newnumdocs에 저장한다.
int *searchDocuments( tHEADER *header, int *posting, TRIE *trie, char *query, int *numdocs);

////////////////////////////////////////////////////////////////////////////////
static char *rtrim( char *str)
{
	char *p = str + strlen( str) - 1;
	
	while (p >= str)
	{
		if (*p == '\n' || *p == ' ' || *p == '\t') *p = 0;
		else return str;
		p--;
	}
}

static char *ltrim( char *str)
{
	char *p = str;
	
	while (*p)
	{
		if (*p == '\n' || *p == ' ' || *p == '\t') p++;
		else 
		{
			return p;
		}
	}
	return NULL;
}

static char *trim( char *str)
{
	if( str == NULL || *str == 0) return str;
	
	return rtrim(ltrim(str));
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	tHEADER *header;
	int *posting;
	TRIE *trie;
	char query[100];
	int index;
	
	header = load_header( "header.idx");
	if (header == NULL) return 1;
	
	posting = load_posting( "posting.idx");
	if (posting == NULL) return 1;
		
	trie = dic2trie( "dic.txt");
	
	printf( "\nQuery: ");
	while (fgets( query, 100, stdin) != NULL)
	{
		int numdocs;
		int *docs = searchDocuments( header, posting, trie, query, &numdocs);
		
		if (docs == NULL) printf( "not found!\n");
		else 
		{
			showDocuments( docs, numdocs);
			free( docs);
		}
		printf( "\nQuery: ");
	}
	
	free( header);
	free( posting);
	trieDestroy( trie);
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// 헤더 정보가 저장된 파일(예) "header.idx")을 읽어 메모리에 저장한다.
// 헤더 구조체 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
tHEADER *load_header( char *filename) {
	FILE *fp;
	tHEADER *header;
	int size;

	fp = fopen(filename, "rb");

	if (fp == NULL) {
		fprintf( stderr, "File open error:%s\n", filename);
		return NULL;
	}
	
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	size = size / sizeof(tHEADER);
	fseek(fp, 0, SEEK_SET);
	header = (tHEADER *)malloc(sizeof(tHEADER) * size);

	fread(header, sizeof(tHEADER), size, fp);

	fclose(fp);

	return header;
}

// 포스팅 리스트가 저장된 파일(예) "posting.idx")를 읽어 메모리에 저장한다.
// 포스팅 리스트(int arrary) 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
int *load_posting( char *filename) {
	FILE *fp;
	int *posting;
	int size;

	fp = fopen(filename, "rb");

	if (fp == NULL) {
		fprintf( stderr, "File open error:%s\n", filename);
		return NULL;
	}
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	size = size / sizeof(int);
	fseek(fp, 0, SEEK_SET);
	posting = (int *)malloc(sizeof(int) * size);
	fread(posting, sizeof(int), size, fp);

	fclose(fp);

	return posting;
}

// 문서 집합을 화면에 출력한다.
void showDocuments( int *docs, int numdocs) {
	for (int i = 0; i < numdocs; i++) {
		printf(" %d", docs[i]);
	}
	printf("\n");
}

// 두 문서 집합의 교집합을 구한다.
// 교집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 교집합의 문서 수는 newnumdocs에 저장한다.
int *intersectDocuments( int *docs, int numdocs, int *docs2, int numdocs2, int *newnumdocs) {
	int i = 0;
	int j = 0;
	int idx = -1;
	int nt = 0;
	int *result;

	if ((docs == NULL) || (docs2 == NULL)) {
		*newnumdocs = 0;
		return NULL;
	}

	result = (int *)malloc(sizeof(int) * 100);

	if (result == NULL) {
		*newnumdocs = 0;
		return NULL;
	}

	while ((i < numdocs) && (j < numdocs2)) {
		if (docs[i] < docs2[j])
			i++;
		else if (docs[i] > docs2[j])
			j++;
		else {
			idx++;
			if (idx % 100 == 0) {
				nt++;
				result = (int *)realloc(result, sizeof(int) * 100 * nt);
			}
			if (result == NULL)	{
				*newnumdocs = 0;
				return NULL;
			}
			result[idx] = docs[i];
			i++;	j++;
		}
		*newnumdocs = idx + 1;
	}

	return result;
}

// 두 문서 집합의 합집합을 구한다.
// 합집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 합집합의 문서 수는 newnumdocs에 저장한다.
int *unionDocuments( int *docs, int numdocs, int *docs2, int numdocs2, int *newnumdocs) {
	int i = 0;
	int j = 0;
	int idx = -1;
	int nt = 1;
	int *result;

	if ((docs == NULL) && (docs2 == NULL)) {
		*newnumdocs = 0;
		return NULL;
	}

	result = (int *)malloc(sizeof(int) * 100);

	if (result == NULL) {
		*newnumdocs = 0;
		return NULL;
	}

	while ((i < numdocs) && (j < numdocs2)) {
		idx++;
		if (idx % 100 == 0) {
			nt++;
			result = (int *)realloc(result, sizeof(int) * 100 * nt);
		}

		if (docs[i] < docs2[j]) {
			result[idx] = docs[i];
			i++;
		}
		else if (docs[i] > docs2[j]) {
			result[idx] = docs2[j];
			j++;
		}
		else {
			result[idx] = docs[i];
			i++;	j++;
		}
	}

	while (i < numdocs) {
		idx++;
		if (idx % 100 == 0) {
			nt++;
			result = (int *)realloc(result, sizeof(int) * 100 * nt);
		}
		result[idx] = *(docs + i);
		i++;
	}
	while (j < numdocs2) {
		idx++;
		if (idx % 100 == 0) {
			nt++;
			result = (int *)realloc(result, sizeof(int) * 100 * nt);
		}
		result[idx] = docs2[j];
		j++;
	}

	*newnumdocs = idx + 1;

	return result;
}

// 단일 텀(single term)을 검색하여 문서를 찾는다.
// 문서 집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 검색된 문서 수는 newnumdocs에 저장한다.
int *getDocuments( tHEADER *header, int *posting, TRIE *trie, char *term, int *numdocs) {
	int Hidx, Pidx;
	int *docs;
	char *clean;

	clean = trim(term);

	Hidx = trieSearch( trie, clean);
	if (Hidx == -1)	 {
		*numdocs = 0;
		return NULL;
	}

	Pidx = header[Hidx].index;
	*numdocs = header[Hidx].df;

	docs = (int *)malloc(sizeof(int) * (*numdocs));
	if (docs == NULL) {
		*numdocs = 0;
		return NULL;
	}

	for (int i = 0; i < *numdocs; i++) {
		docs[i] = posting[Pidx + i];
	}

	return docs;
}

// 질의(query)를 검색하여 문서를 찾는다.
// 질의는 단일 텀 또는 하나 이상의 불린 연산자('&' 또는 '|')를 포함한 질의가 될 수 있다.
// 문서 집합을 위한 메모리를 할당하고 그 주소를 반환
// 실패시 NULL을 반환
// 검색된 문서 수는 newnumdocs에 저장한다.
int *searchDocuments( tHEADER *header, int *posting, TRIE *trie, char *query, int *numdocs) {
	char *clean;
	char *ptr;
	char *temp;

	int front = 0;
	int rear = -1;
	int idx = -1;
	int count = 0;
	int nt = 1;
	int *docs;
	int *boolQ;
	int *term1, *term2, *term3, *tmp;
	int numdocs1, numdocs2, newnumdocs;
	
	clean = trim(query);

	if (strchr(clean, AND) || strchr(clean, OR)) {
		boolQ = (int *)malloc(sizeof(int) * 1000);

		for (int i = 0; i < strlen(clean); i++) {
			if (clean[i] == AND) {
				rear++;
				if ((rear + 1) % 1000 == 0) {
					nt++;
					boolQ = (int *)realloc(boolQ, sizeof(int) * 1000 * nt);
				}
				boolQ[rear] = 1;
			}
			else if (clean[i] == OR) {
				rear++;
				if ((rear + 1) % 1000 == 0) {
					nt++;
					boolQ = (int *)realloc(boolQ, sizeof(int) * 1000 * nt);
				}
				boolQ[rear] = 2;
			}
		}

		ptr = strtok(clean, "&|");
		temp = strdup(ptr);
		term1 = getDocuments(header, posting, trie, temp, &numdocs1);

		free(temp);
		temp = NULL;

		do {
			ptr = strtok(NULL, "&|");
			temp = strdup(ptr);
			term2 = getDocuments(header, posting, trie, temp, &numdocs2);

			free(temp);
			temp = NULL;

			switch (boolQ[front]) {
				case 1:
					term3 = intersectDocuments(term1, numdocs1, term2, numdocs2, &newnumdocs);
					break;
				case 2:
					term3 = unionDocuments(term1, numdocs1, term2, numdocs2, &newnumdocs);
					break;
				default:
					free(term1);
					free(term2);
					free(boolQ);
					return NULL;
			}

			if (term1 != NULL) {
				free(term1);
				term1 = NULL;
			}
			if (term2 != NULL) {
				free(term2);
				term2 = NULL;
			}

			term1 = term3;
			numdocs1 = newnumdocs;

			front++;
		} while (front <= rear);

		free(boolQ);
		boolQ = NULL;

		if (term1 == NULL) {
			return NULL;
		}

		docs = term1;
		*numdocs = newnumdocs;
	}
	else {
		docs = getDocuments(header, posting, trie, clean, numdocs);

		if (docs == NULL)	return NULL;
	}


	return docs;
}
