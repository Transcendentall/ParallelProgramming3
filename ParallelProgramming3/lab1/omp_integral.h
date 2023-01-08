#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

float integrate_seq(float a, float b, f_t f)
{
	double res = 0.;
	double dx = (b - a) / n;
	for (size_t i = 0; i < n; ++i)
		res += f((float) (dx * i + a));
	return (float) (res * dx);
}

float integrate_omp_fs(float a, float b, f_t f) //false sharing
{
	double dx = (b - a) / n;
	double* results;
	double res = 0.0;
	unsigned T;
	#pragma omp parallel shared(results, T)
	{
		unsigned t = (unsigned) omp_get_thread_num(); 
		#pragma omp single
		{
			T = (unsigned) omp_get_num_threads();
			results = (double*) calloc(sizeof(double), T);
			if (!results)
				abort();
		} //Барьер
		for (size_t i = t; i < n; i += T)
			results[t] += f((float) (dx * i + a));
	}
	for (size_t i = 0; i < T; ++i)
		res += results[i];
	free(results);
	return (float) (res * dx);
}

#ifndef __cplusplus
#ifdef _MSC_VER
#define alignas(X) __declspec(align(X))
#else
#include <stdalign.h>
#endif 
#endif
#ifdef _MSC_VER
#define aligned_alloc(alignment, size) _aligned_malloc((size), (alignment))
#define aligned_free(ptr) _aligned_free(ptr)
#else
#define aligned_free(ptr) free(ptr)
#endif

void *aligned_alloc (size_t __alignment, size_t __size)
{
    void *retval = malloc(__size + __alignment);
    size_t rem = ((size_t)retval) % __alignment;
    if (rem) {
        retval = (void *)((size_t)retval + __alignment - rem);
    }
    return retval;
}

float integrate_omp(float a, float b, f_t f) 
{
	double dx = (b - a) / n;
	element_t* results;
	double res = 0.0;
	unsigned T;
	#pragma omp parallel shared(results, T)
	{
		unsigned t = (unsigned) omp_get_thread_num();
		#pragma omp single
		{
			T = (unsigned) omp_get_num_threads();
			results = (element_t*) aligned_alloc(CACHE_LINE, T*sizeof(element_t)); //Alignment: aligned_alloc
			if (!results)
				abort();
			} //Барьер
			results[t].value = 0.0;
			for (size_t i = t; i < n; i += T)
				results[t].value += f((float) (dx * i + a));
		}
		for (size_t i = 0; i < T; ++i)
			res += results[i].value;
//	aligned_free(results);
	return (float) (res * dx);
}

float integrate_omp_cs(float a, float b, f_t f)
{
	double res = 0.0;
	double dx = (b - a) / n;
	#pragma omp parallel shared(res)
	{
		unsigned t = (unsigned) omp_get_thread_num();
		unsigned T = (unsigned) omp_get_num_threads();
		double local_sum = 0.0;
		for (size_t i = t; i < n; i += T)
			local_sum += f((float) (dx * i + a));
		#pragma omp critical
		{
			res += local_sum;
		}
	}
	return (float) (res * dx);
}

float integrate_omp_mtx(float a, float b, f_t f)
{
	double res = 0.0;
	double dx = (b - a) / n;
	omp_lock_t mtx;
	omp_init_lock(&mtx);
	#pragma omp parallel shared(res)
	{
		unsigned t = (unsigned) omp_get_thread_num();
		unsigned T = (unsigned) omp_get_num_threads();
		double local_sum = 0.0;
		for (size_t i = t; i < n; i += T)
			local_sum += f((float) (dx * i + a));
		omp_set_lock(&mtx);
			res += local_sum;
		omp_unset_lock(&mtx);
	}
	return (float) (res * dx);
}

float integrate_omp_atomic(float a, float b, f_t f)
{
	double res = 0.0;
	double dx = (b - a) / n;
	#pragma omp parallel shared(res)
	{
		unsigned t = (unsigned) omp_get_thread_num();
		unsigned T = (unsigned) omp_get_num_threads();
		double local_sum = 0.0;
		for (size_t i = t; i < n; i += T)
			local_sum += f((float) (dx * i + a));
		#pragma omp atomic
			res += local_sum;
	}
	return (float) (res * dx);
}

float integrate_omp_for(float a, float b, f_t f)
{
	double res = 0.0;
	double dx = (b - a) / n;
	int i;
	#pragma omp parallel for shared (res)
	for (i = 0; i < n; ++i)
	{
		double val = f((float) (dx * i + a));
		#pragma omp atomic
		res += val;
	}
	return (float) (res * dx);
}

float integrate_omp_reduce(float a, float b, f_t f)
{
	double res = 0.0;
	double dx = (b - a) / n;
	int i;
	#pragma omp parallel for reduction(+: res) //schedule(static)
	for (i = 0; i < n; ++i)
		res += f((float) (dx * i + a));
	return (float) (res * dx);
} 

float integrate_omp_reduce_dyn(float a, float b, f_t f) 
{
	double res = 0.0;
	double dx = (b - a) / n;
	int i;
	#pragma omp parallel for reduction(+: res) schedule (dynamic) //guided - нагрузка ожидается одинаковая, но задачи могут поступать в разное время, auto, runtime
	for (i = 0; i < n; ++i)
		res += f((float) (dx * i + a));
	return (float) (res * dx);
}