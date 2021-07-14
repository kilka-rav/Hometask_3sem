#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>


#define ERROR(line, file, message) fprintf(stderr, "LINE = %d\tFILE = %s\tMESSAGE: %s\n", line, file, message)
#define print(s) fprintf(stdout, "%d\n", s)

typedef struct _Threads {
	int id;
	double result;
	double dx;
	int threads_num;
	pthread_t thread;
} Threads;

double integrate(int id, double dx, int num) {
	double a = ( (double) id )/ num;
	double b = ( (double) (id + 1)) / num;
	double result = 0;
	for(double i = a; i < b; i += dx) {
		result += dx * sqrt(1 - i * i);
	}
	return result;
}



void* count(void* param) {
	Threads* t = (Threads*) param;
	t->result = integrate(t->id, t->dx, t->threads_num);
	return NULL;
}

int main(int argc, char** argv) {
	if ( argc <= 1 ) {
		ERROR(__LINE__, __FILE__, "ERROR IN INPUT");
		return 1;
	}
	int threads_num = atoi(argv[1]);		//add handle_error
	int num = atoi(argv[2]);
	if ( ( num <= 0 ) || ( threads_num <= 0 ) ) {
		ERROR(__LINE__, __FILE__, "ERROR IN INPUT");
		return 2;
	}
	double res = 0;
	void* kek;
	Threads* threads = (Threads*) malloc(threads_num * sizeof(Threads));
	for(int i = 0; i < threads_num; ++i) {
		threads[i].id = i;
		threads[i].result = 0;
		threads[i].dx = 1.00 / num;
		threads[i].threads_num = threads_num;
		if ( pthread_create(&(threads[i].thread), NULL, count, &(threads[i])) == -1 ) {
			ERROR(__LINE__, __FILE__, "ERROR IN CREATE THREADS");
		}
	}
	for(int i = 0; i < threads_num; ++i) {
		if ( pthread_join(threads[i].thread, &(kek)) == -1 )  {
			ERROR(__LINE__, __FILE__, "ERROR IN JOIN THREADS");
		}
		res += threads[i].result;
	}
	printf("result = %lf\n", 4 * res);
	free(threads);
	return 0;
}

