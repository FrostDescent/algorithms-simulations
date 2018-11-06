
#include "MCKP.h"

#include "MC_GAP.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int* GLOBAL_T = NULL;

void MC_GAP_global_solve(MC_GAP_t* self)
{
  if (NULL != GLOBAL_T)
    {
      printf("warning: already solved? freeing solution\n");
      free(GLOBAL_T);
    }
  MC_GAP_solve(self, &GLOBAL_T);
}

int MC_GAP_sol_get_item_in_class(MC_GAP_t* self, const unsigned int class)
{
  int flag = 0;
  unsigned int cur_item = 0;
  int ret_val = -1;
  
  for (cur_item = 0; cur_item<self->num_items_per_class; cur_item++)
    {
      if (-1 != GLOBAL_T[class*self->num_items_per_class+cur_item])
        {
          if (1==flag)
            {
              printf("ERROR: more than one item assigned in same class!\n");
            }
          else
            {
              flag=1;
              ret_val = cur_item;
            }
        }
    }
  return ret_val;
}

int MC_GAP_sol_get_knapsack_in_class(MC_GAP_t* self, const unsigned int class, const unsigned int item)
{
  return GLOBAL_T[class*self->num_items_per_class+item];
}



MC_GAP_t* MC_GAP_init(const unsigned int num_knapsacks, const unsigned int num_classes,
                    const unsigned int num_items_per_class)
{
  MC_GAP_t* mc_gap = calloc(1, sizeof(MC_GAP_t));


  if (NULL == mc_gap)
    {
      printf("ERROR: calloc failed!\n");
    }
  else
    {
      mc_gap->num_classes = num_classes;
      mc_gap->num_items_per_class = num_items_per_class;
      mc_gap->num_knapsacks = num_knapsacks;
      
      mc_gap->weight_arr = calloc(num_knapsacks*num_classes*num_items_per_class, 
                                  sizeof(unsigned int));
      
      mc_gap->profit_arr = calloc(num_knapsacks*num_classes*num_items_per_class, 
                                  sizeof(unsigned int));
      
      mc_gap->knapsack_sizes = calloc(num_knapsacks, sizeof(unsigned int));

      if (NULL == mc_gap->weight_arr || NULL == mc_gap->profit_arr ||
          NULL == mc_gap->knapsack_sizes)
        {
          printf("ERROR: calloc failed!\n");
        }
    }

  return mc_gap;
}


void MC_GAP_delete(MC_GAP_t* mc_gap)
{
  if (NULL == mc_gap)
    {
      printf("ERROR: NULL pointer given!\n");
    }
  else 
    {
      if (NULL == mc_gap->weight_arr || NULL == mc_gap->profit_arr ||
          NULL == mc_gap->knapsack_sizes)
        {
          printf("ERROR: some fields have NULL pointers!\n");
        }
      else
        {
          free(mc_gap->weight_arr);
          free(mc_gap->profit_arr);
          free(mc_gap->knapsack_sizes);
          free(mc_gap);
          if (NULL != GLOBAL_T)
            {
              free(GLOBAL_T);
              GLOBAL_T = NULL;
            }
        }
    }
}

static unsigned int get_index(MC_GAP_t* self, unsigned int knapsack, 
                              unsigned int class, unsigned int item)
{
  if (knapsack >= self->num_knapsacks ||
      class >= self->num_classes ||
      item >= self->num_items_per_class)
    {
      printf("ERROR: one of the given indexes is invalid!\n");
    }
  
  return (self->num_classes*self->num_items_per_class*knapsack 
          + self->num_items_per_class*class
          + item);
}

void set_weight(MC_GAP_t* self, unsigned int knapsack, 
                unsigned int class, unsigned int item, 
                unsigned int weight)
{
  self->weight_arr[get_index(self, knapsack, class, item)] = weight;
}

void set_profit(MC_GAP_t* self, unsigned int knapsack, 
                unsigned int class, unsigned int item, 
                unsigned int profit)
{
  self->profit_arr[get_index(self, knapsack, class, item)] = profit;
}

