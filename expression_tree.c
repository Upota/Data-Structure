#include <stdlib.h> // malloc, atoi
#include <stdio.h>
#include <ctype.h> // isdigit
#include <assert.h> // assert

#define MAX_STACK_SIZE	50

///////////////////////////////////////////////////////////////////////////////
// LIST type definition


typedef struct node
{
	char		data;
	struct node	*left;
	struct node	*right;
} NODE;

typedef struct
{
	NODE	*root;
} TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *createTree( void);

/* Deletes all data in tree and recycles memory
	return	NULL head pointer
*/
TREE *destroyTree( TREE *pTree);

static void _destroy( NODE *root);

/*  Allocates dynamic memory for a node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
static NODE *_makeNode( char ch);

/* converts postfix expression to binary tree
	return	1 success
			0 invalid postfix expression
*/
int postfix2tree( char *expr, TREE *pTree);

/* Print node in tree using inorder traversal
*/
void traverseTree( TREE *pTree);

/* internal traversal function
	an implementation of ALGORITHM 6-6
*/
static void _traverse( NODE *root);

/* Print tree using inorder right-to-left traversal
*/
void printTree( TREE *pTree);

/* internal traversal function
*/
static void _infix_print( NODE *root, int level);

/* evaluate postfix expression
	return	value of expression
*/
float evalPostfix( char *expr);

/////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	TREE *tree;
	char expr[1024];
	
	fprintf( stdout, "\nInput an expression (postfix): ");
	
	while (fscanf( stdin, "%s", expr) == 1)
	{
		// creates a null tree
		tree = createTree();
		
		if (!tree)
		{
			printf( "Cannot create tree\n");
			return 100;
		}
		
		// postfix expression -> expression tree
		int ret = postfix2tree( expr, tree);
		if (!ret)
		{
			fprintf( stdout, "invalid expression!\n");
			continue;
		}
		
		// expression tree -> infix expression
		fprintf( stdout, "\nInfix expression : ");
		traverseTree( tree);
		
		// print tree with right-to-left infix traversal
		fprintf( stdout, "\n\nTree representation:\n");
		printTree( tree);
		
		// evaluate postfix expression
		float val = evalPostfix( expr);
		fprintf( stdout, "\nValue = %f\n", val);
		
		// destroy tree
		destroyTree( tree);
		
		fprintf( stdout, "\nInput an expression (postfix): ");
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////

TREE *createTree( void) {
	TREE *tree = (TREE *)malloc(sizeof(TREE));

	if(tree == NULL)
		return tree;
	
	tree->root = NULL;

	return tree;
}

TREE *destroyTree( TREE *pTree)
{
	if (pTree)
	{
		_destroy( pTree->root);
	}
		
	free( pTree);
	
	return NULL;	
}

static void _destroy( NODE *root) {
	if(root != NULL) {
		_destroy(root->left);
		_destroy(root->right);
		free(root);
		root = NULL;
	}
}

static NODE *_makeNode( char ch) {
	NODE *newNode = (NODE *)malloc(sizeof(NODE));

	if (newNode == NULL)
		return NULL;
	
	newNode->data = ch;
	newNode->left = NULL;
	newNode ->right = NULL;

	return newNode;
}

int postfix2tree( char *expr, TREE *pTree) {
	NODE *STACK[MAX_STACK_SIZE];
	int stacktop;

	for (int i = 0, top = 0; expr[i] != '\0'; top++, i++) {
		STACK[top] = _makeNode(expr[i]);
		
		if (!isdigit(expr[i])) {
			switch(expr[i]) {
				case '+': break;
				case '-': break;
				case '*': break;
				case '/': break;
				default:
					goto NonOperator;
			}

			if (top > 1) {
				STACK[top]->right = STACK[top-1];
				STACK[top]->left = STACK[top-2];
				STACK[top-2] = STACK[top];
				top -= 2;
			}
			else {
NonOperator:
				while(top + 1) {
					_destroy(STACK[top]);
					top--;
				}
				free( pTree);

				return 0;
			}
		}
		stacktop = top;
	}

	if (stacktop != 0) {
		while(stacktop + 1) {
			_destroy(STACK[stacktop]);
			stacktop--;
		}
		free( pTree);

		return 0;
	}

	pTree->root = STACK[stacktop];

	return 1;
}

void traverseTree( TREE *pTree)
{
	_traverse (pTree->root);
	
	return;
}

static void _traverse( NODE *root) {
	if (root != NULL) {
		if (isdigit(root->data))
			printf("%c", root->data);
		else {
			printf("(");
			_traverse(root->left);
			printf("%c", root->data);
			_traverse(root->right);
			printf(")");
		}
	}
}

void printTree( TREE *pTree)
{
	_infix_print(pTree->root, 0);
	
	return;
}

static void _infix_print( NODE *root, int level) {
	if (root != NULL) {
		_infix_print(root->right, level + 1);
		
		for (int i = 0; i <level; i++)
			printf("\t");

		printf("%c\n", root->data);

		_infix_print(root->left, level + 1);
	}
}

float evalPostfix( char *expr) {
	float STACK[MAX_STACK_SIZE];
	float *val;

	for (int i = 0, top = 0; expr[i] != '\0'; top++, i++) {
		if (isdigit(expr[i])) {
			char num = expr[i];
			STACK[top] = (float)atoi(&num);
		}
		else {
			switch(expr[i]) {
				case '+':
					STACK[top-2] = STACK[top-1] + STACK[top-2];
					break;
				case '-':
					STACK[top-2] = STACK[top-1] - STACK[top-2];
					break;
				case '*':
					STACK[top-2] = STACK[top-1] * STACK[top-2];
					break;
				case '/':
					STACK[top-2] = STACK[top-1] / STACK[top-2];
					break;
			}
			top -= 2;
		}
	}

	return STACK[0];
}