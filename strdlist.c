#include <stdlib.h> // malloc
#include <stdio.h>
#include <string.h> // strdup, strcmp
#include <ctype.h> // toupper

#define QUIT			1
#define FORWARD_PRINT	2
#define BACKWARD_PRINT	3
#define DELETE			4

// User structure type definition
typedef struct 
{
	char	*token;
	int		freq;
} tTOKEN;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tTOKEN		*dataPtr;
	struct node	*llink;
	struct node	*rlink;
} NODE;

typedef struct
{
	int		count;
	NODE	*pos;
	NODE	*head;
	NODE	*rear;
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a list head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
LIST *createList( void);

/* Deletes all data in list and recycles memory
	return	NULL head pointer
*/
LIST *destroyList( LIST *pList);

/* Inserts data into list
	return	-1 if overflow
			0 if successful
			1 if duplicated key
*/
int addNode( LIST *pList, tTOKEN *dataInPtr);

/* Removes data from list
	return	0 not found
			1 deleted
*/
int removeNode( LIST *pList, char *keyPtr, tTOKEN **dataOut);

/* interface to search function
	Argu	key being sought
	dataOut	contains found data
	return	1 successful
			0 not found
*/
int searchList( LIST *pList, char *pArgu, tTOKEN **pDataOut);

/* returns number of nodes in list
*/
int listCount( LIST *pList);

/* returns	1 empty
			0 list has data
*/
int emptyList( LIST *pList);

//int fullList( LIST *pList);

/* prints data from list (forward)
*/
void printList( LIST *pList);

/* prints data from list (backward)
*/
void printListR( LIST *pList);

/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
static int _insert( LIST *pList, NODE *pPre, tTOKEN *dataInPtr);

/* internal delete function
	deletes data from a list and saves the (deleted) data to dataOut
*/
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr);

/* internal search function
	searches list and passes back address of node
	containing target and its logical predecessor
	return	1 found
			0 not found
*/
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, char *pArgu);

/* Allocates dynamic memory for a token structure, initialize fields(token, freq) and returns its address to caller
	return	token structure pointer
			NULL if overflow
*/
tTOKEN *createToken( char *str);

/* Deletes all data in token structure and recycles memory
	return	NULL head pointer
*/
tTOKEN *destroyToken( tTOKEN *pToken);

