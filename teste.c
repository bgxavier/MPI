#include <stdio.h>
#include <stdlib.h>
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

		
	    int *convergencia = (int*) malloc(sizeof(int)*10);
		int *teste;
		teste[0] = 13;
		teste[1] = 12;
	
	memcpy(convergencia + 5, teste, 1);
 	memcpy(convergencia + 6, teste+1, 1);

		for(i=0;i<10;i++)
			printf("%d ",convergencia[i]);
	
}
