#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <numeric>

unsigned get_num_threads();

float integrate_cpp(float a, float b, f_t f)
{
	double dx = (b - a) / n;
	unsigned T = get_num_threads();
	std::vector<element_t> results(T, element_t{0.0});
	auto thread_proc = [=, &results](unsigned t) {
		results[t].value = 0.0;
		for (size_t i = t; i < n; i += T)
			results[t].value += f((float) (dx * i + a));
	};
	std::vector<std::thread> threads;
	for (unsigned t = 1; t < T; ++t)
		threads.emplace_back(thread_proc, t);
	thread_proc(0);
	for (auto& thread:threads)
		thread.join();
	double res = 0.0;
	for (size_t i = 0; i < T; ++i)
		res += results[i].value;
	return (float) (res * dx);
} 

float integrate_cpp_cs(float a, float b, f_t f)
{
	double res = 0.0;
	double dx = (b - a) / n;
	unsigned T = get_num_threads();
	std::mutex mtx;
	auto thread_proc = [=, &res, &mtx](unsigned t) {
		double l_res = 0.0;
		for (size_t i = t; i < n; i += T)
			l_res += f((float) (dx * i + a));
		{
			std::lock_guard<std::mutex> lock(mtx);
			res += l_res;
		}
	};
	std::vector<std::thread> threads;
	for (unsigned t = 1; t < T; ++t)
		threads.emplace_back(thread_proc, t);
	thread_proc(0);
	for (auto& thread:threads)
		thread.join();
	return res * dx;
}

float integrate_cpp_atomic(float a, float b, f_t f) //C++20
{
	std::atomic<double> res = 0.0;
	double dx = (b - a) / n;
	unsigned T = get_num_threads();
	auto thread_proc = [=, &res](unsigned t) {
		double l_res = 0.0;
		for (size_t i = t; i < n; i += T)
			l_res += f((float) (dx * i + a));
		res = res + l_res;
	};
	std::vector<std::thread> threads;
	for (unsigned t = 1; t < T; ++t)
		threads.emplace_back(thread_proc, t);
	thread_proc(0);
	for (auto& thread:threads)
		thread.join();
	return res * dx; 
}

#include <iterator> 

class Iterator 
{
	f_t f;
	double dx, a;
	unsigned i = 0;
	public:
	typedef double value_type, *pointer, &reference;
	using iterator_category = std::input_iterator_tag;
	//Iterator() = default;
	Iterator(f_t fun, double delta_x, double x0, unsigned index):f(fun), dx(delta_x), a(x0), i(index) {}
	double value() const{
		return f(a + i * dx);
	}
	auto operator*() const {return this->value();}
	Iterator& operator++()
	{
		++i;
		return *this;
	}
	Iterator operator++(int)
	{
		auto old = *this;
		++*this;
		return old;
	}
	bool operator==(const Iterator& other) const
	{
		return i == other.i;
	}
};

#if !defined( __GNUC__) || (__GNUC__ > 10)
float integrate_cpp_reduce_1(float a, float b, f_t f)
{
	double dx = (b - a) / n;
	return std::reduce(Iterator(f, dx, a, 0), Iterator(f, dx, a, n)) * dx; 
}
#endif //__GNUC__ 

#include "reduce_par.h"
float integrate_cpp_reduce_2(float a, float b, f_t f) 
{
	double dx = (b - a) / n;
	return reduce_par_2([f, dx](double x, double y){return x + y;}, f, (double) a, (double) b, (double) dx, 0.0) * dx; 
}