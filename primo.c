#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DELAY_MSG_MICRO 100 * 1000 // (args in microseconds, need to multiply millis with 1k)

#define TRUE 1
#define FALSE 0

#define TASK_INI_NUMBER 1

int primeTest (int n) {
	int i = 2;
	int ret = TRUE;
	int topLimit = n / 2;

	if(n == 1) {
			ret = FALSE;
	}

	for(;i <= topLimit && ret; i++) {
		if(n % i == 0) {
			ret = FALSE;
		}
	}
	return ret;
}

int findPrimeInInterval(int ini, int end, int * primes) {
	int ret = 0;
	int i;
	for(i = ini; i <= end; i++) {
		if(primeTest(i)) {
				ret++;
				primes[++primes[0]] = i;
		}
	}
	return ret;
}

int taskDivision(int size, int topLimitTest) {
	int i;
	int buffLimit = topLimitTest;
	int divMaster = 0;

	if(size > 0) {

		divMaster = topLimitTest / size;
		buffLimit -= divMaster;

		for(i = 1; i < size; i++) {
			int div = topLimitTest / size;
			buffLimit -= div;
			if(buffLimit != 0 && i == (size - 1)) {
				div += buffLimit;
			}
			MPI_Send(&div, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

	}

	return divMaster;
}

char * reallocStr(char * src, int n) {
	char * dummy = realloc(src, sizeof(char) * n);
	if(dummy == NULL) {
		printf("ERRO AO ALOCAR MEMORIA PARA STRING DE SAIDA! Fechando o programa...");
		free(dummy);
		exit(-1);
	}
	return dummy;
}

char * printPrimesArray(int * primes) {
	int i;
	int size = primes[0];
	char * buffer = (char *) malloc(sizeof(char));
	char bufferNum[255];
	int totalChars = 0;
	char * dummy = NULL;

	bufferNum[0] = buffer[0] = 0;

	for(i = 1; i <= size; i++) {
		totalChars += snprintf(bufferNum, 255, "%d ", primes[i]);
		buffer = reallocStr(buffer, totalChars + 1);
		buffer[totalChars] = 0;
		strcat(buffer, bufferNum);
	}

	return buffer;
}

int main (int argc, char* argv[]) {
	int rank, size;
	int topLimit;
	int taskEnd;
	int taskIni;
	int taskSize;
	int taskTotalFound = 0;
	int * primesFound;
	char * taskFoundString;
	MPI_Status status;

	if(argc < 2) {
		printf("ERRO! Nao ha parametros suficientes.");
		exit(-1);
	}

	topLimit = atoi(argv[1]);

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0) {
		printf("Buscando primos entre %d e %d ...\n", TASK_INI_NUMBER, topLimit);
		taskSize = taskDivision(size, topLimit);
		taskIni = TASK_INI_NUMBER;
		taskEnd = taskSize;
		primesFound = (int *) malloc(sizeof(int) * (taskSize + 1));
	} else {
		int divRecv;
		MPI_Recv(&divRecv, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		taskSize = divRecv;
		taskIni = (topLimit / size) * rank;
		taskEnd = taskIni + taskSize;
		taskIni += 1;
		primesFound = (int *) malloc(sizeof(int) * taskSize);
	}

	primesFound[0] = 0;
	taskTotalFound = findPrimeInInterval(taskIni, taskEnd, primesFound);
 	taskFoundString = printPrimesArray(primesFound);

	usleep(rank * DELAY_MSG_MICRO);

	if(taskTotalFound != 0) {
		printf("Processo rank %d buscou %d inteiros entre %d e %d, e encontrou %d primos: %s\n", rank, taskSize, taskIni, taskEnd, taskTotalFound, taskFoundString);
	} else {
		printf("Processo rank %d buscou %d inteiros entre %d e %d, e nao encontrou primos\n", rank, taskSize, taskIni, taskEnd);
	}

	free(primesFound);
	free(taskFoundString);

	if(rank == 0) {
		int i;
		int totalPrimesFound = taskTotalFound;
		for(i = 1; i < size; i++) {
			int recvFound = 0;
			MPI_Recv(&recvFound, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			totalPrimesFound += recvFound;
		}
		usleep(size * DELAY_MSG_MICRO);
		printf("Foram encontrados %d primos no total entre %d e %d\n", totalPrimesFound, TASK_INI_NUMBER, topLimit);
	} else {
		MPI_Send(&taskTotalFound, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();

	return 0;
}
