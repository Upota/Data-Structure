#include <stdlib.h> // malloc
#include <stdio.h>
#include <ctype.h> // toupper

#define QUIT	1
#define INSERT	2
#define DELETE	3
#define PRINT	4
#define SEARCH	5

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	int			data;
	struct node	*link;
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
			1 if dupe key
*/
int addNode( LIST *pList, int dataIn);

/* Removes data from list
	return	0 not found
			1 deleted
*/
int removeNode( LIST *pList, int Key, int *dataOut);

/* interface to search function
	Argu	key being sought
	dataOut	contains found data
	return	1 successful
			0 not found
*/
int searchList( LIST *pList, int Argu, int *dataOut);

/* returns number of nodes in list
*/
int listCount( LIST *pList);

/* returns	1 empty
			0 list has data
*/
int emptyList( LIST *pList);

//int fullList( LIST *pList);

/* prints data from list
*/
void printList( LIST *pList);

/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
static int _insert( LIST *pList, NODE *pPre, int dataIn);

/* internal delete function
	deletes data from a list and saves the (deleted) data to dataOut
*/
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, int *dataOut);

/* internal search function
	searches list and passes back address of node
	containing target and its logical predecessor
	return	1 found
			0 not found
*/
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, int argu);

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
		case 'P':
			return PRINT;
		case 'I':
			return INSERT;
		case 'D':
			return DELETE;
		case 'S':
			return SEARCH;
	}
	return 0; // undefined action
}

////////////////////////////////////////////////////////////////////////////////
int main( void)
{
	int num;
	LIST *pList;
	int data;
	
	pList = createList();

	if ( !pList)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	fprintf( stdout, "Select Q)uit, P)rint, I)nsert, D)elete, or S)earch: ");
	
	while(1)
	{
		int action = get_action();
		
		switch( action)
		{
			case QUIT:
				destroyList( pList);
				return 0;
			
			case PRINT:
				// print function call
				printList( pList);
				break;
				
			case INSERT:
				fprintf( stdout, "Enter a number to insert: ");
				fscanf( stdin, "%d", &num);

				// insert function call
				addNode( pList, num);
				
				// print function call
				printList( pList);
				break;
				
			case DELETE:
				fprintf( stdout, "Enter a number to delete: ");
				fscanf( stdin, "%d", &num);
				
				// delete function call
				removeNode( pList, num, &data);
				// print function call
				printList( pList);
				break;
			
			case SEARCH:
				fprintf( stdout, "Enter a number to retrieve: ");
				fscanf( stdin, "%d", &num);
				
				// search function call
				int found;
				found = searchList( pList, num, &data);
				if (found) fprintf( stdout, "Found: %d\n", data);
				else fprintf( stdout, "Not found: %d\n", num);
				
				break;
		}
		if (action) fprintf( stdout, "Select Q)uit, P)rint, I)nsert, D)elete, or S)earch: ");
		
	}
	
	return 0;
}

LIST *createList( void) {
    LIST *pList = (LIST *)malloc(sizeof(LIST));
	
	if(pList == NULL) 
		return NULL;
	
	pList->count = 0;
    pList->pos = NULL;
    pList->head = NULL;
    pList->rear = NULL;
    return pList;
};

LIST *destroyList( LIST *pList) {
	while(pList->head != NULL) {
		pList->pos = pList->head;
		pList->head = pList->pos->link;
		free(pList->pos);
	}
	free(pList);
	pList = NULL;

	return pList;
};

int addNode( LIST *pList, int dataIn) {
	pList->pos = pList->head;
	NODE *pPre = NULL;
	NODE *pLoc = NULL;
	
	pLoc = pList->pos;

	if( _search(pList, &pPre, &pLoc, dataIn))
		return 1;

	if ( !_insert(pList, pPre, dataIn))
		return -1;
	
	return 0;
};

int removeNode( LIST *pList, int Key, int *dataOut) {
	if ( !emptyList(pList)) {
		pList->pos = pList->head;
		NODE *pPre = NULL;
		NODE *pLoc = NULL;
		
		pLoc = pList->pos;

		if( _search( pList, &pPre, &pLoc, Key)) {
			_delete( pList, pPre, pLoc, dataOut);

			return 1;
		}
	}
		return 0;
};

int searchList( LIST *pList, int Argu, int *dataOut) {
	if( !emptyList(pList)) {
		pList->pos = pList->head;
		NODE *pPre = NULL;
		NODE *pLoc = NULL;
		
		pLoc = pList->pos;

		if( _search(pList, &pPre, &pLoc, Argu)) {
			*dataOut = pLoc->data;
			return 1;
		};
	}

	return 0;
};

int listCount( LIST *pList) {
	return pList->count;
};

int emptyList( LIST *pList) {
	if( listCount( pList) == 0)
		return 1;
	else
		return 0;
};

void printList( LIST *pList) {
	pList->pos = pList->head;
	while(pList->pos != NULL) {
		fprintf( stdout, "%d->", pList->pos->data);
		pList->pos = pList->pos->link;
	}
	fprintf( stdout, "NULL\n");
};

static int _insert( LIST *pList, NODE *pPre, int dataIn) {
	NODE *newNode = (NODE *)malloc(sizeof(NODE));

	if( newNode == NULL)
		return 0;

	newNode->data = dataIn;
	// insert at beginning
	if( pPre == NULL) {
		newNode->link = pList->head;
		pList->head = newNode;
	}
	else {
	// insert in middle, at end
	newNode->link = pPre->link;
	pPre->link = newNode;
	}

	pList->count++;

	return 1;
};

static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, int *dataOut) {
	*dataOut = pLoc->data;
	if( pPre == NULL) {
		pList->head = pLoc->link;
	}
	else {
		pPre->link = pLoc->link; 
	}
	free( pLoc);
	pList->count--;
};

static int _search( LIST *pList, NODE **pPre, NODE **pLoc, int argu) {
	while( *pLoc != NULL) {
		if( (*pLoc)->data == argu)
			return 1;
		else if( (*pLoc)->data > argu)
			break;
	
		*pPre = *pLoc;
		*pLoc = (*pLoc)->link;
	}
	
	return 0;
};