/* gets user's input
*/
int get_action()
{
	char ch;
	scanf( "%c", &ch);
	ch = toupper( ch);
	switch( ch)
	{
		case 'Q':
			return QUIT;
		case 'F':
			return FORWARD_PRINT;
		case 'B':
			return BACKWARD_PRINT;
		case 'D':
			return DELETE;
	}
	return 0; // undefined action
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	char str[1024];
	tTOKEN *pToken;
	int ret;
	FILE *fp;
	
	if (argc != 2)
	{
		fprintf( stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (!fp)
	{
		fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}
	
	// creates a null list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	while(fscanf( fp, "%s", str) == 1)
	{
		pToken = createToken( str);
		
		// insert function call
		ret = addNode( list, pToken);
		
		if (ret == 1) // duplicated 
			destroyToken( pToken);
	}
	
	fclose( fp);
	
	fprintf( stdout, "Select Q)uit, F)orward print, B)ackward print, D)elete: ");
	
	while (1)
	{
		int action = get_action();
		
		switch( action)
		{
			case QUIT:
				destroyList( list);
				return 0;
			
			case FORWARD_PRINT:
				printList( list);
				break;
			
			case BACKWARD_PRINT:
				printListR( list);
				break;
			
			case DELETE:
				fprintf( stdout, "Input a string to delete: ");
				fscanf( stdin, "%s", str);
				int ret = removeNode( list, str, &pToken);
				if (ret)
				{
					fprintf( stdout, "%s deleted\n", pToken->token);
					destroyToken( pToken);
				}
				else fprintf( stdout, "%s not found\n", str);
				break;
			}
		
		if (action) fprintf( stdout, "Select Q)uit, F)orward print, B)ackward print, D)elete: ");
	}
	return 0;
}

LIST *createList( void) {
	LIST *list = (LIST *)malloc(sizeof(LIST));
	
	if(list == NULL)
		return NULL;

	list->count = 0;
	list->pos = NULL;
	list->head = NULL;
	list->rear = NULL;

	return list;
};

LIST *destroyList( LIST *pList) {
	while(pList->head != NULL) {
		pList->pos = pList->head;
		pList->head = pList->pos->rlink;
		free(pList->pos->dataPtr->token);
		free(pList->pos->dataPtr);
		free(pList->pos);
	}
	free(pList);
	pList = NULL;

	return pList;
};

int addNode( LIST *pList, tTOKEN *dataInPtr) {
	pList->pos = pList->head;
	NODE *pLoc = NULL;
	NODE *pPre = NULL;
	
	pLoc = pList->pos;

	if( _search( pList, &pPre, &pLoc, dataInPtr->token)) {
		pLoc->dataPtr->freq++;

		return 1;
	}

	if( !_insert( pList, pPre, dataInPtr))
		return -1;

	return 0;
};

int removeNode( LIST *pList, char *keyPtr, tTOKEN **dataOut) {
	if ( !emptyList(pList)) {
		pList->pos = pList->head;
		NODE *pLoc = NULL;
		NODE *pPre = NULL;
		
		pLoc = pList->pos;

		if( _search( pList, &pPre, &pLoc, keyPtr)) {
			_delete( pList, pPre, pLoc, dataOut);

			return 1;
		}
	}
		return 0;
};

int searchList( LIST *pList, char *pArgu, tTOKEN **pDataOut) {
	if( !emptyList(pList)) {
		pList->pos = pList->head;
		NODE *pLoc = NULL;
		NODE *pPre = NULL;

		pLoc = pList->pos;

		if( _search(pList, &pPre, &pLoc, pArgu)) {
			*pDataOut = pLoc->dataPtr;
			return 1;
		}
	}
	return 0;
};

int listCount( LIST *pList) {
	return pList->count;
};

int emptyList( LIST *pList) {
	if( listCount( pList) == 0)
		return 1;
	
	return 0;
};

void printList( LIST *pList) {
	pList->pos = pList->head;
	while(pList->pos != NULL) {
		printf("%s\t%d\n", pList->pos->dataPtr->token, pList->pos->dataPtr->freq);
		pList->pos = pList->pos->rlink;
	}
};

void printListR ( LIST *pList) {
	pList->pos = pList->rear;
	while(pList->pos != NULL) {
		printf("%s\t%d\n", pList->pos->dataPtr->token, pList->pos->dataPtr->freq);
		pList->pos = pList->pos->llink;
	}
};

static int _insert( LIST *pList, NODE *pPre, tTOKEN *dataInPtr) {
	NODE *newNode = (NODE *)malloc(sizeof(NODE));

	if(newNode == NULL)
		return 0;

	newNode->dataPtr = dataInPtr;

	if( pPre == NULL) {
		newNode->rlink = pList->head;
		newNode->llink = NULL;
		pList->head = newNode;
	}
	else {
		newNode->rlink = pPre->rlink;
		newNode->llink = pPre;
		pPre->rlink = newNode;
	}

	if( newNode->rlink == NULL)
		pList->rear = newNode;
	else {
		newNode->rlink->llink = newNode;
	}
	
	pList->count++;

	return 1;
};

static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr) {
	*dataOutPtr = pLoc->dataPtr;

	if( pPre == NULL) {
		pList->head = pLoc->rlink;
		pList->head->llink = NULL;
	}
	else {
		pPre->rlink = pLoc->rlink;
		if(pLoc->rlink == NULL) {
			pList->rear = pPre;
		}
		else {
			pLoc->rlink->llink = pPre;
		}
	}

	free( pLoc);

	pList->count--;
};

static int _search( LIST *pList, NODE **pPre, NODE **pLoc, char *pArgu) {
	while( *pLoc != NULL) {
		int cmp = strcmp( pArgu, (*pLoc)->dataPtr->token);

		if( cmp == 0)  {
			return 1;
		}
		else if( cmp < 0)
			break;

		*pPre = *pLoc;
		*pLoc = (*pLoc)->rlink;
	}
	
	return 0;
};

tTOKEN *createToken( char *str) {
	tTOKEN *Token = (tTOKEN *)malloc(sizeof(tTOKEN));
	
	if( Token == NULL)
		return NULL;
	Token->token = strdup(str);
	Token->freq = 1;

	return Token;
};

tTOKEN *destroyToken( tTOKEN *pToken) {
	free(pToken->token);
	free(pToken);
	pToken = NULL;

	return pToken;
};