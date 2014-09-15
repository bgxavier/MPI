#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define RANDOM_INTERVAL 100 // Intervalo de preenchimento do vetor
#define PRINT 0 // booleano para impressao do vetor final

void bubble_sort(int *list, int n);
int *merge_sort(int *v1,int n1,int *v2,int n2);

int main(int argc, char **argv)
{
	double t1,t2;
	int i,j;
	int vector_size = atoi(argv[1]);
	int message_size,delta;
    int rank, nprocs;
	int *vector = (int *) malloc(sizeof(int)*vector_size);
	int *merge = (int *)malloc(message_size*sizeof(int));

	MPI_Status status;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	delta = vector_size/((nprocs+1)/2);
	
	if(rank == 0){
		/* Popula o vetor com numeros aleatorios */
    	srand(time(0));

    	for(i=0; i<vector_size; i++){
        	vector[i] = rand() % RANDOM_INTERVAL;
    	}

		t1 = MPI_Wtime(); // Captura o inicio da execucao

		// Envia vetores aos filhos
		MPI_Send(vector, vector_size/2, MPI_INT, rank*2+1, 0, MPI_COMM_WORLD);
		MPI_Send(vector+vector_size/2, vector_size/2, MPI_INT, rank*2+2, 0, MPI_COMM_WORLD);

		// Aguarda o recebimento
		MPI_Recv(vector, vector_size, MPI_INT, rank*2+1, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
	    MPI_Recv(vector+vector_size/2, vector_size, MPI_INT, rank*2+2, MPI_ANY_TAG, MPI_COMM_WORLD,&status);

		// Merge final
		merge = merge_sort(vector,vector_size/2,vector+vector_size/2,vector_size/2);

		t2 = MPI_Wtime(); // Captura fim da execucao

		printf("Tempo de execucao: %f\n", t2-t1);

		if(PRINT){
			for(i=0; i<vector_size;i++)
            	printf("%d ",merge[i]);
			printf("\n");
		}
	}
	else{
			MPI_Recv(vector, vector_size, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,&status);	
    		MPI_Get_count(&status, MPI_INT, &message_size);

			// conquista
			if(message_size == delta){
				bubble_sort(vector,message_size);
				MPI_Send(vector, message_size, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
			}
			// divide
			else{
				int parent = (rank-1)/2;
				int left = rank*2+1;
				int right = rank*2+2;

				// Envia para os filhos
        		MPI_Send(vector, message_size/2, MPI_INT, left, 0, MPI_COMM_WORLD);
        		MPI_Send(vector + message_size/2, message_size/2, MPI_INT, right, 0, MPI_COMM_WORLD);

				// Aguarda o Retorno, efetua o merge e sobe
				MPI_Recv(vector, message_size/2, MPI_INT, left, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
				MPI_Recv(vector + message_size, message_size/2, MPI_INT, right, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
				
				merge = merge_sort(vector,message_size/2,vector+message_size,message_size/2);

				MPI_Send(merge, message_size/2, MPI_INT, parent, 0,MPI_COMM_WORLD);
			}
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

int *merge_sort(int *v1,int n1,int *v2,int n2)
{  
   int i,j,k;
   int *saida;

   saida = (int *)malloc((n1+n2)*sizeof(int));

   i=0; j=0; k=0;
   while(i<n1 && j<n2)
    if(v1[i]<v2[j]){  
       saida[k] = v1[i];
       i++; k++;
    }
    else{  
       saida[k] = v2[j];
       j++; k++;
    }
   if(i==n1)
    while(j<n2){  
       saida[k] = v2[j];
       j++; k++;
    }
   else
   while(i<n1){
      saida[k] = v1[i];
      i++; k++;
   }
   return saida;
}
