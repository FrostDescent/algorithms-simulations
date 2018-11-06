/* functions for handling MCKP problem /*


/* This function is the core function that solves MCKP using dynamic programming
as specified int knapsack problems book by kellerer (page 329).
weight_arr - an array of weights.
profit_arr - an array of profits.
num_classes - number of classes in the MCKP
num_items_per_class - we assume all classes has the same number of items,
this might increases the running time a little.
knapsack_size - size of the given knapsack
sol_val - this array is used in the dynamic programming for holding the 
best profit under a certain classes and capacity.
sol_class - used to indicate the selected class along the dynamic programming solution
sol_item_in_class - used to indicate the selected item in the class

IMPORTANT: 
1. all arrays are two dimensional where the first index is the class and the second
index is the item in the class. all arrays are of total size num_classes*num_items_per_class
besides sol_val, sol_class and sol_item_in_class, which are (num_classes+1)*(knapsack_size+1)
because the dynamic programming goes as far as to 0 classes allowed.

2. It is assumed that the first element in each class is of weight 0 and profit 0. (this
in fact indicate this item was not selected)
*/

void MCKP_init(const unsigned int num_classes, const unsigned int num_items_per_class,
               const unsigned int knapsack_size);

void MCKP_set_global_profit(const unsigned int class_index, const unsigned int item_index, 
                            const unsigned int profit);


unsigned int MCKP_get_global_profit(const unsigned int class_index, const unsigned int item_index);


void MCKP_set_global_weight(const unsigned int class_index, const unsigned int item_index, const unsigned int weight);

unsigned int MCKP_get_global_weight(const unsigned int class_index, const unsigned int item_index);

void MCKP_delete(void);


void MCKP_solve(const unsigned int* weight_arr, const unsigned int* profit_arr, 
                const unsigned int num_classes, const unsigned int num_items_per_class,
                const unsigned int knapsack_size,
                unsigned int* sol_val, unsigned int* sol_class,
                unsigned int* sol_item_in_class);


/* returns the solution in out_sol array */
void get_MCKP_solution(const unsigned int* weight_arr, const unsigned int* profit_arr,
                       const unsigned int num_classes, const unsigned int num_items_per_class,
                       const unsigned int knapsack_size,
                       unsigned int* sol_val, unsigned int* sol_class,
                       unsigned int* sol_item_in_class,
                       int* out_sol);



/* gets the arrays from the dynamic programming solution and prints it */

void print_MCKP_solution(const unsigned int* weight_arr, const unsigned int* profit_arr,
                         const unsigned int num_classes, const unsigned int num_items_per_class,
                         const unsigned int knapsack_size,
                         unsigned int* sol_val, unsigned int* sol_class,
                         unsigned int* sol_item_in_class);
