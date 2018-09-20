#include <stdio.h>
#include <stdlib.h>

/*
* THis program takes a 10 character string and an integer from standard inuput.
* It then prints the digit in the string at the index provided (0 <= i <= 9),
* It prints out the string if i = -1, else it prints out error.
*/
int main () {
  char phone[11], flag[3];
  // Get user input
  printf("Please enter a ten digit string: ");
  scanf("%s", phone);
  printf("Please enter an integer: ");
  scanf("%s", flag);

  //Convert string to long
  char *ptr;
  long flag_long = strtol(flag, &ptr, 10);

  if( flag_long == -1 ) {
    printf("%s", phone);
    return 0;
  }

  if( flag_long <= 9 && flag_long >= 0 ) {
    printf("%c", phone[flag_long]);
    return 0;
  } else {
    printf("ERROR");
    return 1;
  }
}
