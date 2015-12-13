#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LEN 134217728
#define RAND_LIMIT 256

long int result = 0;
int total_threads;
int *mas;

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

void *count(void *thread_number)
{
	int offset = LEN / total_threads * (*(int *)thread_number);
	int last_item = offset + LEN / total_threads;
	long int thread_sum = 0;
	for (int j = offset; j < last_item; j++)
		thread_sum += mas[j];
	printf("thread #%d, %ld\n", (*(int *)thread_number), thread_sum);
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
	
//	time_t start_t, end_t;
	srand(time(NULL));
	mas = (int*)malloc(LEN * sizeof(int));
	for (int i = 0; i < LEN; i++)
		mas[i] = 1;
		//mas[i] = random_at_most(RAND_LIMIT);

	total_threads = atoi(argv[1]);
	pthread_t *thr = (pthread_t *)malloc(total_threads * sizeof(pthread_t));
	int *thread_numbers = (int *)malloc(total_threads * sizeof(int));
	
//	time(&start_t);
	clock_t start = clock();
	for (int i = 0; i < total_threads; i++)
	{
		thread_numbers[i] = i;
		if (pthread_create(&thr[i], (pthread_attr_t *) NULL, count, (void *)&(thread_numbers[i])) != 0)
		{
			printf("Cant create threads\n");
			return -1;
		}
	}
	for (int i = 0; i < total_threads; i++)
		if (pthread_join(thr[i], (void **)NULL) != 0)
		{
			printf("Cant close thread\n");
			return -1;
		}
	clock_t end = clock();
//	time(&end_t);
	
	printf("The result is %ld, time: %.1f\n", result, (float)(end - start) / CLOCKS_PER_SEC);
	free(thread_numbers);
	free(thr);
	return 0;
}
