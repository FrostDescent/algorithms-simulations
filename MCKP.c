


/* Contains functions for computing a solution to MCKP */

 
#include "gen_aux.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define MAX_ITEMS_PER_CLASS 20

/* A global auxiliary array */

unsigned int GLOBAL_aux_arr[MAX_ITEMS_PER_CLASS];

/* more globals needed for python module and swig */
unsigned int* GLOBAL_weight_arr = NULL;
unsigned int* GLOBAL_profit_arr = NULL;
unsigned int GLOBAL_num_classes = 0;
unsigned int GLOBAL_num_items_per_class = 0;
unsigned int GLOBAL_knapsack_size = 0;

unsigned int* GLOBAL_sol_val = NULL;
unsigned int* GLOBAL_sol_class = NULL;
unsigned int* GLOBAL_sol_item_in_class = NULL;
int* GLOBAL_sol_MCKP = NULL;

/* callocs all the globals */
void MCKP_init(const unsigned int num_classes, const unsigned int num_items_per_class,
               const unsigned int knapsack_size)
{
  if (NULL != GLOBAL_weight_arr || NULL != GLOBAL_profit_arr ||
      NULL != GLOBAL_sol_val || NULL != GLOBAL_sol_class ||
      NULL != GLOBAL_sol_item_in_class || NULL!= GLOBAL_sol_MCKP)
    {
      printf("ERROR: already initiated?\n");
    }
  else
    {
      GLOBAL_num_classes = num_classes;
      GLOBAL_num_items_per_class = num_items_per_class;
      GLOBAL_knapsack_size = knapsack_size;
      
      GLOBAL_weight_arr = calloc(num_classes*num_items_per_class, sizeof(unsigned int));
      GLOBAL_profit_arr = calloc(num_classes*num_items_per_class, sizeof(unsigned int));
      
      GLOBAL_sol_val = calloc((num_classes+1)*(knapsack_size+1), sizeof(unsigned int));
      GLOBAL_sol_class = calloc((num_classes+1)*(knapsack_size+1), sizeof(unsigned int));
      GLOBAL_sol_item_in_class = calloc((num_classes+1)*(knapsack_size+1), sizeof(unsigned int));
      GLOBAL_sol_MCKP = calloc(num_classes, sizeof(int));

      if (NULL == GLOBAL_weight_arr || NULL == GLOBAL_profit_arr ||
          NULL == GLOBAL_sol_val || NULL == GLOBAL_sol_class ||
          NULL == GLOBAL_sol_item_in_class || NULL == GLOBAL_sol_MCKP)
        {
          printf("ERROR: malloc failed!\n");
        }
    }
}

void MCKP_delete(void)
{
  if (NULL == GLOBAL_weight_arr || NULL == GLOBAL_profit_arr ||
      NULL == GLOBAL_sol_val || NULL == GLOBAL_sol_class ||
      NULL == GLOBAL_sol_item_in_class || NULL == GLOBAL_sol_MCKP)
    {
      printf("ERROR: can't call free on NULL pointers!\n");
    }
  else
    {
      free(GLOBAL_weight_arr);
      free(GLOBAL_profit_arr);
      free(GLOBAL_sol_val);
      free(GLOBAL_sol_class);
      free(GLOBAL_sol_item_in_class);
      free(GLOBAL_sol_MCKP);
      GLOBAL_weight_arr = NULL;
      GLOBAL_profit_arr = NULL;
      GLOBAL_sol_val = NULL;
      GLOBAL_sol_class = NULL;
      GLOBAL_sol_item_in_class = NULL;
      GLOBAL_sol_MCKP = NULL;
    }
}



int MCKP_global_get_sol(const unsigned int class_index)
{
  return GLOBAL_sol_MCKP[class_index];
}


void MCKP_set_global_profit(const unsigned int class_index, const unsigned int item_index, 
                            const unsigned int profit)
{
  GLOBAL_profit_arr[class_index*GLOBAL_num_items_per_class + item_index] = profit;
}

unsigned int MCKP_get_global_profit(const unsigned int class_index, const unsigned int item_index)
{
  return GLOBAL_profit_arr[class_index*GLOBAL_num_items_per_class + item_index];
}

void MCKP_set_global_weight(const unsigned int class_index, const unsigned int item_index, 
                            const unsigned int weight)
{
  GLOBAL_weight_arr[class_index*GLOBAL_num_items_per_class + item_index] = weight;
}

unsigned int MCKP_get_global_weight(const unsigned int class_index, const unsigned int item_index)
{
  return GLOBAL_weight_arr[class_index*GLOBAL_num_items_per_class + item_index];
}





