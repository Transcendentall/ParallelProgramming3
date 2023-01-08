#include <vector>
#include <thread>

#define A 134775813
#define B 1
#define C 4294967296

unsigned get_num_threads();

std::vector<unsigned> pow_A(unsigned T) {
    std::vector<unsigned> result;
	result.reserve(T);
    result.emplace_back(A);
    for (unsigned i = 1; i < T + 1; i++) {
		unsigned next_A = (result[i-1] * A) % C;
        result.emplace_back(next_A);
    }
    return result;
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

double randomize(unsigned* V, unsigned N, unsigned min, unsigned max) {

    unsigned T = get_num_threads();
    std::vector<unsigned> multipliers = pow_A(T);
    double sum = 0;
    std::vector<element_t> partial(T, element_t{0.0});
    std::vector<std::thread> threads;
    unsigned seed = std::time(0);
    for (std::size_t t = 0; t < T; ++t)
        threads.emplace_back([t, T, V, N, seed, &partial, multipliers, min, max]() {
        auto At = multipliers.back();
        unsigned off = (B * (At - 1) / (A - 1)) % C;
            unsigned x = ((seed * multipliers[t]) % C + (B % C * (multipliers[t] - 1) / (A - 1)) % C) % C;
            double acc = 0;
            for (size_t i = t; i < N; i += T) {
                V[i] = x % (max - min) + min;
                acc += V[i];
                x = (x * At) % C + off % C;
            }
            partial[t].value = acc;
            });
    for (auto& thread:threads)
        thread.join();
    for (unsigned i = 0; i < T; ++i)
        sum += partial[i].value;
    return sum / N;
}

