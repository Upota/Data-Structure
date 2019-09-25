
#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW
#define EOW			'$' // end of word

// used in the following functions: trieInsert, trieSearch, triePrefixList
#define getIndex(x)		(((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a'))

// TRIE type definition
typedef struct trieNode {
	int 			index; // 0, 1, 2, ...
	struct trieNode	*subtrees[MAX_DEGREE];
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
TRIE *trieCreateNode(void) {
    TRIE *trie = (TRIE *)malloc(sizeof(TRIE));

    if (trie == NULL)
        return NULL;
    
    trie->index = -1;

	for (int i = 0; i < MAX_DEGREE; i++)
		trie->subtrees[i] = NULL;

	return trie;
}

/* Deletes all data in trie and recycles memory
*/
void trieDestroy( TRIE *root) {
	if (root == NULL)
		return;

	for (int i = 0; i < MAX_DEGREE; i++) {
		if (*(root->subtrees + i))
			trieDestroy(*(root->subtrees + i));
	}
	
	free(root);
}

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert( TRIE *root, char *str, int dic_index) {
	if (!str)
		return 0;

	TRIE *pos = root;
	
	for (int i = 0 ; i < strlen(str); i++) {
		if (getIndex(str[i]) < 0 || getIndex(str[i]) > 26)
			return 0;
	}

	for (int i = 0; i < strlen(str); i++) {
		if (pos->subtrees[getIndex(str[i])] == NULL)
			pos->subtrees[getIndex(str[i])] = trieCreateNode();

		pos = pos->subtrees[getIndex(str[i])];
	}

	if (pos->index == -1) {
		pos->index = dic_index;

		return 1;
	}

	return 0;
}

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
int trieSearch( TRIE *root, char *str) {
	TRIE *pos = root;
	int len = strlen(str);

	for (int i = 0 ; i < len; i++) {
		if (getIndex(str[i]) < 0 || getIndex(str[i]) > 26)
			return -1;
	}

	for (int i = 0; i < len; i++) {
		if (pos->subtrees[getIndex(str[i])] == NULL)
			return -1;

		pos = pos->subtrees[getIndex(str[i])];
	}

	if (pos->index != -1)
		return pos->index;
	
	return -1;
}

/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root) {
	if (root->index != -1)
		printf("%d\n", root->index);
	for (int i = 0; i < MAX_DEGREE; i++) {
		if (*(root->subtrees + i) != NULL)
			trieList(*(root->subtrees + i));
	}
}

/* prints all entries starting with str (as prefix) in trie
   ex) "abb" -> "abbas", "abbasid", "abbess", ...
	using trieList function
*/
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

/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
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

/* recycles memory for permuterms
*/
void clear_permuterms( char *permuterms[], int size) {
	for (int i = 0; i < size; i++)
		free(permuterms[i]);
}

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	using triePrefixList function
*/
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

/* makes a trie for given dictionary file
	return	trie head node pointer
			NULL failure
*/ 
TRIE *dic2trie( char *dicfile) {
	TRIE *trie;
	char str[100];
	FILE *fp;
	int num_p;
	int dic_index = -1;
	
	fp = fopen(dicfile, "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "File open error: %s\n", dicfile);
		return NULL;
	}
	
	trie = trieCreateNode(); // original trie
	
	printf( "Inserting to trie...\t");
	while (fscanf( fp, "%s", str) == 1) // words file
	{	
		dic_index++;
		trieInsert( trie, str, dic_index);
	}
	
	printf( "[done]\n"); // Inserting to trie
	fclose( fp);

	return trie;
}


/* makes a permuterm trie for given dictionary file
	return	trie head node pointer
			NULL failure
*/ 
TRIE *dic2permute_trie( char *dicfile) {
	TRIE *trie;
	TRIE *permute_trie;
	int ret;
	char str[100];
	FILE *fp;
	char *permuterms[100];
	int num_p;
	int dic_index = -1;
	
	fp = fopen(dicfile, "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "File open error: %s\n", dicfile);
		return NULL;
	}
	
	trie = trieCreateNode(); // original trie
	permute_trie = trieCreateNode(); // trie for permuterm index
	
	printf( "Inserting to trie...\t");
	while (fscanf( fp, "%s", str) == 1) // words file
	{	
		dic_index++;
		ret = trieInsert( trie, str, dic_index);
		
		if (ret)
		{
			num_p = make_permuterms( str, permuterms);
			
			for (int i = 0; i < num_p; i++)
				trieInsert( permute_trie, permuterms[i], dic_index);
			
			clear_permuterms( permuterms, num_p);
		}
	}
	
	printf( "[done]\n"); // Inserting to trie
	fclose( fp);

	return permute_trie;
}