void MCKP_solve(const unsigned int* weight_arr, const unsigned int* profit_arr, 
                const unsigned int num_classes, const unsigned int num_items_per_class,
                const unsigned int knapsack_size,
                unsigned int* sol_val, unsigned int* sol_class,
                unsigned int* sol_item_in_class)
{
  unsigned int cur_class = 0;
  unsigned int cur_item_in_class = 0;
  
  unsigned int cur_total_weight = 0;
  

  if (NULL == memset(sol_val, 0, (num_classes+1)*(knapsack_size+1)*sizeof(unsigned int)))
    {
      printf("call to memset failed!\n");
      exit(0);
    }

  for(cur_class=0; cur_class<num_classes; cur_class++)
    {
      /* sol classes index adjustments (adding one) */
      unsigned int sol_class_index = cur_class + 1;
      unsigned int max_index = 0;

      for (cur_total_weight = 0; cur_total_weight < knapsack_size+1; cur_total_weight++)
        {
          
          /* perparing auxiliary array */
          memset(GLOBAL_aux_arr, 0, num_items_per_class*sizeof(unsigned int)); 
          for(cur_item_in_class = 0; cur_item_in_class<num_items_per_class; cur_item_in_class++)
            {
              unsigned int cur_item_index = cur_class*num_items_per_class + cur_item_in_class;
              unsigned int cur_item_weight = weight_arr[cur_item_index];
              unsigned int cur_item_profit = profit_arr[cur_item_index];
              if (cur_total_weight >= cur_item_weight)
                {
                  GLOBAL_aux_arr[cur_item_in_class] = sol_val[(sol_class_index-1)*(knapsack_size+1) 
                                                       + cur_total_weight - cur_item_weight] + 
                    cur_item_profit;
                }
            }
          max_index = get_max_index(GLOBAL_aux_arr, num_items_per_class);
          
          /* now calculaing using the dynamic programming equation for MCKP */
          sol_val[sol_class_index*(knapsack_size+1) + cur_total_weight] = GLOBAL_aux_arr[max_index];
          sol_class[sol_class_index*(knapsack_size+1) + cur_total_weight] = cur_class;
          sol_item_in_class[sol_class_index*(knapsack_size+1) + cur_total_weight] = max_index;
        }
    }

}



void print_MCKP_solution(const unsigned int* weight_arr, const unsigned int* profit_arr,
                         const unsigned int num_classes, const unsigned int num_items_per_class,
                         const unsigned int knapsack_size,
                         unsigned int* sol_val, unsigned int* sol_class,
                         unsigned int* sol_item_in_class)
{
  /* find max profit when all classes are considered */
  unsigned int sol_index = get_max_index(&(sol_val[(num_classes)*(knapsack_size+1)]), 
                                         knapsack_size+1);

  unsigned int remaining_weight = sol_index;

  unsigned int cur_class = num_classes;
  
  printf("printing solution to MCKP:\n\n");


  while (remaining_weight>0 && cur_class>0) 
    {    
      unsigned int index_for_sol_arrs = (cur_class)*(knapsack_size+1) + remaining_weight;
      printf("remaining_weight: \t%d\n", remaining_weight);
      printf("sol_val: \t%d\n", sol_val[index_for_sol_arrs]);
      printf("sol_class: \t%d\n", sol_class[index_for_sol_arrs]);
      printf("sol_item_in_class: \t%d\n\n", sol_item_in_class[index_for_sol_arrs]);
      printf("item weight: \t%d\n", weight_arr[sol_class[index_for_sol_arrs]*num_items_per_class + 
                                               sol_item_in_class[index_for_sol_arrs]]);
      printf("item profit: \t%d\n",profit_arr[sol_class[index_for_sol_arrs]*num_items_per_class + 
                                              sol_item_in_class[index_for_sol_arrs]]);
      
      printf("-----------------------------------------\n");
      
      remaining_weight -= (weight_arr[sol_class[index_for_sol_arrs]*num_items_per_class + 
                                      sol_item_in_class[index_for_sol_arrs]]);
      cur_class--;
    }
}


void get_MCKP_solution(const unsigned int* weight_arr, const unsigned int* profit_arr,
                       const unsigned int num_classes, const unsigned int num_items_per_class,
                       const unsigned int knapsack_size,
                       unsigned int* sol_val, unsigned int* sol_class,
                       unsigned int* sol_item_in_class,
                       int* out_sol)
{
  /* find max profit when all classes are considered */
  unsigned int sol_index = get_max_index_reverse(&(sol_val[(num_classes)*(knapsack_size+1)]), 
                                                 knapsack_size+1);
  unsigned int remaining_weight = sol_index;
  unsigned int cur_class = num_classes;
  int i = 0;

  /* initing out_sol */
  for(i=0; i<num_classes; i++)
    {
      out_sol[i] = -1;
    }

  while (remaining_weight>0 && cur_class>0) 
    {    
      unsigned int index_for_sol_arrs = (cur_class)*(knapsack_size+1) + remaining_weight;

      out_sol[sol_class[index_for_sol_arrs]] =  sol_item_in_class[index_for_sol_arrs];

      remaining_weight -= (weight_arr[sol_class[index_for_sol_arrs]*num_items_per_class + 
                                      sol_item_in_class[index_for_sol_arrs]]);
      cur_class--;
    }
}

void MCKP_global_solve(void)
{
  MCKP_solve(GLOBAL_weight_arr, GLOBAL_profit_arr, 
             GLOBAL_num_classes, GLOBAL_num_items_per_class,
             GLOBAL_knapsack_size,
             GLOBAL_sol_val, GLOBAL_sol_class,
             GLOBAL_sol_item_in_class);

  get_MCKP_solution(GLOBAL_weight_arr, GLOBAL_profit_arr,
                    GLOBAL_num_classes, GLOBAL_num_items_per_class,
                    GLOBAL_knapsack_size,
                    GLOBAL_sol_val, GLOBAL_sol_class,
                    GLOBAL_sol_item_in_class,
                    GLOBAL_sol_MCKP);
}
