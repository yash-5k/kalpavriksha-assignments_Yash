#include <stdio.h>
#include <stdlib.h>

void printMatrix(const int* matrixPtr, int dimension);
void rotateMatrix(int* matrixPtr, int dimension);
void smoothMatrix(int* matrixPtr, int dimension);

int main() {
    int matrixSize;
    printf("Enter matrix size (2-10): ");
    scanf("%d", &matrixSize);

    if (matrixSize < 2 || matrixSize > 10) {
        fprintf(stderr, "Error: Matrix size must be between 2 and 10.\n");
        return 1;
    }


    int matrix[matrixSize][matrixSize];

    for (int row = 0; row < matrixSize; row++) {
        for (int col = 0; col < matrixSize; col++) {
            matrix[row][col] = rand() % 256;
        }
    }

    int *matrixPtr = (int *)matrix;

    printf("\nOriginal Randomly Generated Matrix:\n");
    printMatrix(matrixPtr, matrixSize);

    rotateMatrix(matrixPtr, matrixSize);
    printf("\nMatrix after 90° Clockwise Rotation:\n");
    printMatrix(matrixPtr, matrixSize);

    smoothMatrix(matrixPtr, matrixSize);
    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    printMatrix(matrixPtr, matrixSize);

    return 0;
}

void printMatrix(const int* matrixPtr, int dimension) {
    for (int row = 0; row < dimension; row++) {
        for (int col = 0; col < dimension; col++) {
            printf("%5d", *(matrixPtr + row * dimension + col));
        }
        printf("\n");
    }
}

void rotateMatrix(int* matrixPtr, int dimension) {
    for (int row = 0; row < dimension; row++) {
        for (int col = row; col < dimension; col++) {
            int* pointerToCell_RowCol = matrixPtr + row * dimension + col;
            int* pointerToCell_ColRow = matrixPtr + col * dimension + row;
            
            int tempValue = *pointerToCell_RowCol;
            *pointerToCell_RowCol = *pointerToCell_ColRow;
            *pointerToCell_ColRow = tempValue;
        }
    }

    for (int rowIndex = 0; rowIndex < dimension; rowIndex++) {
        int* leftPointer = matrixPtr + rowIndex * dimension;
        int* rightPointer = matrixPtr + rowIndex * dimension + (dimension - 1);
        
        while (leftPointer < rightPointer) {
            int tempValue = *leftPointer;
            *leftPointer = *rightPointer;
            *rightPointer = tempValue;
            
            leftPointer++;
            rightPointer--;
        }
    }
}

void smoothMatrix(int* matrixPtr, int dimension) {
    if (dimension < 2) return;

    int* previousRowBuffer = (int*)malloc(dimension * sizeof(int));
    int* currentRowBuffer = (int*)malloc(dimension * sizeof(int));


    for (int row = 0; row < dimension; ++row) {
        for (int index = 0; index < dimension; ++index) {
            *(currentRowBuffer + index) = *(matrixPtr + row * dimension + index);
        }

        for (int col = 0; col < dimension; ++col) {
            long long sumOfNeighbors = 0;
            int neighborCount = 0;

            for (int deltaRow = -1; deltaRow <= 1; ++deltaRow) {
                for (int deltaCol = -1; deltaCol <= 1; ++deltaCol) {
                    int neighborRow = row + deltaRow;
                    int neighborCol = col + deltaCol;

                    if (neighborRow >= 0 && neighborRow < dimension && neighborCol >= 0 && neighborCol < dimension) {
                        neighborCount++;
                        
                        if (deltaRow == -1) {
                            sumOfNeighbors += *(previousRowBuffer + neighborCol);
                        } else if (deltaRow == 0) {
                            sumOfNeighbors += *(currentRowBuffer + neighborCol);
                        } else {
                            sumOfNeighbors += *(matrixPtr + neighborRow * dimension + neighborCol);
                        }
                    }
                }
            }
            *(matrixPtr + row * dimension + col) = (int)(sumOfNeighbors / neighborCount);
        }

        for (int index = 0; index < dimension; ++index) {
            *(previousRowBuffer + index) = *(currentRowBuffer + index);
        }
    }

    free(previousRowBuffer);
    free(currentRowBuffer);
}