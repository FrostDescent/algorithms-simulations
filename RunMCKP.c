#include "MCKP.h"
#include <malloc.h>


#define PROFIT_FILE "mckp_input/profit.txt"
#define WEIGHT_FILE "mckp_input/weight.txt"
#define SIZE_FILE "mckp_input/size.txt"
#define NUM_CLASSES_FILE "mckp_input/num_classes.txt"
#define NUM_ITEMS_PER_CLASS_FILE "mckp_input/num_items_per_class.txt"

unsigned int getNumberOfCommasInFile(char* filename){
	FILE *file = fopen(filename, "r");
	int c;
	int count = 0;

	if(file == NULL){
		printf("Unable to open file %s", filename);
		return 0;
	}

	while ((c = fgetc(file)) != EOF){
		if((char)c == ','){
			++count;
		}
	}
	fclose(file);

	return count;
	
}

unsigned int* file2array(char* filename, unsigned int arraySize){
	unsigned int* array  = NULL;
	FILE *file = fopen(filename, "r");
	int i;

	if(file == NULL){
		printf("Unable to open file %s", filename);
		return NULL;
	}

	array = malloc(sizeof(unsigned int) * arraySize);
	for(i = 0; i < arraySize; ++i){
		fscanf(file, "%d,", &array[i]);
	}

	fclose(file); 

	return array;	
}

unsigned int readNumberFromFile(char* filename){
	FILE *file = fopen(filename, "r");
	unsigned int num;

	if(file == NULL){
		printf("Unable to open file %s", filename);
		return 0;
	}

	fscanf(file, "%d", &num);
	
	fclose(file);	

	return num;
}

int main(){
	unsigned int numOfProfit = getNumberOfCommasInFile(PROFIT_FILE);
	unsigned int numOfWeight = getNumberOfCommasInFile(WEIGHT_FILE);

	unsigned int* profit_arr = file2array(PROFIT_FILE, numOfProfit);
	unsigned int* weight_arr = file2array(WEIGHT_FILE, numOfWeight);
	unsigned int num_classes = readNumberFromFile(NUM_CLASSES_FILE);
	unsigned int num_items_per_class = readNumberFromFile(NUM_ITEMS_PER_CLASS_FILE);
	unsigned int knapsack_size = readNumberFromFile(SIZE_FILE);
	
	if(numOfProfit != numOfWeight){	
		printf("ERROR: Profit and weight array have different sizes!");
		return 1;
	}

	unsigned int sol_size = (knapsack_size+1)*(num_classes+1);
        unsigned int *sol_val = (unsigned int*)malloc(sol_size*sizeof(unsigned int));
        unsigned int *sol_class = (unsigned int*)malloc(sol_size*sizeof(unsigned int));
        unsigned int *sol_item_in_class = (unsigned int*)malloc(sol_size*sizeof(unsigned int));	

	int* sol_MCKP;
	int i;

	sol_MCKP = malloc(num_classes*num_items_per_class*sizeof(int));

	MCKP_solve(weight_arr, profit_arr, 
                num_classes, num_items_per_class,
                knapsack_size,
                sol_val, sol_class,
                sol_item_in_class);

	get_MCKP_solution(weight_arr, profit_arr, 
                num_classes, num_items_per_class,
                knapsack_size,
                sol_val, sol_class,
                sol_item_in_class,
		sol_MCKP);

	for(i = 0 ; i < num_classes - 1 ; ++i){
		printf("%d ", sol_MCKP[i]);
	}
	printf("%d", sol_MCKP[num_classes - 1]);

	return 1;
}
