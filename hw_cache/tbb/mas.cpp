#include <iostream>
#include <stdlib.h>
#include "include/tbb/task_scheduler_init.h"
#include "include/tbb/parallel_reduce.h"
#include "include/tbb/blocked_range.h"
#include "include/tbb/tick_count.h"
#include <time.h>

#define RAND_LIMIT 256

using namespace tbb;
using namespace std;

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

class Count {
	long *mas;
	public:
	mutable long res;
	void operator() (const blocked_range<int> &r) const
	{
		for (int i = r.begin(); i != r.end(); i++)
			res += mas[i];
	}
	Count(Count &t, tbb::split) : mas(t.mas), res(0){ }
	
	void join(Count &t)
    {
        res += t.res;
    }
    
    Count(long *a) : mas(a), res(0) { }
};

int main(int argc, char *argv[]) {
	if (argc != 3)
	{
		cout << "wrong number of arguments!" << endl;
		return -1;
	}
	
	int nthreads = atoi(argv[1]);
	long int len = atoi(argv[2]);
	long *mas = new long[len];
	
	task_scheduler_init init(nthreads);
	srand(time(NULL));
	for (int i = 0; i < len; i++)
		mas[i] = random_at_most(RAND_LIMIT);
	
	Count res(mas);
	
	tick_count t0 = tick_count::now();
	tbb::parallel_reduce(tbb::blocked_range<int>(0, len, 1000), res);
	double time = (tick_count::now() - t0).seconds();
	cout << "The result is: " << res.res << endl;
	cout << "TBB summing time: " << time << " ticks" << endl;

	delete[] mas;

	return 0;
}
