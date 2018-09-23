#include <stdio.h>

/* Each of the n elements of array elements, is the address of an
 * array of n integers.
 * Return 0 if every integer is between 1 and n^2 and all
 * n^2 integers are unique, otherwise return 1.
 */
int check_group(int **elements, int n) {
   int is_all_unique[n*n];
   for (int i = 0; i < n*n; i++) {
     is_all_unique[i] = 0; // initialize the array with 0s
   }

   for (int i = 0; i < n; i++) {
     //for each array
     for (int j = 0; j < n; j++) {
       //for each array element
       int current_number = elements[i][j];
       if (is_all_unique[(current_number - 1)] == 0) {
         is_all_unique[(current_number - 1)] = 1;
       }
     }
   }

   for (int i = 0; i < (n*n); i++) {
     if (is_all_unique[i] == 0) {
       return 1;
     }
   }
   return 0;
}


/*
* This method takes a sudoku puzzle, which is a collection of rows,
* and converts it into a collection of columns
*/
int *build_columns (int **puzzle) {
  int col1[9] = {};
  int col2[9] = {};
  int col3[9] = {};
  int col4[9] = {};
  int col5[9] = {};
  int col6[9] = {};
  int col7[9] = {};
  int col8[9] = {};
  int col9[9] = {};

  int *columns[9] = {col1, col2, col3, col4, col5, col6, col7, col8, col9};

  for (int i = 0; i < 9; i++) {
    for (int j = 0; i < 9; j++) {
      // take the value of puzzle[j][i] and append it to columns[i][j]
      columns[i][j] = puzzle[j][i];
    }
  }
  return *columns;
}

/*
* This method takes a collection of rows and converts it into a collection of boxes
*/
int *build_boxes (int **puzzle) {
  int puzzle_1d[81];
  int boxes_1d[81];
  int counter = 0;

  //convert 2d array into 1d array
  for (int row = 0; row < 9; row++) {
    for (int col = 0; col < 9; col++) {
      puzzle_1d[col + row * 9] = puzzle[row][col];
    }
  }

  //build the boxes into a 1d array
  for (int i = 0; i < 3; i++) {
    for (int j = i; j < i + 7; j += 3 ) {
      for (int k = 3*j; k < j + 3; k++) {
        boxes_1d[counter] = puzzle_1d[k];
        counter++;
      }
    }
  }

  int a1[9], b1[9], c1[9], a2[9], b2[9], c2[9], a3[9], b3[9], c3[9];
  int *boxes[9] = {a1, b1, c1, a2, b2, c2, a3, b3, c3};
  counter = 0;

  //convert the 1d array into a 2d array
  for (int row = 0; row < 9; row++) {
    for (int col = 0; col < 9; col++) {
      boxes[row][col] = boxes_1d[counter];
      counter++;
    }
  }

  return *boxes;
}

/* puzzle is a 9x9 sudoku, represented as a 1D array of 9 pointers
* each of which points to a 1D array of 9 integers.
* Return 0 if puzzle is a valid sudoku and 1 otherwise. You must
* only use check_group to determine this by calling it on
* each row, each column and each of the 9 inner 3x3 squares
*/
int check_regular_sudoku(int **puzzle) {
  int error_flag = 0;
  error_flag = check_group(puzzle, 3);
  if (error_flag == 1) {
    return 1;
  }
  int *columns = build_columns(puzzle);
  error_flag = check_group(&columns, 3);
  if (error_flag == 1) {
    return 1;
  }

  int *boxes = build_boxes(puzzle);
  error_flag = check_group(&boxes, 3);
  if (error_flag == 1) {
    return 1;
  }
  return 0;
}
