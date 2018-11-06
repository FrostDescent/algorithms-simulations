#include "MC_GAP.h"
#include <malloc.h>
#include <stdlib.h>

#define PROFIT_FILE "mcgap_input/profit.txt"
#define WEIGHT_FILE "mcgap_input/weight.txt"
#define SIZES_FILE "mcgap_input/sizes.txt"
#define NUM_CLASSES_FILE "mcgap_input/num_classes.txt"
#define NUM_ITEMS_PER_CLASS_FILE "mcgap_input/num_items_per_class.txt"


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
	MC_GAP_t data;
	
	unsigned int numOfProfit = getNumberOfCommasInFile(PROFIT_FILE);
	unsigned int numOfWeight = getNumberOfCommasInFile(WEIGHT_FILE);
	unsigned int numOfSizes = getNumberOfCommasInFile(SIZES_FILE);	

	unsigned int* profit_arr = file2array(PROFIT_FILE, numOfProfit);
	unsigned int* weight_arr = file2array(WEIGHT_FILE, numOfWeight);
	unsigned int* knapsack_sizes = file2array(SIZES_FILE, numOfSizes);

	unsigned int num_classes = readNumberFromFile(NUM_CLASSES_FILE);
	unsigned int num_items_per_class = readNumberFromFile(NUM_ITEMS_PER_CLASS_FILE);
	unsigned int num_knapsacks = numOfSizes;
	
	int* T_ptr;
	int i;

	data.weight_arr = weight_arr;
	data.profit_arr = profit_arr;
	data.num_classes = num_classes;
	data.num_items_per_class = num_items_per_class;
	data.knapsack_sizes = knapsack_sizes;
	data.num_knapsacks = num_knapsacks;

	if(numOfProfit != numOfWeight){	
		printf("Profit and weight array have different sizes!");
		return 1;
	}

	
	MC_GAP_solve(&data, &T_ptr);


	printf("%d|", MC_GAP_get_solution_profit(&data, T_ptr));
	for(i = 0 ; i < num_classes * num_items_per_class - 1 ; ++i){
		printf("%d,", T_ptr[i]);
	}
	printf("%d", T_ptr[num_classes * num_items_per_class - 1]);	

	return 1;
}
