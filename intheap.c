#include <stdio.h>
#include <stdlib.h> // malloc, rand
#include <time.h> // time
#include <assert.h>

#define MAX_ELEM	20

typedef struct
{
	int *heapArr;
	int	last;
	int	capacity;
} HEAP;

HEAP *heapCreate( int capacity);
void heapDestroy( HEAP *heap);
int heapInsert( HEAP *heap, int data);
static void _reheapUp( HEAP *heap, int index);
int heapDelete( HEAP *heap, int* data);
static void _reheapDown( HEAP *heap, int index);
void heapPrint( HEAP *heap);

int main(void)
{
	HEAP *heap;
	int data;
	
	heap = heapCreate(MAX_ELEM);
	
	srand( time(NULL));
	
	for (int i = 1; i < MAX_ELEM; i++)
	{
		data = rand() % MAX_ELEM * 3 + 1; // 1 ~ MAX_ELEM*3 random number
				
		fprintf( stdout, "Inserting %2d: ", data);
		
		// insert function call
		heapInsert( heap, data);
		
		heapPrint( heap);
 	}

	while (heap->last > 0)
	{
		// delete function call
		heapDelete( heap, &data);

		fprintf( stdout, "Deleting %2d: ", data);

		heapPrint( heap);
 	}
	
	heapDestroy( heap);
	
	return 0;
}

HEAP *heapCreate( int capacity) {
	HEAP *heap = (HEAP *)malloc(sizeof(HEAP));

	if (heap == NULL)
		return NULL;

	heap->heapArr = (int *)malloc(sizeof(int)*capacity);
	heap->last = 0;
	heap ->capacity = capacity;

	return heap;
}

void heapDestroy( HEAP *heap) {
	for (int i = heap->capacity - 1; i < 0; i--)	free(heap->heapArr + i);
	free(heap->heapArr);
	free(heap);
}

int heapInsert( HEAP *heap, int data) {
	if (heap->last == heap->capacity)
		return 0;

	heap->heapArr[++(heap->last)] = data;

	if (heap->last > 1)
		_reheapUp(heap, heap->last);

	return 1;
}

static void _reheapUp( HEAP *heap, int index) {
	int temp;
	while (index != 1 && heap->heapArr[index] > heap->heapArr[index / 2]) {
		temp = heap->heapArr[index];
		heap->heapArr[index] = heap->heapArr[index / 2];
		heap->heapArr[index / 2] = temp;
		
		index /= 2;
	}
}

int heapDelete( HEAP *heap, int* data) {
	if (heap->last == 0)
		return 0;
	
	*data = heap->heapArr[1];
	heap->heapArr[1] = heap->heapArr[heap->last--];

	_reheapDown(heap, 1);

	return 1;
}

static void _reheapDown( HEAP *heap, int index) {
	int temp, left, right, larger;

	if (2 * index  <= heap->last) {
		left = heap->heapArr[2 * index];
	
		if (2 * index + 1 <= heap->last) {
			right = heap->heapArr[2 * index + 1];
		}
		else
			right = 0;
		
		if (left > right)
			larger = 2 * index;
		else
			larger = 2 * index + 1;

		if (heap->heapArr[index] < heap->heapArr[larger]) {
			temp = heap->heapArr[index];
			heap->heapArr[index] = heap->heapArr[larger];
			heap->heapArr[larger] = temp;

			_reheapDown(heap, larger);
		}
	}
}

void heapPrint( HEAP *heap) {
	for (int i = 1; i <= heap->last; i++)	printf("%6d", heap->heapArr[i]);
	printf("\n");
}