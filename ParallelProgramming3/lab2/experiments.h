#include <thread>
#include <omp.h>
#include <iomanip>

void set_num_threads(unsigned T);

typedef struct experiment_result_ {
    double result;
    double time;
    double speedup;
} experiment_result;

experiment_result* run_experiments(unsigned* V, unsigned count, double (*accumulate)(unsigned*, unsigned, unsigned, unsigned), unsigned min, unsigned max) {
    unsigned P = (unsigned)std::thread::hardware_concurrency();
    experiment_result* results = (experiment_result*)malloc(P * sizeof(experiment_result));
    for (unsigned i = 0; i < P; ++i) {
        double t0 = omp_get_wtime();
        set_num_threads(i + 1);
        results[i].result = accumulate(V, count, min, max);
        results[i].time = omp_get_wtime() - t0;
        results[i].speedup = results[0].time/results[i].time;
    }
    return results;
}

void print_experiment_results(const experiment_result* results){
    unsigned w = 10;
    std::cout << std::setw(w) << "T" << "\t"
    << std::setw(w) << "Time" << "\t"
    << std::setw(w) << "Result" << "\t"
    << std::setw(w) << "Speedup\n";
    for (unsigned T = 1; T <= std::thread::hardware_concurrency(); T++)
        std::cout << std::setw(w) << T << "\t"
        << std::setw(w) << results[T-1].time << "\t"
        << std::setw(w) << results[T-1].result<< "\t"
        << std::setw(w) << results[0].time/results[T-1].time << "\n";
}

void run_experiments_for(unsigned* V, unsigned count, double (*accumulate)(unsigned*, unsigned, unsigned, unsigned), unsigned min, unsigned max) {
    experiment_result* results = run_experiments(V, count, accumulate, min, max);
    print_experiment_results(results);
    free(results);
}