void set_knapsack_size(MC_GAP_t* self, unsigned int knapsack, 
                       unsigned int knapsack_size)
{
  if (knapsack < self->num_knapsacks)
    {
      self->knapsack_sizes[knapsack] = knapsack_size;
    }
  else
    {
      printf("ERROR: knapsack index invalid!\n");
    }
}



unsigned int get_weight(MC_GAP_t* self, unsigned int knapsack, 
                               unsigned int class, unsigned int item)
{
  return self->weight_arr[get_index(self, knapsack, class, item)];
}

unsigned int get_profit(MC_GAP_t* self, unsigned int knapsack, 
                               unsigned int class, unsigned int item)
{
  return self->profit_arr[get_index(self, knapsack, class, item)];
}

unsigned int get_knapsack_size(MC_GAP_t* self, unsigned int knapsack)
{
  if (knapsack < self->num_knapsacks)
    {
      return self->knapsack_sizes[knapsack];
    }
  else
    {
      printf("ERROR: knapsack index invalid!\n");
      return 0;
    }
}



 
/* this function calculates the Pj which is required for MC_GAP
   note: it is not the same as in Liran's paper, but modified */
void calc_profit_func(MC_GAP_t* self, unsigned int* Pj, int* T,
                      unsigned int knapsack, unsigned int class, unsigned int item)
{
  int is_assigned_in_class = 0; /*used as bool to indicate if previous assignment to 
                                  this class exists*/
  int assigned_knapsack = -1;
  int assigned_item_in_class = -1;
  int i=0;

  for(i=0; i<self->num_items_per_class; i++)
    {
      if (-1 != T[(class*self->num_items_per_class) + i])
        {
          is_assigned_in_class = 1;
          assigned_knapsack = T[(class*self->num_items_per_class) + i];
          assigned_item_in_class = i;
          break; /*item is assigned at most once, so we can break loop here */
        }
    }

  if (0 == is_assigned_in_class)
    {
      Pj[class*(self->num_items_per_class)+item] = 
        get_profit(self, knapsack, class, item);
      
    }
  else
    {
      unsigned int assigned_profit = get_profit(self, assigned_knapsack,
                                                class, assigned_item_in_class);
      
      if (get_profit(self, knapsack, class, item)<assigned_profit)
        {
          Pj[class*(self->num_items_per_class)+item] = 0;
        }
      else
        {
          Pj[class*(self->num_items_per_class)+item] = 
           (get_profit(self, knapsack, class, item) - assigned_profit);
        }
    }
}
                      
                      
/* solves the MC-GAP problem */
/* the T_ptr will point to an array T which contains the solution as
   specified in the GAP algorithm in Liran's paper*/

