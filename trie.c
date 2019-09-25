#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // isupper, tolower

#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW
#define EOW			'$' // end of word

// used in the following functions: trieInsert, trieSearch, triePrefixList
#define getIndex(x)		(((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a'))

// TRIE type definition
typedef struct trieNode {
	char 			*entry;
	struct trieNode	*subtrees[MAX_DEGREE];
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
TRIE *trieCreateNode(void);

/* Deletes all data in trie and recycles memory
*/
void trieDestroy( TRIE *root);

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert( TRIE *root, char *str);

/* Retrieve trie for the requested key
	return	1 key found
			0 key not found
*/
int trieSearch( TRIE *root, char *str);

/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root);

/* prints all entries starting with str (as prefix) in trie
   ex) "abb" -> "abbas", "abbasid", "abbess", ...
	using trieList function
*/
void triePrefixList( TRIE *root, char *str);

/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms( char *str, char *permuterms[]);

/* recycles memory for permuterms
*/
void clear_permuterms( char *permuterms[], int size);

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	using triePrefixList function
*/
void trieSearchWildcard( TRIE *root, char *str);

int main(int argc, char **argv)
{
	TRIE *trie;
	TRIE *permute_trie;
	int ret;
	char str[100];
	FILE *fp;
	char *permuterms[100];
	int num_p;
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "File open error: %s\n", argv[1]);
		return 1;
	}
	
	trie = trieCreateNode(); // original trie
	permute_trie = trieCreateNode(); // trie for permuterm index
	
	printf( "Inserting to trie...\t");
	while (fscanf( fp, "%s", str) == 1) // words file
	{	
		ret = trieInsert( trie, str);
		
		if (ret)
		{
			num_p = make_permuterms( str, permuterms);
			
			for (int i = 0; i < num_p; i++)
				trieInsert( permute_trie, permuterms[i]);
			
			clear_permuterms( permuterms, num_p);
		}
	}
	
	printf( "[done]\n"); // Inserting to trie
	fclose( fp);
	
	printf( "\nQuery: ");
	while (fscanf( stdin, "%s", str) == 1)
	{
		if (strchr( str, '*')) // wildcard search term
		{
			trieSearchWildcard( permute_trie, str);
		}
		else // search term
		{
			ret = trieSearch( trie, str);
			printf( "[%s]%s found!\n", str, ret ? "": " not");
		}
		printf( "\nQuery: ");
	}

	trieDestroy( trie);
	trieDestroy( permute_trie);
	
	return 0;
}

TRIE *trieCreateNode(void) {
    TRIE *trie = (TRIE *)malloc(sizeof(TRIE));

    if (trie == NULL)
        return NULL;
    
    trie->entry = NULL;

	for (int i = 0; i < MAX_DEGREE; i++)
		trie->subtrees[i] = NULL;

	return trie;
}

void trieDestroy( TRIE *root) {
	if (root == NULL)
		return;

	for (int i = 0; i < MAX_DEGREE; i++) {
		if (*(root->subtrees + i))
			trieDestroy(*(root->subtrees + i));
	}
	
	if (root->entry != NULL)
		free(root->entry);

	free(root);
}

int trieInsert( TRIE *root, char *str) {
	if (!str)
		return 0;

	TRIE *pos = root;
	
	for (int i = 0 ; i < strlen(str); i++) {
		str[i] = tolower(str[i]);

		if (getIndex(str[i]) < 0 || getIndex(str[i]) > 26)
			return 0;
	}

	for (int i = 0; i < strlen(str); i++) {
		if (pos->subtrees[getIndex(str[i])] == NULL)
			pos->subtrees[getIndex(str[i])] = trieCreateNode();

		pos = pos->subtrees[getIndex(str[i])];
	}

	if(pos->entry == NULL) {
		pos->entry = strdup(str);

		return 1;
	}

	return 0;
}

int trieSearch( TRIE *root, char *str) {
	TRIE *pos = root;
	int len = strlen(str);

	for (int i = 0 ; i < strlen(str); i++) {
		if (getIndex(str[i]) < 0 || getIndex(str[i]) > 26)
			return 0;
	}

	for (int i = 0; i < len; i++) {
		if (pos->subtrees[getIndex(str[i])] == NULL)
			return 0;

		pos = pos->subtrees[getIndex(str[i])];
	}

	if (pos->entry)
		return 1;
	
	return 0;
}

void trieList( TRIE *root) {
	if (root->entry != NULL)
		printf("%s\n", root->entry);
	for (int i = 0; i < MAX_DEGREE; i++) {
		if (*(root->subtrees + i) != NULL)
			trieList(*(root->subtrees + i));
	}
}

void triePrefixList( TRIE *root, char *str) {
	TRIE *pos = root;
	int len = strlen(str);

	for (int i = 0 ; i < strlen(str); i++) {
		if (getIndex(str[i]) < 0 || getIndex(str[i]) > 26)
			return;
	}

	for (int i = 0; i < len; i++) {
		if (pos->subtrees[getIndex(str[i])] == NULL)
			return;

		pos = pos->subtrees[getIndex(str[i])];
	}

	trieList(pos);
}

int make_permuterms( char *str, char *permuterms[]) {
	int len = strlen(str);
	char ch[2] = {0, 0};
	char *pmt;
	char *tstr;
	
	str[len++] = '$';
	str[len] = '\0';

	permuterms[0] = strdup(str);

	pmt = strdup(str);
	tstr = strdup(str);

	for (int i = 1; i < len; i++) {
		strncpy(ch, tstr, 1);
		strcpy(pmt, tstr + 1);
		strcat(pmt, ch);
		permuterms[i] = strdup(pmt);
		strcpy(tstr, pmt);
	}

	free(pmt);
	free(tstr);

	return len;
}

void clear_permuterms( char *permuterms[], int size) {
	for (int i = 0; i < size; i++)
		free(permuterms[i]);
}

void trieSearchWildcard( TRIE *root, char *str) {
	int len = strlen(str);
	char ch[2] = {0, 0};
	char *pmt;
	char *tstr;

	if (str[0] == '*' && str[len - 1] == '*') {
		for (int i = 1; i <= len; i++) {
			str[i - 1] = str[i];
		}
		len--;
	}

	str[len++] = '$';
	str[len] = '\0';
	pmt = strdup(str);
	tstr = strdup(str);

	while(1) {
		if (pmt[len - 1] == '*') {
			pmt[len - 1] = '\0';

			triePrefixList( root, pmt);
			free(pmt);
			free(tstr);

			return;
		}
		strncpy(ch, tstr, 1);
		strcpy(pmt, tstr + 1);
		strcat(pmt, ch);
		strcpy(tstr, pmt);
	}
}