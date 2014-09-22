#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
int sum(int *vector,int size)
{

	int i;
	int soma=0;
	for(i=0;i<size;i++)
		soma += vector[i];
	return soma;
}

int main(int argc, char **argv)
{
        int rank, nprocs;
		int i;
        MPI_Status status;

        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	    int *convergencia = (int*) malloc(sizeof(int)*nprocs-1);
	
		for(i=0;i<nprocs-1;i++)
			convergencia[i]=0;
		
		convergencia[rank] = 1;

		for(i=0;i<nprocs;i++)
			MPI_Bcast(&convergencia[i], 1, MPI_INT, i, MPI_COMM_WORLD); 

		printf("Soma Rank %d: %d\n",rank,sum(convergencia,nprocs));

		if(rank==2){

			for(i=0;i<nprocs;i++)
				printf("%d ",convergencia[i]);

		}

		MPI_Finalize();
}
