#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int **Creatematrix(int size);

void DFS(int **adj, int size);

void BFS(int **adj, int size);

int Search(int* list, int count, int key);

int QSearch(int* list, int front, int rear, int size, int key);

int main(int argc, char **argv) {
    char str[100];
    int **adj;
    int row, col, size;
    FILE *fp;

    if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 1;
	}
	
    fp = fopen(argv[1], "rt");
    if (fp == NULL)
	{
		fprintf( stderr, "File open error: %s\n", argv[1]);
		return 1;
	}
    
    fscanf(fp, "%s", str);
    fscanf(fp, "%s", str);
    size = atoi(str);
    adj = Creatematrix(size);

    while (fscanf(fp, "%s", str) == 1) {
        if (strchr(str, '*'))
            continue;
        else
        {
            row = atoi(str);
            fscanf(fp, "%s", str);
            col = atoi(str);
            adj[col - 1][row - 1] = 1;
            adj[row - 1][col - 1] = 1;
        }
    }
    
    fclose(fp);

    DFS(adj, size);
    BFS(adj, size);

    free(adj[0]);
    free(adj);
    
    return 1;
}

int **Creatematrix(int size) {
    int **adj = (int **)malloc(sizeof(int *) * size);
    adj[0] = (int *)malloc(sizeof(int) * size * size);

    for (int i = 1; i < size; i++) {
        adj[i] = adj[i - 1] + size;
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++)
            adj[j][i] = 0;
    }

    return adj;
}

void DFS(int **adj, int size) {
    int stack[size];
    int pro[size];
    int v = 0;
    int top = -1;
    int cnt = 0;

    printf("DFS : ");
    stack[++top] = v;           //push
    while (top != -1) {
        v = stack[top--];       //pop
        pro[cnt++] = v;         
        printf("%d ", v + 1);

        for (int ver = 0; ver < size; ver++) { 
            if (adj[ver][v] == 1) {
                if ( ( !Search(stack, top, ver))&&( !Search(pro, cnt - 1, ver)))
                    stack[++top] = ver;
            }
        }
    }
    printf("\n");
}

void BFS(int **adj, int size) {
    int queue[size];    
    int pro[size];
    int front = 0;
    int rear = 0;
    int v = 0;
    int cnt = 0;

    printf("BFS : ");
    queue[rear] = v;
    rear = ++rear % size;
    while (front != rear) {
        v = queue[front];
        front = ++front % size;
        pro[cnt++] = v;
        printf("%d ", v + 1);

        for (int ver = 0; ver < size; ver++) { 
            if (adj[ver][v] == 1) {
                if ( ( !QSearch(queue, front, rear, size, ver))&&( !Search(pro, cnt - 1, ver))) {
                    queue[rear] = ver;
                    rear = ++rear % size;
                }
            }
        }
    }
    printf("\n");
}

int Search(int* list, int count, int key) {
    for (int i = 0; i <= count; i++) {
        if (list[i] == key)
            return 1;
    }

    return 0;
}

int QSearch(int* list, int front, int rear, int size, int key) {
    for (int i = front; i != rear; i = ++i % size) {
        if (list[i] == key)
            return 1;
    }

    return 0;
}