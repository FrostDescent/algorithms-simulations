#ifndef MC_GAP_H_
#define MC_GAP_H_

/* functions and struct for handling MC-GAP problem */


/* input is similar to MCKP, except that now the arrays are 3 dimensional since another
   index to indicate which knapsack is involved.
   Added as input is also an array knapsack_sizes to indicate the size of each knapsack.
*/
typedef struct 
{
  unsigned int* weight_arr;
  unsigned int* profit_arr;
  unsigned int num_classes;
  unsigned int num_items_per_class;
  unsigned int* knapsack_sizes;
  unsigned int num_knapsacks;
} MC_GAP_t;


/* this function solves the MC-GAP problem using similar idea to the scheme of GAP
   presented in Reuven and Liran's paper. 
   The algorithm used for MCKP is the pseudopolynomial optimal algorithm found in
   MCKP.c/h
*/


void MC_GAP_solve(MC_GAP_t* self, int** T_ptr);

unsigned int MC_GAP_get_solution_profit(MC_GAP_t* self, int* T);

/* an exponential time function that returns the optimal profit for MC-GAP */
unsigned int MC_GAP_opt_sol(MC_GAP_t* self);

#endif
