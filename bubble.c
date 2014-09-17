#include <stdio.h>
#include <stdlib.h>
#define RANDOM_INTERVAL 100

void bubble_sort(int *list, int n);

void main(int argc, char **argv){
	int i;
	int vector_size = atoi(argv[1]);
	int *vector = (int *) malloc(sizeof(int)*vector_size);
	
	srand(time(0));
	
	for(i=0; i<vector_size; i++){	
		vector[i] = rand() % RANDOM_INTERVAL;
	}

	bubble_sort(vector,vector_size);

}

void bubble_sort(int *list, int n)
{
    long c,d,t;
    for(c=0; c<(n-1); c++){
        for(d=0; d<n-c-1; d++){
            if (list[d] > list[d+1]){
                t = list[d];
                list[d] = list[d+1];
                list[d+1] = t;
            }
        }
    }
}
