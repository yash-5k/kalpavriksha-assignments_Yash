#include <stdio.h>
#include <stdlib.h>
#include <time.h>   

void printMatrix(int** matrix, int dimension);
void rotateMatrix(int** matrix, int dimension);
void smoothMatrix(int** matrix, int dimension);

int main() {
   
    srand(time(NULL));
    
    int matrixSize = 0;
    while (matrixSize < 2 || matrixSize > 10) {
        printf("Enter matrix size (2-10): ");
        scanf("%d", &matrixSize);
        
        
        if (matrixSize < 2 || matrixSize > 10) {
            printf("Please enter a number between 2 and 10.\n");
        }
    }


   
  
    int **matrix = (int **)malloc(matrixSize * sizeof(int *));
    if (matrix == NULL) {
        printf("Memory allocation failed for rows!\n");
        return 1;
    }

    // Allocate memory for each row
    for (int i = 0; i < matrixSize; i++) {
        matrix[i] = (int *)malloc(matrixSize * sizeof(int));
        if (matrix[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            printf("Memory allocation failed for columns!\n");
            return 1;
        }
    }


    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            *(*(matrix+i)+j) = rand() % 256;
        }
    }

    printf("\nOriginal Randomly Generated Matrix:\n");
    printMatrix(matrix, matrixSize);

    rotateMatrix(matrix, matrixSize);
    printf("\nMatrix after 90 degree Clockwise Rotation:\n");
    printMatrix(matrix, matrixSize);

    smoothMatrix(matrix, matrixSize);
    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    printMatrix(matrix, matrixSize);

   
    
    for (int i = 0; i < matrixSize; i++) {
        free(matrix[i]);
    }
    free(matrix);
    return 0;
}

void printMatrix(int** matrix, int dimension) {
    for (int row = 0; row < dimension; row++) {
        int* rowPtr = *(matrix + row);
        for (int col = 0; col < dimension; col++) {
            printf("%5d", *rowPtr++);
        }
        printf("\n");
    }
}

void rotateMatrix(int** matrix, int dimension) {
    for (int row = 0; row < dimension; row++) {
        int* rowPtr = *(matrix + row);
        for (int col = row; col < dimension; col++) {
            int* colPtr = *(matrix + col) + row;
            int temp = *rowPtr;
            *rowPtr = *colPtr;
            *colPtr = temp;
            rowPtr++;
        }
    }

   
    for (int row = 0; row < dimension; row++) {
        int* leftPtr = *(matrix + row);
        int* rightPtr = leftPtr + dimension - 1;
        
        while (leftPtr < rightPtr) {
            int temp = *leftPtr;
            *leftPtr = *rightPtr;
            *rightPtr = temp;
            leftPtr++;
            rightPtr--;
        }
    }
}

void smoothMatrix(int** matrix, int dimension) {
    if (dimension < 2) return;

    int* prevRow = (int*)malloc(dimension * sizeof(int));
    int* currRow = (int*)malloc(dimension * sizeof(int));
    
    if (!prevRow || !currRow) {
        free(prevRow);
        free(currRow);
        return;
    }

    for (int col = 0; col < dimension; col++) {
        *(prevRow+col) = *(*(matrix+0)+col);
    }

    for (int row = 0; row < dimension; row++) {
        // Save current row's original values
        for (int col = 0; col < dimension; col++) {
            *(currRow+col) = *(*(matrix+row)+col);
        }

        // Process each cell in current row
        for (int col = 0; col < dimension; col++) {
            int sum = 0;
            int count = 0;

            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
               
                    int nRow = row + i;
                    int nCol = col + j;

                   
                    if (nRow < 0 || nRow >= dimension || nCol < 0 || nCol >= dimension) {
                        continue;
                    }

                    count++;
                    if (i == -1) {        
                        sum += *(prevRow+nCol);
                    } else if (i == 0) {
                        sum += *(currRow+nCol);
                    } else {
                        sum += *(*(matrix+nRow)+nCol);
                    }
                }
            }

            *(*(matrix+row)+col) = sum / count;
        }

        // Save current row for next iteration
        for (int col = 0; col < dimension; col++) {
            *(prevRow+col) = *(currRow+col);
        }
    }

    free(prevRow);
    free(currRow);
}