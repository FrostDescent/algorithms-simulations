#ifndef GEN_AUX_H_
#define GEN_AUX_H_


/* This file contains auxiliary functions */


/* Returns the index of the maximum element in an array of integers
   arr - an array of integers
   arr_size - the size of the array, must be > 0
*/
inline unsigned int get_max_index(const unsigned int* arr, const unsigned int arr_size);
  

/* next function is used by get_MCKP_solution in order to ensure that the solution
   for the MCKP will be a one where the total weight is maximal */
inline unsigned int get_max_index_reverse(const unsigned int* arr, const unsigned int arr_size);


/* prints a 2-dimensional array */
void print_2d_array(const unsigned int* arr, const unsigned int num_rows,
                    const unsigned int num_cols);

/* itoa:  convert n to characters in s */
void itoa(int n, char s[]);

#endif

