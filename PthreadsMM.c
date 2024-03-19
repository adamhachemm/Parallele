#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_SIZE 100

int M, N, P;
int A[MAX_SIZE][MAX_SIZE];
int B[MAX_SIZE][MAX_SIZE];
int C[MAX_SIZE][MAX_SIZE];

struct ThreadData {
    int i, j;
};

void* multiply(void* arg) {
    struct ThreadData* data = (struct ThreadData*)arg;
    int i = data->i;
    int j = data->j;

    for (int k = 0; k < N; k++) {
        C[i][j] += A[i][k] * B[k][j];
    }

    return NULL;
}

int main() {
    printf("Enter dimensions M, N, P: ");
    scanf("%d %d %d", &M, &N, &P);

    printf("Enter matrix A(%d x %d):\n", M, N);
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            scanf("%d", &A[i][j]);
        }
    }

    printf("Enter matrix B(%d x %d):\n", N, P);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < P; j++) {
            scanf("%d", &B[i][j]);
        }
    }

    pthread_t threads[MAX_SIZE][MAX_SIZE];

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < P; j++) {
            struct ThreadData* data = (struct ThreadData*)malloc(sizeof(struct ThreadData));
            data->i = i;
            data->j = j;
            pthread_create(&threads[i][j], NULL, multiply, data);
        }
    }

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < P; j++) {
            pthread_join(threads[i][j], NULL);
        }
    }

    printf("Resultant Matrix:\n");
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < P; j++) {
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }

    return 0;
}
