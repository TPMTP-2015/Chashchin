#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define THREAD_NUM 16
#define LOCKS 10000000

long int result = 0;
pthread_mutex_t mutex;
pthread_spinlock_t spinlock;

void *mutex_job(void *data)
{
	for (int i = 0; i < LOCKS; i++)
	{
		pthread_mutex_lock(&mutex);
		result++;
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void *spinlock_job(void *data)
{
	for (int i = 0; i < LOCKS; i++)
	{
		pthread_spin_lock(&spinlock);
		result++;
		pthread_spin_unlock(&spinlock);
	}
	return NULL;
}

int main()
{
	pthread_t *thr = (pthread_t *)malloc(THREAD_NUM * sizeof(pthread_t));
	
	pthread_mutex_init(&mutex, NULL);
	
	clock_t start = clock();
	for (int i = 0; i < THREAD_NUM; i++)
	{
		if (pthread_create(&thr[i], (pthread_attr_t *) NULL, mutex_job, NULL) != 0)
		{
			printf("Cant create threads\n");
			return -1;
		}
	}
	
	for (int i = 0; i < THREAD_NUM; i++)
		if (pthread_join(thr[i], (void **)NULL) != 0)
		{
			printf("Cant close thread\n");
			return -1;
		}
	clock_t end = clock();
	printf("Mutex: result=%ld, time=%.1f\n", result, (float)(end - start) / CLOCKS_PER_SEC / 2);
	result = 0;
	
	pthread_mutex_destroy(&mutex);
	
	//spinlock
	pthread_spin_init(&spinlock, PTHREAD_PROCESS_SHARED);
	start = clock();
	for (int i = 0; i < THREAD_NUM; i++)
	{
		if (pthread_create(&thr[i], (pthread_attr_t *) NULL, spinlock_job, NULL) != 0)
		{
			printf("Cant create threads\n");
			return -1;
		}
	}
	
	for (int i = 0; i < THREAD_NUM; i++)
		if (pthread_join(thr[i], (void **)NULL) != 0)
		{
			printf("Cant close thread\n");
			return -1;
		}
	end = clock();
	printf("Spinlock: result=%ld, time=%.1f\n", result, (float)(end - start) / CLOCKS_PER_SEC / 2);
	
	pthread_spin_destroy(&spinlock);
	
	free(thr);
}