void MC_GAP_solve(MC_GAP_t* self, int** T_ptr)
{
  int total_num_of_items = self->num_classes*self->num_items_per_class;

  /* T array as defined in the GAP scheme algorithm */
  int* T = malloc(total_num_of_items*sizeof(int));

  /* Profit vector as defined in the GAP scheme alg */
  unsigned int* Pj = malloc(total_num_of_items*sizeof(unsigned int));
  unsigned int j=0, i = 0;

  unsigned int cur_knapsack = 0, cur_class = 0, cur_item = 0;
  
  unsigned int* sol_val;
  unsigned int* sol_class;
  unsigned int* sol_item_in_class;
  unsigned int sol_size;
  unsigned int index_for_MCKP = 0;
  int* sol_MCKP; /*this array will indicate per class the item selected
                            in the MCKP solution */
  
  /* allocate room for MCKP solution, which we'll use later on*/
  sol_MCKP = malloc(self->num_classes*self->num_items_per_class*sizeof(int));

  
  if (NULL == T || NULL == sol_MCKP)
    {
      printf("Error in malloc!\n");
      exit(1);
    }

  /* initing the T array */
  for(i=0 ; i<total_num_of_items ; i++)
    {
      T[i]=-1;
    }
  
  for(cur_knapsack=0; cur_knapsack<(self->num_knapsacks); cur_knapsack++)
    {
      for(cur_class=0; cur_class<(self->num_classes); cur_class++)
        {
          for(cur_item = 0; cur_item<(self->num_items_per_class); cur_item++)
            {
              /* printf("cur_knapsack: %d, cur_class: %d, cur_item: %d\n", 
                 cur_knapsack, cur_class, cur_item); */
              calc_profit_func(self, Pj, T,
                               cur_knapsack, cur_class, cur_item);
              
                
            }
        }
      
      /* Now we need to get readay to
         run the MCKP algorithm for the current knapsack */
      
      
      /* first some allocating and initing */
      sol_size = (self->knapsack_sizes[cur_knapsack]+1)*(self->num_classes+1);
      sol_val = malloc(sol_size*sizeof(unsigned int));
      sol_class = malloc(sol_size*sizeof(unsigned int));
      sol_item_in_class = malloc(sol_size*sizeof(unsigned int));
      
      if (NULL == sol_val ||
          NULL == sol_class ||
          NULL == sol_item_in_class ||
          NULL == sol_MCKP)
        {
      printf("Error in malloc!\n");
      exit(1);
        }
      

      memset(sol_val, 0, sol_size*sizeof(unsigned int));
      memset(sol_class, 0, sol_size*sizeof(unsigned int));
      memset(sol_item_in_class, 0, sol_size*sizeof(unsigned int));
      
      index_for_MCKP = cur_knapsack*self->num_classes*self->num_items_per_class;
      
      MCKP_solve(&(self->weight_arr[index_for_MCKP]),
                 Pj,
                 self->num_classes,
                 self->num_items_per_class,
                 self->knapsack_sizes[cur_knapsack],
                 sol_val, sol_class, sol_item_in_class);
      /* TODO: continue coding here according to alg */
      
      /* print_MCKP_solution(&(self->weight_arr[index_for_MCKP]),
                          Pj,
                          self->num_classes,
                          self->num_items_per_class,
                          self->knapsack_sizes[cur_knapsack],
                          sol_val, sol_class, sol_item_in_class); */
      
      /*      printf("calling get_MCKP_solution()\n"); */
      
      get_MCKP_solution(&(self->weight_arr[index_for_MCKP]),
                        Pj,
                        self->num_classes,
                        self->num_items_per_class,
                        self->knapsack_sizes[cur_knapsack],
                        sol_val, sol_class, sol_item_in_class,
                        sol_MCKP);
      /* printf("returned from get_MCKP_solution()\n"); */

      for (i=0; i<self->num_classes; i++)
        {
          if (sol_MCKP[i]>0) /*means we selected an item of this class 
                               (item 0 in class means not selected)*/
            {
              /* we need to make sure we didn't already select an item
                 from this class to a different knapsck.
                 If we did, we should remove it */
              for(j=0; j<self->num_items_per_class; j++)
                {
                  T[i*(self->num_items_per_class)+j] = -1;
                }

              T[i*(self->num_items_per_class)+sol_MCKP[i]] = cur_knapsack;

            }
        }
  
      free(sol_val);
      free(sol_class);
      free(sol_item_in_class);
}
  
  /*temporarily we print here the solution */
  /*  
    for(i = 0; i<total_num_of_items; i++)
    {
    if (-1 != T[i])
    {
    printf("item %d in class %d is in knapsack %d (weight: %d, profit: %d)\n",
    i % self->num_items_per_class,
    i / self->num_items_per_class,
    T[i],
                 get_weight(self, 
                 T[i],
                 i / self->num_items_per_class,
                 i % self->num_items_per_class),
                 get_profit(self, 
                 T[i],
                 i / self->num_items_per_class,
                 i % self->num_items_per_class));
                 }
      
                 }
  */
  free(sol_MCKP);
  free(Pj);

  /*  free(T);*/
  *T_ptr = T; /*this is how we return the solution */

}


unsigned int MC_GAP_get_solution_profit(MC_GAP_t* self, int* T)
{
  int total_num_of_items = self->num_classes*self->num_items_per_class;
  int i=0;
  unsigned int total_profit = 0;

  for(i = 0; i<total_num_of_items; i++)
    {
      if (-1 != T[i])
        {
          total_profit += self->profit_arr[T[i]*total_num_of_items + i];
        }

    }
  return total_profit;
  
}


