#include <stdio.h>
#include <stdlib.h> 

void printMatrix(const int* matrix, int n);
void rotateMatrix(int* matrix, int n);
void smoothMatrix(int* matrix, int n);

int main() {
    int n;
    printf("Enter matrix size (2-10): ");
    scanf("%d", &n);

    if (n < 2 || n > 10) {
        fprintf(stderr, "Error: Matrix size must be between 2 and 10.\n");
        return 1; 
    }

    int a[n][n];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            a[i][j] = rand() % 256;
        }
    }

    int *ptr = (int *)a;

    printf("\nOriginal Randomly Generated Matrix:\n");
    printMatrix(ptr, n);

    //Rotate Matrix 90° Clockwise 
    rotateMatrix(ptr, n);
    printf("\nMatrix after 90° Clockwise Rotation:\n");
    printMatrix(ptr, n);

    // Apply 3x3 Smoothing Filter
    smoothMatrix(ptr, n);
    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    printMatrix(ptr, n);

    return 0;
}

void printMatrix(const int* matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d", *(matrix + i * n + j));
        }
        printf("\n");
    }
}

void rotateMatrixTwoStep(int* matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            int* p1 = matrix + i * n + j; 
            int* p2 = matrix + j * n + i; 
            int temp = *p1;
            *p1 = *p2;
            *p2 = temp;
        }
    }

    for (int i = 0; i < n; i++) {
        int* left = matrix + i * n;
        int* right = matrix + i * n + (n - 1); 
        while (left < right) {
            int temp = *left;
            *left = *right;
            *right = temp;
            left++;
            right--;
        }
    }
}

void smoothMatrix(int* matrix, int n) {
    if (n < 2) return;
    int* prevRow = (int*)malloc(n * sizeof(int));
    int* currentRow = (int*)malloc(n * sizeof(int));

    for (int r = 0; r < n; ++r) {
        for (int k = 0; k < n; ++k) {
            *(currentRow + k) = *(matrix + r * n + k);
        }

     
        for (int c = 0; c < n; ++c) {
            long long sum = 0;
            int count = 0;

            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    int nr = r + dr; 
                    int nc = c + dc;

            
                    if (nr >= 0 && nr < n && nc >= 0 && nc < n) {
                        count++;
                      
                        if (dr == -1) {   
                            sum += *(prevRow + nc);
                        } else if (dr == 0) {
                            sum += *(currentRow + nc);
                        } else {     
                            sum += *(matrix + nr * n + nc);
                        }
                    }
                }
            }
           
            *(matrix + r * n + c) = (int)(sum / count);
        }

        for (int k = 0; k < n; ++k) {
            *(prevRow + k) = *(currentRow + k);
        }
    }

    free(prevRow);
    free(currentRow);
}