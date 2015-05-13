/*

Autor: Bruno Gomes Xavier

Execucao: 
	
	master_slave <tamanho dos vetores> <numero de vetores>

*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define RANDOM_INTERVAL 100
#define KILLTAG 99999999
#define PRINT 0 // booleano para impressao dos vetores finais

void master(int vector_size, int num_vectors);
void slave(int vector_size);

int main(int argc, char **argv)
{
    if(argc > 2){
        int vector_size = atoi(argv[1]);
        int num_vectors = atoi(argv[2]);
        int rank, nprocs;

        MPI_Status status;

        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

        if(rank == 0){
            master(vector_size, num_vectors);
        }
        else{
            slave(vector_size);
        }

        MPI_Finalize();
    }
    else{
        printf("Usage: %s vector_size num_vectors\n",argv[0]);
    }
}

void master(int vector_size, int num_vectors)
{
    int i,j;
    double t1,t2;
    int received_vectors=0;
    int next_vector;
    int nprocs,rank;
    int **vectors_bag = (int **)malloc(num_vectors * sizeof(int *));

    for(i=0;i<num_vectors;i++){
        vectors_bag[i] = (int*)malloc(vector_size * sizeof(int));
    }

    /* Popula Vetores com numeros aleatorios */
    srand(time(0));

    for(i=0; i<num_vectors; i++){
        for(j=0 ; j<vector_size; j++){
            vectors_bag[i][j] = rand() % RANDOM_INTERVAL;
        }
    }

    MPI_Status status;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    /* Envia os vetores para os slaves. A TAG e utilizada para marcar a posicao do vetor enviado */
    t1 = MPI_Wtime();

    for(rank=1; rank < nprocs; rank++){
        next_vector = rank-1;
        MPI_Send(vectors_bag[next_vector], vector_size, MPI_INT,rank,next_vector,MPI_COMM_WORLD);
    }

    /* Aguarda o recebimento dos vetores */
    while(received_vectors < num_vectors ){

        /* Verifica a proxima TAG a ser recebida */
        MPI_Probe(MPI_ANY_SOURCE,MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        /* Utiliza a TAG para reinserir o vetor ordenado na sua posicao */
        MPI_Recv(vectors_bag[status.MPI_TAG],vector_size,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);

        received_vectors++;
        next_vector++;

        /* Envia o proximo vetor para o slave que se liberou */
        if(next_vector < num_vectors)
            MPI_Send(vectors_bag[next_vector], vector_size, MPI_INT,status.MPI_SOURCE,next_vector,MPI_COMM_WORLD);

    }
    t2 = MPI_Wtime();
    /* Envia a tag de kill para que os slaves parem o processamento */
    for(rank = 1;rank<nprocs;rank++)
        MPI_Send(0,0,MPI_INT,rank,KILLTAG,MPI_COMM_WORLD);

    /* Imprime o tempo final */
    printf("Tempo de execucao: %f\n", t2-t1);

    if(PRINT==1){
        for(i=0;i<num_vectors;i++){
            for(j=0;j<vector_size;j++)
                printf("%d ",vectors_bag[i][j]);
            printf("\n");
        }
    }
}

/* Funcao comparativa para o qsort */
int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

void slave(int vector_size)
{
    int nprocs,rank;
    int *vector = (int*) malloc(vector_size * sizeof(int));

    MPI_Status status;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Recebe os vetores do master */
    while(1){

        MPI_Recv(vector, vector_size, MPI_INT, 0, MPI_ANY_TAG,MPI_COMM_WORLD, &status);

        if (status.MPI_TAG == KILLTAG) {
            break;
        }

    /* Ordena Vetor e devolve ao master */
    qsort(vector, vector_size, sizeof(int), cmpfunc);
    MPI_Send(vector, vector_size, MPI_INT, 0, status.MPI_TAG, MPI_COMM_WORLD);
    }
}
