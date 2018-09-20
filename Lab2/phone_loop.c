#include <stdio.h>
#include <stdlib.h>

int main () {
  char phone[11], flag[3];
  int error_flag = 0;
  int scanf_ret_value = 1;

  printf("Please enter a ten character string: ");
  scanf("%s", phone);

  do {
    printf("Please enter an integer: ");
    //scanf returns -1 if stdin is ended by user, else 1
    scanf_ret_value = scanf("%s", flag);
    if (scanf_ret_value > 0) {
      //Convert string to long
      char *ptr;
      long flag_long = strtol(flag, &ptr, 10);

      if ( flag_long == -1 ) { printf("%s\n", phone); }
      else if ( flag_long <= 9 && flag_long >= 0 ) { printf("%c\n", phone[flag_long]); }
      else {
         error_flag = 1;
         printf("ERROR\n");
       }
    }
  } while (scanf_ret_value > 0);

  return error_flag;
}
