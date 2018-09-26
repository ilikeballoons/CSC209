#include <stdio.h>
#include <stdlib.h>

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
* puzzle is a pointer to an array of arrays which represents the rows of the puzzle
* cols is 2d array representing the columns of the puzzle
* This function converts the rows into columns and places the values in cols
*/
void build_columns (int **puzzle, int cols[9][9]) {
  for (int row = 0; row < 9; row++) {
    for (int col = 0; col < 9; col++) {
    // take the value of puzzle[j][i] and append it to columns[i][j]
    cols[row][col] = puzzle[col][row];
    }
  }
 }

 /*
 * puzzle is a pointer to an array of arrays which represents the rows of the puzzle
 * boxes is 2d array representing the internal boxes of the puzzle
 * This function converts the rows into boxes and places the values in boxes
 */
void build_boxes (int **puzzle, int boxes[9][9]) {
int index = 0;

for (int starting_row = 0; starting_row < 9; starting_row += 3) {
  for (int starting_col = 0; starting_col < 9; starting_col += 3) {
  		int z = 0;
  		for (int r = starting_row; r < starting_row + 3; r++) {
  			for (int c = starting_col; c < starting_col + 3; c++) {
  				boxes[index][z++] = puzzle[r][c];
  			}
  		}
  		index++;
  	}
  }
}

/*
* Takes a single 9 item array of the puzzle and converts it into a 3x3 array
* as needed for the check_group function described in the assignment sheet
*/
void build_2d_array(int group[9], int** array_2d) {
  int counter = 0;
  for (int i = 0; i < 3; i++) {
  	for (int j = 0; j < 3; j++) {
  		array_2d[i][j] = group[counter++];
		}
	}
}

/* puzzle is a 9x9 sudoku, represented as a 1D array of 9 pointers
* each of which points to a 1D array of 9 integers.
* Return 0 if puzzle is a valid sudoku and 1 otherwise. You must
* only use check_group to determine this by calling it on
* each row, each column and each of the 9 inner 3x3 squares
*/
int check_regular_sudoku(int **puzzle) {
  int error_flag = 0;
  int *array_2d[3];
  for (int i = 0; i < 3; i++) {
    array_2d[i] = malloc(3 * sizeof(int));
  }

  // check 9 rows
  for (int row = 0; row < 9; row++) {
    build_2d_array(puzzle[row], array_2d);
    error_flag = check_group(array_2d, 3);
    if (error_flag == 1) {
      return 1;
     }
   }

  //build columns
  int columns[9][9] = {};
  build_columns(puzzle, columns);
  //check 9 columns
  for (int col = 0;  col < 9; col++) {
    build_2d_array(columns[col], array_2d);
    error_flag = check_group(array_2d, 3);
    if (error_flag == 1) {
      return 1;
    }
  }

  //build boxes
  int boxes[9][9] = {};
  build_boxes(puzzle, boxes);
  //check 9 boxes
  for (int box = 0; box < 9; box++) {
    build_2d_array(boxes[box], array_2d);
    error_flag = check_group(array_2d, 3);
    if (error_flag == 1) {
      return 1;
    }
  }

  return 0; // if the program has reached here, then it is a valid sudoku
}
