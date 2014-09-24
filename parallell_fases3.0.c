#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>

#define RANDOM_INTERVAL 100 // Intervalo de preenchimento do vetor
#define PRINT 1 // booleano para impressao do vetor final
#define BORDER 10

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
	int received_minor;
	int received_major;
	int *vector_majors = (int*) malloc(sizeof(int)*BORDER);
	int *vector_minors = (int*) malloc(sizeof(int)*BORDER);
	int *returned_majors = (int*) malloc(sizeof(int)*BORDER);
	int converge = 1;
	int signal;
	int dev_count=0;

    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	MPI_Request request;

    int vetor_convergencia[nprocs]; // controle de convergencia
	
	for(i=0;i<nprocs;i++)
		vetor_convergencia[i] = 0;

	vetor_convergencia[nprocs-1] = 1; // O ultimo nao precisa convergir
	
	int ultimo_valor = (vector_size/nprocs)-1;

    int left = rank-1;
    int right = rank+1;

	// Popula Vetor
	if(rank==0){
		srand(time(0));
		for(i=0; i<vector_size; i++)
			vector[i] = rand() % RANDOM_INTERVAL;
	}

    // Dispara as fatias
	MPI_Scatter(vector,vector_size/nprocs,MPI_INT,slice,vector_size/nprocs,MPI_INT,0,MPI_COMM_WORLD);
	

while(sum(vetor_convergencia,nprocs) < nprocs)
{
/*
*   FASE 1 - Processamento Local
*/

	// Ordena fatias localmente
	bubble_sort(slice,vector_size/nprocs);

/*
*	 FASE 2 - CONDICAO DE PARADA
*/

	// Todos mandam para a esquerda o seu menor
	if(rank > 0)
		MPI_Send(&slice[0], 1, MPI_INT, left, 7, MPI_COMM_WORLD);

    // Todos recebem da direita para verificacao
	if(rank<nprocs-1){
		MPI_Recv(&received_minor, 1, MPI_INT, right, 7, MPI_COMM_WORLD,&status);
		if(slice[ultimo_valor] <= received_minor){
			converge = 0;
			vetor_convergencia[rank] = 1;
		}
		else{
			converge = 1;
		}
		MPI_Send(&converge, 1, MPI_INT, right, 2, MPI_COMM_WORLD);
	}

/*
*    FASE 3 - CONVERGENCIA
*/

	if(rank>0){
		// Verifica o sinal de convergencia. Caso sim, envia os menores para a esquerda e aguardo os maiores
		MPI_Recv(&signal, 1, MPI_INT, left, 2, MPI_COMM_WORLD,&status);
		if(signal){
			MPI_Send(slice, BORDER, MPI_INT, left, 8, MPI_COMM_WORLD);

			MPI_Recv(&dev_count,1,MPI_INT,left,9,MPI_COMM_WORLD,&status);
			MPI_Recv(slice,dev_count,MPI_INT,left,10,MPI_COMM_WORLD,&status);
		}
	}
	// Recebe os menores
	if(rank<nprocs-1){
		if(converge){
			MPI_Recv(vector_minors, BORDER, MPI_INT, right, 8, MPI_COMM_WORLD,&status);
         	// Armazeno os que preciso no meu vetor e devolvo os meus maiores na mesma quantidade
			int dev_count=0;
			for(i=0;i<BORDER;i++){
				if(vector_minors[i] <= slice[ultimo_valor-dev_count]){
					returned_majors[dev_count] = slice[ultimo_valor-dev_count];
					slice[ultimo_valor-dev_count] = vector_minors[i];
					dev_count++;
				}
			}
			MPI_Send(&dev_count, 1, MPI_INT, right, 9, MPI_COMM_WORLD);
			MPI_Send(returned_majors, dev_count, MPI_INT, right, 10, MPI_COMM_WORLD);
		}
	}
	// Dispara broadcast com o estado atual
    for(i=0;i<nprocs;i++)
        MPI_Bcast(&vetor_convergencia[i], 1, MPI_INT, i, MPI_COMM_WORLD);
}

	if(PRINT){
		printf("Rank %d: ",rank);
		for(i=0;i<(vector_size/nprocs);i++){
			printf("%d ",slice[i]);
		}
		printf("\n");
	}

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

int sum(int *vector,int size)
{

    int i;
    int soma=0;
    for(i=0;i<size;i++)
        soma += vector[i];
    return soma;
}
