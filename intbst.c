#include <stdlib.h> // malloc, atoi, rand
#include <stdio.h>
#include <ctype.h> // isdigit
#include <assert.h> // assert
#include <time.h> // time

////////////////////////////////////////////////////////////////////////////////
// TREE type definition
typedef struct node
{
	int			data;
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
TREE *BST_Create( void);

/* Deletes all data in tree and recycles memory
	return	NULL head pointer
*/
TREE *BST_Destroy( TREE *pTree);
static void _destroy( NODE *root);

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int BST_Insert( TREE *pTree, int data);

static NODE *_makeNode( int data);

/* Deletes a node with dltKey from the tree
	return	1 success
			0 not found
*/
int BST_Delete( TREE *pTree, int dltKey);

/* internal function
	success is 1 if deleted; 0 if not
	return	pointer to root
*/
NODE *_delete( NODE *root, int dltKey, int *success);

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
int *BST_Retrieve( TREE *pTree, int key);

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
NODE *_retrieve( NODE *root, int key);

/* prints tree using inorder traversal
*/
void BST_Traverse( TREE *pTree);
static void _traverse( NODE *root);

/* Print tree using inorder right-to-left traversal
*/
void printTree( TREE *pTree);
/* internal traversal function
*/
static void _infix_print( NODE *root, int level);


////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	TREE *tree;
	int data;
	
	// creates a null tree
	tree = BST_Create();
	
	if (!tree)
	{
		printf( "Cannot create tree\n");
		return 100;
	}

	fprintf( stdout, "Inserting: ");
	
	srand( time(NULL));
	for (int i = 1; i < 20; i++)
	{
		data = rand() % 100 + 1; // 1 ~ 100 random number
		
		fprintf( stdout, "%d ", data);
		
		// insert funtion call
		BST_Insert( tree, data);
 	}
	fprintf( stdout, "\n");
			
	// inorder traversal
	fprintf( stdout, "Inorder traversal: ");
	BST_Traverse( tree);
	fprintf( stdout, "\n");
	
	// print tree with right-to-left infix traversal
	fprintf( stdout, "Tree representation:\n");
	printTree(tree);
	
	int ret;
	do
	{
		fprintf( stdout, "Input a number to delete: "); 
		int num;
		ret = scanf( "%d", &num);
		if (ret != 1) break;
		
		ret = BST_Delete( tree, num);
		if (!ret) fprintf( stdout, "%d not found\n", num);
		
		// print tree with right-to-left infix traversal
		fprintf( stdout, "Tree representation:\n");
		printTree(tree);
		
	} while(1);
	
	BST_Destroy( tree);

	return 0;
}
///////////////////////////////////////////////////////////////////////////////

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *BST_Create( void) {
	TREE *pTree = (TREE *)malloc(sizeof(TREE));

	if (pTree == NULL)
		return pTree;

	pTree->root = NULL;

	return pTree;
}

/* Deletes all data in tree and recycles memory
	return	NULL head pointer
*/
TREE *BST_Destroy( TREE *pTree) {
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

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int BST_Insert( TREE *pTree, int data) {
	NODE *pos = NULL;
	NODE *newNode = _makeNode(data);

	pos = pTree->root;

	if (newNode == NULL)
		return 0;

	if (pTree->root == NULL) {
		pTree->root = newNode;
		return 1;
	}

	while (1) {
		if (data < pos->data) {
			if( pos->left == NULL) {
				pos->left = newNode;
				break;
			}
			else
				pos = pos->left;
		}
		else {
			if( pos->right == NULL) {
				pos->right = newNode;
				break;
			}
			else
				pos = pos->right;
		}
	}
	

	return 1;
}

static NODE *_makeNode( int data) {
	NODE *newNode = (NODE *)malloc(sizeof(NODE));

	if (newNode == NULL)
		return NULL;
	
	newNode->data = data;
	newNode->left = NULL;
	newNode ->right = NULL;

	return newNode;
}

/* Deletes a node with dltKey from the tree
	return	1 success
			0 not found
*/
int BST_Delete( TREE *pTree, int dltKey) {
	int ret_val;
	NODE *pos = pTree->root;

	if(pos == NULL)
		return 0;
	
	if (pos->data == dltKey) {
		if (pos->left == NULL) {
			pTree->root = pos->right;
			free(pos);
			pos = NULL;
		}
		else if (pos->right == NULL) {
			pTree->root = pos->left;
			free(pos);
			pos = NULL;
		}
		else
			_delete(pos, dltKey, &ret_val);

		return 1;
	}
	else 
		_delete(pos, dltKey, &ret_val);

	return ret_val;
}

/* internal function
	success is 1 if deleted; 0 if not
	return	pointer to root
*/
NODE *_delete( NODE *root, int dltKey, int *success) {
	NODE *pos = NULL;

	if (root == NULL) {
		*success = 0;
		return root;
	}

	if (dltKey < root->data)
		root->left = _delete(root->left, dltKey, success);
	else if (dltKey > root->data)
		root->right = _delete(root->right, dltKey, success);
	else {
		if (root->left == NULL) {
			pos = root->right;
			free(root);
			root = NULL;
			*success = 1;
			return pos;
		}
		else if (root->right == NULL) {
			pos = root->left;
			free(root);
			root = NULL;
			*success = 1;

			return pos;
		}
		else {
			pos = root->left;
			while (pos->right != NULL)	pos = pos->right;
			root->data = pos->data;

			root->left = _delete(root->left, pos->data, success);
		}
	}
	return root;
}

/* Retrieve tree for the node containing the requested key
	return	address of data of the node containing the key
			NULL not found
*/
int *BST_Retrieve( TREE *pTree, int key) {
	NODE *ret_node = _retrieve(pTree->root, key);
	
	if (ret_node == NULL)
		return NULL;

	return &(ret_node->data);
}

/* internal function
	Retrieve node containing the requested key
	return	address of the node containing the key
			NULL not found
*/
NODE *_retrieve( NODE *root, int key) {
	if (root == NULL) 
		return NULL;

	if (key == root->data)
		return root;
	else if (key < root->data)
		return _retrieve(root->left, key);
	else
		return _retrieve(root->right, key);
}

/* prints tree using inorder traversal
*/
void BST_Traverse( TREE *pTree) {
	_traverse (pTree->root);
	
	return;
}

static void _traverse( NODE *root) {
	if (root != NULL) {
		_traverse(root->left);
		printf("%d ", root->data);
		_traverse(root->right);
	}
}

/* Print tree using inorder right-to-left traversal
*/
void printTree( TREE *pTree) {
	_infix_print(pTree->root, 0);
	
	return;
}

static void _infix_print( NODE *root, int level) {
	if (root != NULL) {
		_infix_print(root->right, level + 1);
		
		for (int i = 0; i <level; i++)
			printf("\t");

		printf("%d\n", root->data);

		_infix_print(root->left, level + 1);
	}
}