/* A recursive function to be use by MC_GAP_opt_sol() */
/* cur_mcgap is used to make an instance of mcgap where the items are in fact */
/* assigned each in at most one knapsack (by setting profit&size to 0 for the rest */
/* Then, the optimal MCKP is calculated for each bin and the maximum of all options is returned */
unsigned int MC_GAP_opt_rec(MC_GAP_t* self, 
                            MC_GAP_t* cur_mcgap,
                            int cur_class)
{
  unsigned int old_profit = 0;
  unsigned int cur_profit = 0;
  unsigned int cur_knapsack = 0;
  unsigned int j = 0;

  /*  printf("MC_GAP_open_rec() called with cur_class=%d\n",cur_class);*/

  if (cur_class < self->num_classes)
    {
      /* used as boolean to see if profit in all knapsacks is zero*/
      int all_profit_all_knapsacks_zero = 1;

      for(cur_knapsack=0 ; cur_knapsack<self->num_knapsacks; cur_knapsack++)
        {
          /* used as boolean to see if all profits of this class is zero for cur_knapsack */
          int all_profit_zero = 1;

          /* insert cur_class into cur_knapsack */
          for(j=0 ; j<self->num_items_per_class; j++)
            {
              int cur_index = (cur_knapsack*self->num_classes*
                               self->num_items_per_class + 
                               cur_class*self->num_items_per_class + j);

              cur_mcgap->weight_arr[cur_index] = self->weight_arr[cur_index];
              cur_mcgap->profit_arr[cur_index] = self->profit_arr[cur_index];

              if (self->profit_arr[cur_index]>0)
                {
                  all_profit_zero = 0;
                  all_profit_all_knapsacks_zero = 0;
                }
              
            }

          /* we invoke the recursive algorithm only if there is some profit>0 for this class */
          if (0 == all_profit_zero)
            {
              cur_profit = MC_GAP_opt_rec(self, cur_mcgap, cur_class+1);
            }
          
          /* remove cur_class from cur_knapsack*/
          for(j=0 ; j<self->num_items_per_class; j++)
            {
              int cur_index = (cur_knapsack*self->num_classes*
                               self->num_items_per_class + 
                               cur_class*self->num_items_per_class + j);

              cur_mcgap->weight_arr[cur_index] = 0;
              cur_mcgap->profit_arr[cur_index] = 0;
            }


          if (cur_profit > old_profit)
            {

              old_profit = cur_profit;
            }
        
        }

      if (1 == all_profit_all_knapsacks_zero) 
        {
          /* if we are here we must desend once in the recursion to check solutions
             of assigning this item to some knapsack (doesn't matter which one because
             its profit is 0 anyway */

          cur_profit = MC_GAP_opt_rec(self, cur_mcgap, cur_class+1);
          
          if (cur_profit > old_profit)
            {
              old_profit = cur_profit;
            }

        }


      /* return the max profit we encountered */
      return old_profit;
    }

  else /* if we are here then each class is in a knapsack */
    {
      unsigned int total_profit = 0;
      unsigned int* sol_val;
      unsigned int* sol_class;
      unsigned int* sol_item_in_class;
      unsigned int sol_size;
      unsigned int index_for_MCKP = 0;
      int* sol_MCKP; /*this array will indicate per class the item selected
                       in the MCKP solution */
      int* T;
      int i=0;

       
      /* allocate room for MCKP solution, which we'll use later on*/
      sol_MCKP = malloc(cur_mcgap->num_classes*cur_mcgap->num_items_per_class*sizeof(int));
      
      T = malloc(cur_mcgap->num_classes*cur_mcgap->num_items_per_class*sizeof(int));
      
      if (NULL == sol_MCKP ||
          NULL == T)
        {
              printf("Error in malloc!\n");
              exit(1);
        }
      
      

      for(cur_knapsack=0; cur_knapsack<self->num_knapsacks; cur_knapsack++)
        {
       
          /* first some allocating and initing */
          sol_size = (cur_mcgap->knapsack_sizes[cur_knapsack]+1)*(cur_mcgap->num_classes+1);
          sol_val = malloc(sol_size*sizeof(unsigned int));
          sol_class = malloc(sol_size*sizeof(unsigned int));
          sol_item_in_class = malloc(sol_size*sizeof(unsigned int));
          
          if (NULL == sol_val ||
              NULL == sol_class ||
              NULL == sol_item_in_class)
            {
              printf("Error in malloc!\n");
              exit(1);
            }
 

          memset(sol_val, 0, sol_size*sizeof(unsigned int));
          memset(sol_class, 0, sol_size*sizeof(unsigned int));
          memset(sol_item_in_class, 0, sol_size*sizeof(unsigned int));
          
          index_for_MCKP = cur_knapsack*cur_mcgap->num_classes*cur_mcgap->num_items_per_class;
      

          MCKP_solve(&(cur_mcgap->weight_arr[index_for_MCKP]),
                     &(cur_mcgap->profit_arr[index_for_MCKP]),
                     cur_mcgap->num_classes,
                     cur_mcgap->num_items_per_class,
                     cur_mcgap->knapsack_sizes[cur_knapsack],
                     sol_val, sol_class, sol_item_in_class);
 

          get_MCKP_solution(&(cur_mcgap->weight_arr[index_for_MCKP]),
                            &(cur_mcgap->profit_arr[index_for_MCKP]),
                            cur_mcgap->num_classes,
                            cur_mcgap->num_items_per_class,
                            cur_mcgap->knapsack_sizes[cur_knapsack],
                            sol_val, sol_class, sol_item_in_class,
                            sol_MCKP);
       

          for(j=0; j<(self->num_items_per_class*self->num_classes); j++)
            {
              T[j] = -1;
            }
          
          
          for (i=0; i<self->num_classes; i++)
            {
              if (sol_MCKP[i]>0) /*means we selected an item of this class 
                                   (item 0 in class means not selected)*/
                {
                  T[i*(self->num_items_per_class)+sol_MCKP[i]] = cur_knapsack;
                  
                }
            }
          
          total_profit += MC_GAP_get_solution_profit(cur_mcgap, T);
          
          free(sol_val);
          free(sol_class);
          free(sol_item_in_class);
  
        }
      free(sol_MCKP);
      free(T);
      return total_profit;
      
    }

}


