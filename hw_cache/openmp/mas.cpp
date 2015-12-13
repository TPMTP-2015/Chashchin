#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LEN 134217728
#define RAND_LIMIT 256

int nthreads;
int *mas;
long int result = 0;

long random_at_most(long max) {
	unsigned long
    num_bins = (unsigned long) max + 1,
    num_rand = (unsigned long) RAND_MAX + 1,
    bin_size = num_rand / num_bins,
    defect   = num_rand % num_bins;

	long x;
	do
		x = random();
	while (num_rand - defect <= (unsigned long)x);
	
	return x / bin_size;
}

void *count(int thread_number)
{
	int offset = LEN / nthreads * thread_number;
	int last_item = offset + LEN / nthreads;
	long int thread_sum = 0;
	for (int j = offset; j < last_item; j++)
		thread_sum += mas[j];
	printf("thread #%d, %ld\n", thread_number, thread_sum);
	result += thread_sum;
	return NULL;
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("wrong number of arguments!\n");
		return -1;
	}
	
	double wall_timer;
	mas = (int*)malloc(LEN * sizeof(int));
	for (int i = 0; i < LEN; i++)
		mas[i] = random_at_most(RAND_LIMIT);

	nthreads = atoi(argv[1]);
	omp_set_num_threads(nthreads);
	wall_timer = omp_get_wtime();
	#pragma omp parallel for
		for (int i = 0; i < nthreads; i++)
		{
			printf("%d\n", i);
			count(i);
		}

	
	printf("The result is %ld, time: %f\n", result, omp_get_wtime() - wall_timer);
	return 0;
}
