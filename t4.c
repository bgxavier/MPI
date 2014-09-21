#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>

#define RANDOM_INTERVAL 100 // Intervalo de preenchimento do vetor
#define PRINT 1 // booleano para impressao do vetor final

void bubble_sort(int *list, int n);
int sum(int *vector, int size);

int main(int argc, char **argv)
{
    double t1,t2;
    int i,j;
    int vector_size = atoi(argv[1]);
    int rank, nprocs;
	int *vector = (int *) malloc(sizeof(int)*vector_size);
	int *slice = (int*) malloc(sizeof(int)*vector_size);
	int verify = 0;

    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	int left = rank-1;
	int right = rank+1;

	if(right > nprocs-1) right = MPI_PROC_NULL;
	if(left < 0 ) left = MPI_PROC_NULL;

	int ultimo_valor = (vector_size/nprocs)-1;

	int *convergencia = (int*) malloc(sizeof(int)*2);

	memset(convergencia,0,sizeof(convergencia));

	if(rank==0){
		srand(time(0));
		for(i=0; i<vector_size; i++)
			vector[i] = rand() % RANDOM_INTERVAL;
	}

	MPI_Scatter(vector,vector_size/nprocs,MPI_INT,slice,vector_size/nprocs,MPI_INT,0,MPI_COMM_WORLD);

	while(sum(convergencia,nprocs) < nprocs){

		// Ordena fatias localmente
		bubble_sort(slice,vector_size/nprocs);

		// Todos mandam para esquerda o seu menor para verificar convergencia
		MPI_Send(&slice[0], 1, MPI_INT, left,0,MPI_COMM_WORLD);
		
		// Todos recebem da direita os menores para verificar convergencia
		MPI_Recv(&verify, 1, MPI_INT,right,0,MPI_COMM_WORLD,&status);
		
    	// Compara o menor recebido com o maior local nos processos menores que o ultimo
    	if(verify < slice[ultimo_valor]){

			// Envia o maior para a direita
			MPI_Send(&slice[ultimo_valor],1,MPI_INT,right,0,MPI_COMM_WORLD);
			
			// Coloco o menor recebido da direita na ultima posicao do meu buffer
			if(rank!=nprocs-1)
				slice[ultimo_valor] = verify;

			// Recebendo o maior valor da esquerda e alocando na primeira posicao do buffer
			MPI_Recv(&slice[0],1,MPI_INT,left,0,MPI_COMM_WORLD,&status);
		}

		else{
			convergencia[rank] = 1;
			for(i=0;i<nprocs;i++)
                MPI_Bcast(&convergencia[i],1,MPI_INT,i,MPI_COMM_WORLD);
        }
		
	}

	// Imprime estado atual
    printf("rank %d: ",rank);
      for(i=0;i<vector_size/nprocs;i++)
	       printf("%d ",slice[i]);
    printf("\n");

	MPI_Finalize();
}

int sum(int *vector, int size)
{
	int i;
	int soma=0;
	for(i=0;i<size;i++)
		soma += vector[i];
	return soma;
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
