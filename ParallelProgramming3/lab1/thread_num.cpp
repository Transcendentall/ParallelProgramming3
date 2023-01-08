#include <thread> 
#include <atomic> 
#include <omp.h>

std::atomic<unsigned> thread_num {std::thread::hardware_concurrency()};

void set_num_threads(unsigned T)
{
	if (T > std::thread::hardware_concurrency())
		T = std::thread::hardware_concurrency();
	thread_num = T;
	omp_set_num_threads(T);
}
unsigned get_num_threads()
{
	return thread_num;
}