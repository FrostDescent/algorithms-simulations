
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* next function is used by get_MCKP_solution in order to ensure that the solution
   for the MCKP will be a one where the total weight is maximal */
inline unsigned int get_max_index_reverse(const unsigned int* arr, const unsigned int arr_size)
{
  unsigned int cur_max_val = 0;
  unsigned int cur_max_index = 0;
  unsigned int i = 0;

  assert(arr_size>0);
  cur_max_val = arr[arr_size-1];
  cur_max_index = arr_size-1;

  for(i=arr_size; i>0; i--)
    {
      if (cur_max_val < arr[i-1])
        {
          cur_max_val = arr[i-1];
          cur_max_index = i-1;
        }
    }

  return cur_max_index;
}

inline unsigned int get_max_index(const unsigned int* arr, const unsigned int arr_size)
{
  unsigned int cur_max_val = 0;
  unsigned int cur_max_index = 0;
  unsigned int i = 0;

  assert(arr_size>0);
  cur_max_val = arr[0];
  cur_max_index = 0;

  for(i=0; i<arr_size; i++)
    {
      if (cur_max_val < arr[i])
        {
          cur_max_val = arr[i];
          cur_max_index = i;
        }
    }

  return cur_max_index;
}

/* prints a 2-dimensional array */
void print_2d_array(const unsigned int* arr, const unsigned int num_rows,
                    const unsigned int num_cols)
{
  int i=0, j=0;

  for(i=0;i<num_rows;i++)
    {
      for(j=0;j<num_cols;j++)
        {
          printf("arr[%d][%d] = %d\n", i, j, arr[i*num_cols+j]);
        }

    }

}

 /* reverse:  reverse string s in place */
static void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }


 /* uitoa:  convert unsigned int n to characters in s */
 void uitoa(unsigned int n, char s[])
 {
     int i;
 
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     reverse(s);
 }



