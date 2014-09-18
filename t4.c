#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define RANDOM_INTERVAL 100 // Intervalo de preenchimento do vetor
#define PRINT 1 // booleano para impressao do vetor final

void bubble_sort(int *list, int n);

int main(int argc, char **argv)
{
    double t1,t2;
    int i,j;
    int vector_size = atoi(argv[1]);
    int rank, nprocs;
    int *vector = (int *) malloc(sizeof(int)*vector_size);
	int *slice = (int*) malloc(sizeof(int)*vector_size/nprocs);

    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	srand(time(0));
		for(i=0; i<vector_size; i++){
		vector[i] = rand() % RANDOM_INTERVAL;
	}

	for(i=0;i<nprocs-1;i++)
		MPI_Scatter(vector,vector_size/nprocs,MPI_INT,slice,MPI_INT,0,MPI_COMM_WORLD);

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