/* an exponential time function that returns the optimal profit for MC-GAP */
unsigned int MC_GAP_opt_sol(MC_GAP_t* self)
{
  MC_GAP_t cur_mcgap;

  unsigned int arrays_size = (self->num_classes*self->num_items_per_class*
                              self->num_knapsacks);
  
  unsigned int opt_profit = 0;

  cur_mcgap.weight_arr = malloc(sizeof(unsigned int)*arrays_size);
  cur_mcgap.profit_arr = malloc(sizeof(unsigned int)*arrays_size);
  cur_mcgap.knapsack_sizes = malloc(sizeof(unsigned int)*self->num_knapsacks);
  cur_mcgap.num_classes = self->num_classes;
  cur_mcgap.num_items_per_class = self->num_items_per_class;
  cur_mcgap.num_knapsacks = self->num_knapsacks;


  if (NULL == cur_mcgap.weight_arr ||
      NULL == cur_mcgap.profit_arr ||
      NULL == cur_mcgap.knapsack_sizes)
    {
      printf("ERROR in malloc()!\n");
      exit(1);
    }


  memset(cur_mcgap.weight_arr, 0, arrays_size*sizeof(unsigned int));
  memset(cur_mcgap.profit_arr, 0, arrays_size*sizeof(unsigned int));
  memcpy(cur_mcgap.knapsack_sizes, self->knapsack_sizes, self->num_knapsacks*sizeof(unsigned int));

  opt_profit =  MC_GAP_opt_rec(self, &cur_mcgap, 0);

  free(cur_mcgap.weight_arr);
  free(cur_mcgap.profit_arr);
  free(cur_mcgap.knapsack_sizes);

  return opt_profit;

 

}

