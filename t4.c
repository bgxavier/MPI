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
	int *slice = (int*) malloc(sizeof(int)*vector_size);
	int verify;

    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	if(rank==0){
		srand(time(0));
		for(i=0; i<vector_size; i++)
			vector[i] = rand() % RANDOM_INTERVAL;
	}

	MPI_Scatter(vector,vector_size/nprocs,MPI_INT,slice,vector_size/nprocs,MPI_INT,0,MPI_COMM_WORLD);

	// Ordena fatias localmente
	bubble_sort(slice,vector_size/nprocs);

	printf("rank %d: ", rank);
	for(i=0;i<vector_size/nprocs;i++)
		printf("%d ", slice[i]);
	printf("\n");

	// Verifica convergencia

	if(rank!=0)
		MPI_Send(&slice[0], 1, MPI_INT,rank-1,0,MPI_COMM_WORLD);

	if(rank!=3)
		MPI_Recv(&verify,1,MPI_INT,rank+1,MPI_ANY_TAG,MPI_COMM_WORLD,&status);

	printf("Numero recebido por rank %d: %d\n",rank, verify);

    if (left < 0) left = MPI_PROCC_NULL;
    if (right > nprocs - 1) right = MPI_PROC_NULL;

	// Compara o menor recebido com o maior local
	if(verify < slice[(vector_size/nprocs)-1])
		MPI_Send(&slice[0], 1, MPI_INT,rank-1,0,MPI_COMM_WORLD);

		// Sem convergencia, efetua a troca
		slice[

	MPI_Finalize();
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
