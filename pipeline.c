#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mpi.h"

#define NUM_VECTORS 10000 // numero de vetores
#define INTERVAL 100 // intervalo de preenchimento
#define LIMIT 10000 // tamanho dos vetores
#define PRINT 0 // booleando para imprimir vetores no final da execucao

// Funcao para imprimir o vetor sem zeros e 1
void print_clean_vector(int *v, int size)
{
    int i;
    for(i=0;i<size;i++){
        if(v[i] && v[i]!=1)
            printf("%d ",v[i]);
    }

}

main(int argc, char** argv){

    int current;
    int vector_counter=0;
    int i,j;
    double t1,t2;
    int my_rank,dest,source,proc_n;
    int *numbers = (int *) malloc(LIMIT * sizeof(int));
    int square_r_limit = (int) sqrt(INTERVAL); // calcula a raiz quadrada sobre o intervalo de valores

    /*
    for(i=0;i<NUM_VECTORS;i++){
        numbers[i] = malloc(NUM_VECTORS * LIMIT * sizeof(int));
    }
    */
    MPI_Status status;

    MPI_Init (&argc , & argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

    srand(time(0));
    /*
       O primeiro processo cria os vetores com valores aleatorios dentro do intervalo definido
       na constante  INTERVAL
    */
    t1 = MPI_Wtime();
    while(vector_counter < NUM_VECTORS){

    if (my_rank == 0){
        current=2;
        for(i=0;i<LIMIT;i++){
            int value = rand() % INTERVAL;
            numbers[i] = value;
        }
    }

    else{
    // Recebe da esquerda
        MPI_Recv(numbers,LIMIT,MPI_INT, my_rank-1, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // recebe vetor
        MPI_Recv(&current,1,MPI_INT,my_rank-1,MPI_ANY_TAG,MPI_COMM_WORLD,&status); // recebe contador
        MPI_Recv(&vector_counter,1,MPI_INT,my_rank-1,MPI_ANY_TAG,MPI_COMM_WORLD,&status); // recebe contador de vetores
    }
    // Se o valor atual do contador eh inferior a raiz quadrada do limite, processa

    //printf("Estagio %d Processando vetor %d\n", my_rank,vector_counter);

    if(current <= square_r_limit){
        for(i=0;i<LIMIT;i++){
            if((numbers[i] != current ) && (numbers[i] % current == 0))
                numbers[i]=0;
        }
        current++; // incrementa contador
    }

    // Ultimo processo imprime o vetor final e o tempo de execucao 
    if(my_rank == proc_n-1){
        t2 = MPI_Wtime();

        if(PRINT){
			printf("Vetor %d: ", vector_counter);
        	print_clean_vector(numbers,LIMIT);
        	printf("\n");
		}

        if(vector_counter == NUM_VECTORS-1)
            printf("Tempo de execucao: %f\n", t2-t1);
    }
    // Envia para a direita
    else{
        MPI_Send(numbers,LIMIT,MPI_INT, my_rank+1,1,MPI_COMM_WORLD); // enviar o vetor para a direita
        MPI_Send(&current,1,MPI_INT,my_rank+1,1,MPI_COMM_WORLD); // envia contador
        MPI_Send(&vector_counter,1,MPI_INT,my_rank+1,1,MPI_COMM_WORLD); // envia contador de vetores
    }
    vector_counter++;
    }
    MPI_Finalize();
}

