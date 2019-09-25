#define DEBUG 0

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// 토큰-문서 구조체
typedef struct {
	char	*token;	// 토큰
	int		docid;	// 문서번호(document ID)
} tTokenDoc;

typedef struct {
	int		index;	// starting position in posting.idx
	int		df;		// 문서 빈도(document frequency)
} tHEADER;

////////////////////////////////////////////////////////////////////////////////
// 토큰 구조체로부터 역색인 파일을 생성한다.
void invertedIndex( tTokenDoc *tokens, int num_tokens, 
					char *dicfilename, char *headerfilename, char *postingfilename);

// 입력 파일로부터 토큰-문서 구조체를 생성한다.
tTokenDoc *get_tokens(char *filename, int *num_tokens);

// qsort를 위한 비교함수 (첫번째 정렬 기준: 토큰 문자열, 두번째 정렬 기준: 문서 번호)
static int _compare(const void *n1, const void *n2);

static void print_tokens( tTokenDoc *tokens, int num_tokens);

void destroy_tokens( tTokenDoc *tokens, int num_tokens);

char *s_gets(char *st, int n, FILE *fp);
////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	tTokenDoc *tokens;
	int num_tokens = 0;

	if (argc != 2)
	{
		printf( "Usage: %s FILE\n", argv[0]);
		return 2;
	}
	
	tokens = get_tokens( argv[1], &num_tokens);
	
	//assert( tokens != NULL && num_tokens > 0);

#if DEBUG	
	print_tokens( tokens, num_tokens);
#endif
	
	// 정렬 (첫번째 정렬 기준: 토큰 문자열, 두번째 정렬 기준: 문서 번호)
	qsort( tokens, num_tokens, sizeof( tTokenDoc), _compare);
	
	invertedIndex( tokens, num_tokens, "dic.txt", "header.idx", "posting.idx");

#if DEBUG	
	print_tokens( tokens, num_tokens);
#endif	
	
	destroy_tokens( tokens, num_tokens);
	
	return 0;
}

void invertedIndex( tTokenDoc *tokens, int num_tokens, 
					char *dicfilename, char *headerfilename, char *postingfilename) {
	
	int nt = 1;
	int idx = -1;
	int Pidx = -1;
	int *posting;
	tHEADER *header;
	FILE *fpD;
	FILE *fpH;
	FILE *fpP;
	FILE *test;			//test

	fpD = fopen(dicfilename, "wt");
	fpH = fopen(headerfilename, "wb");
	fpP = fopen(postingfilename, "wb");
	test = fopen("test.txt", "wt");			//test

	header = (tHEADER *)malloc(sizeof(tHEADER) * num_tokens);
	posting = (int *)malloc(sizeof(int) * num_tokens);

	if (fpD == NULL) {
		fprintf( stderr, "File open error:%s\n", dicfilename);
		return;
	}
	if (fpH == NULL) {
		fprintf( stderr, "File open error:%s\n", headerfilename);
		return;
	}
	if (fpP == NULL) {
		fprintf( stderr, "File open error:%s\n", postingfilename);
		return;
	}

	header[++idx].index = 0;
	header[idx].df = 1;
	posting[++Pidx] = tokens[0].docid;

	fputs(tokens[idx].token, fpD);
	fprintf(fpD, "\n");

	for (int index = 0; index < num_tokens - 1; index++) {
		if (strcmp(tokens[index].token, tokens[index + 1].token) != 0) {
			posting[++Pidx] = tokens[index + 1].docid;
			header[++idx].index = Pidx;
			header[idx].df = 1;
			
			fputs(tokens[index + 1].token, fpD);
			fprintf(fpD, "\n");
		}
		else if (tokens[index].docid != tokens[index + 1].docid) {
			posting[++Pidx] = tokens[index + 1].docid;
			header[idx].df++;
		}
	}

	for (int i = 0; i <= idx; i++) {
		fprintf(test, "%d\t%d\n", header[i].index, header[i].df);
	}

	fwrite( header, sizeof(tHEADER), idx + 1, fpH);
	fwrite( posting, sizeof(int), Pidx + 1, fpP);

	fclose(test);				//test
	fclose(fpD);
	fclose(fpH);
	fclose(fpP);
	free(header);
	free(posting);
}

tTokenDoc *get_tokens(char *filename, int *num_tokens) {
	FILE *fp;
	tTokenDoc *Dic;
	char *str;
	char *ptr;
	int docNum = 0;
	int nt = 1;


	Dic = (tTokenDoc *)malloc(sizeof(tTokenDoc) * 1000);
	str = (char *)malloc(sizeof(char) * 5000);

	fp = fopen(filename, "rt");

	while (s_gets(str, 5000, fp)) {
		docNum++;
		ptr = strtok(str, " \n");

		do {
			*num_tokens += 1;

			if ((*num_tokens) % 1000 == 0) {
				nt++;
				Dic = (tTokenDoc *)realloc(Dic, sizeof(tTokenDoc) * nt * 1000);
			}

			Dic[*num_tokens - 1].token = strdup(ptr);
			Dic[*num_tokens - 1].docid = docNum;

		} while (ptr = strtok(NULL, " \n"));
		
		free(str);
		str = NULL;
		str = (char *)malloc(sizeof(char) * 5000);
	}

	free(str);
	fclose(fp);

	return Dic;
}

static int _compare(const void *n1, const void *n2) {
	const tTokenDoc *ptr1 = (const tTokenDoc *)n1;
	const tTokenDoc *ptr2 = (const tTokenDoc *)n2;

	if (strcmp(ptr1->token, ptr2->token) < 0)
		return -1;
	else if (strcmp(ptr1->token ,ptr2->token) == 0) {
		if (ptr1->docid > ptr2->docid)
			return 1;
		else 
			return -1;
	}
	else
		return 1;
	
}

static void print_tokens( tTokenDoc *tokens, int num_tokens)
{
	int i;
	
	for (i = 0; i < num_tokens; i++)
	{
		printf( "%s\t%d\n", tokens[i].token, tokens[i].docid);
	}
}

void destroy_tokens( tTokenDoc *tokens, int num_tokens)
{
	int i;
	
	for (i = 0; i < num_tokens; i++)
	{
		free( tokens[i].token);
	}
	free( tokens);
}

char *s_gets(char *st, int n, FILE *fp) {
	char *ret_val;
	char *find;

	ret_val = fgets(st, n, fp);
	if (ret_val) {
		find = strchr(st, '\n');
		if (find)
			*find = '\0';
		else
			while (getchar() != '\n');
	}
	return ret_val;
}