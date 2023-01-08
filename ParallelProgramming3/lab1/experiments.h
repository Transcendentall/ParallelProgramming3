#include <omp.h>
#include <thread>
#include <iomanip>

typedef float (*f_t) (float);

void set_num_threads(unsigned T);

float g(float x)
{
	return x * x;
}

#ifdef _MSC_VER
constexpr std::size_t CACHE_LINE = std::hardware_destructive_interference_size;
#else
#define CACHE_LINE 64
#endif

typedef struct element_t_
{
	alignas(CACHE_LINE) double value;
} element_t;

typedef struct experiment_result_t_ 
{
	float result;
	double time;
} experiment_result_t; 

typedef float (*integrate_t)(float a, float b, f_t f);
experiment_result_t run_experiment(integrate_t integrate) 
{
	experiment_result_t result;
	double t0 = omp_get_wtime();
	result.result = integrate(-1, 1, g);
	result.time = omp_get_wtime() - t0;
	return result; 
} 

void run_experiments(experiment_result_t* results, float (*I) (float, float, f_t))
{
	for (unsigned T = 1; T <= std::thread::hardware_concurrency(); ++T)
	{
		set_num_threads(T);
		results[T-1] = run_experiment(I);
	}
}

void show_results_for(const char* name, const experiment_result_t* results)
{
    unsigned w = 10;
    std::cout << name << "\n";
    std::cout << std::setw(w) << "T" << "\t"
    << std::setw(w) << "Time" << "\t"
    << std::setw(w) << "Result" << "\t"
    << std::setw(w) << "Speedup\n";
    for (unsigned T = 1; T <= omp_get_num_procs(); T++)
        std::cout << std::setw(w) << T << "\t"
        << std::setw(w) << results[T-1].time << "\t"
        << std::setw(w) << results[T-1].result<< "\t"
        << std::setw(w) << results[0].time/results[T-1].time << "\n";
};