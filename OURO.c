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
	int received_minor;
	int received_major;
	int convergir = 1;
	int send;
	int signal;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int vetor_convergencia[nprocs]; // Controle de convergencia
	
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
/*
    printf("rank %d: ",rank);
      for(i=0;i<vector_size/nprocs;i++)
           printf("%d ",slice[i]);
    printf("\n");
*/
	// FASE 1 - Processamento Local
while(sum(vetor_convergencia,nprocs) < nprocs)
{

	// Ordena fatias localmente
	bubble_sort(slice,vector_size/nprocs);

	// FASE 2 - CONDICAO DE PARADA

	// Todos mandam para a esquera o seu menor
	if(rank > 0)
		MPI_Send(&slice[0], 1, MPI_INT, left, 1, MPI_COMM_WORLD);

    // Todos recebem da direita para verificacao
	if(rank<nprocs-1){
		MPI_Recv(&received_major, 1, MPI_INT, right, 1, MPI_COMM_WORLD,&status);
		printf("SOU %d e recebi %d de %d para verificacao\n",rank, received_major,status.MPI_SOURCE);
		if(slice[ultimo_valor] < received_major){
			convergir = 0;
		}
		else
			convergir = 1;
	}

	// FASE 3 - Comunicar para convergir

	if(rank<nprocs-1){
		// Envia o maior e seta a variavel de retorno
		if(convergir){
			printf("SOU %d e preciso convergir\n",rank);
			send=1;
			//printf("ENVIANDO %d para %d\n",slice[ultimo_valor],right);
			MPI_Send(&slice[ultimo_valor], 1, MPI_INT, right, 1, MPI_COMM_WORLD);
		}
		// Nao enviar retorno
		else{
			vetor_convergencia[rank] = 1; // Ja esta convergido
			send=0;
		}
		// Mensagem para envio ou nao do retorno
		//printf("ENVIANDO sinal %d para %d\n",send,right);
		MPI_Send(&send, 1, MPI_INT, right, 2, MPI_COMM_WORLD);
	}

	if(rank>0){
		MPI_Recv(&signal, 1, MPI_INT, left, 2, MPI_COMM_WORLD,&status);
		printf("%d recebeu sinal %d de %d\n",rank,signal,status.MPI_SOURCE);
		// Se for para retornar, recebe o maior e devolve o menor
		if(signal==1){
			MPI_Recv(&received_major, 1, MPI_INT, left, 1, MPI_COMM_WORLD,&status);
			MPI_Send(&slice[0], 1, MPI_INT, left, 1, MPI_COMM_WORLD);	
			slice[0] = received_major;

		}
	}

	// Recebe o Retorno

	if(rank < nprocs-1){
		if(convergir == 1){
			MPI_Recv(&received_minor, 1, MPI_INT, right, 1, MPI_COMM_WORLD,&status);
			slice[ultimo_valor] = received_minor;
		}
	}

	// Dispara broadcast com o estado atual
    for(i=0;i<nprocs;i++)
        MPI_Bcast(&vetor_convergencia[i], 1, MPI_INT, i, MPI_COMM_WORLD);

	// Todos imprimem seu vetor de convergencia
	//printf("Convergencia do rank %d: ",rank);
	//printf("%d ",sum(vetor_convergencia,nprocs));
	//printf("\n");
}

    printf("rank %d: ",rank);
      for(i=0;i<vector_size/nprocs;i++)
           printf("%d ",slice[i]);
    printf("\n");

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
