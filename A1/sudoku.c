#include <stdio.h>

int check_regular_sudoku(int **puzzle);

int main() {
    // You could change these values to hand-test your function.
    // This example is invalid. Notice that the very middle inner 3x3
    // box has a pair of 3's and a pair of 4's, no 6 and no 7.

    int row1[9] = {1, 2, 3, 4, 5, 6, 7, 8 ,9};
    int row2[9] = {4, 5, 6, 7, 8 ,9, 1, 2, 3};
    int row3[9] = {7, 8 ,9, 1, 2, 3, 4, 5, 6};
    int row4[9] = {9, 1, 2, 3, 4, 5, 6, 7, 8};
    int row5[9] = {5, 6, 7, 8 ,9, 1, 2, 3, 4};
    int row6[9] = {8 ,9, 1, 2, 3, 4, 5, 6, 7};
    int row7[9] = {3, 4, 5, 6, 7, 8 ,9, 1, 2};
    int row8[9] = {6, 7, 8 ,9, 1, 2, 3, 4, 5};
    int row9[9] = {2, 3, 4, 5, 6, 7, 8, 9, 1};

    // This puzzle is correct
    // int row1[9] = {2, 9, 5, 7, 4, 3, 8, 6, 1};
    // int row2[9] = {4, 3, 1, 8, 6, 5, 9, 2, 7};
    // int row3[9] = {8, 7, 6, 1, 9, 2, 5, 4, 3};
    // int row4[9] = {3, 8, 7, 4, 5, 9, 2, 1, 6};
    // int row5[9] = {6, 1, 2, 3, 8, 7, 4, 9, 5};
    // int row6[9] = {5, 4, 9, 2, 1, 6, 7, 3, 8};
    // int row7[9] = {7, 6, 3, 5, 3, 4, 1, 8, 9};
    // int row8[9] = {9, 2, 8, 6, 7, 1, 3, 5, 4};
    // int row9[9] = {1, 5, 4, 9, 3, 8, 6, 7, 2};

    int *puzzle[9] = {row1, row2, row3, row4, row5, row6, row7, row8, row9};
    if (check_regular_sudoku(puzzle)) {
        printf("Invalid Sudoku\n");
    } else {
        printf("Valid Sudoku\n");
    }

    return 0;
}
