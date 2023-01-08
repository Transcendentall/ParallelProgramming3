#include <iostream>

#define n 100000000u

#include "experiments.h"
#include "omp_integral.h"
#include "cpp_integral.h"

unsigned get_num_threads();

int main(int argc, char **argv) {
	experiment_result_t* results = (experiment_result_t*)malloc(get_num_threads()*sizeof(experiment_result_t));
	printf("------------------------------OpenMP------------------------\n");
    run_experiments(results, integrate_omp);
    show_results_for("integrate_omp", results);
	run_experiments(results, integrate_seq);
	show_results_for("integrate_omp_seq", results);
	run_experiments(results, integrate_omp_fs);
	show_results_for("integrate_omp_fs", results);
	run_experiments(results, integrate_omp_reduce);
	show_results_for("integrate_omp_reduce", results);
	run_experiments(results, integrate_omp_reduce_dyn);
	show_results_for("integrate_omp_reduce_dyn", results);
	run_experiments(results, integrate_omp_atomic);
	show_results_for("integrate_omp_atomic", results);
	run_experiments(results, integrate_omp_for);
	show_results_for("integrate_omp_for", results);
	run_experiments(results, integrate_omp_cs);
	show_results_for("integrate_omp_cs", results);
	run_experiments(results, integrate_omp_mtx);
	show_results_for("integrate_omp_mtx", results);
	printf("-----------------------------C++--------------------------\n");
	run_experiments(results, integrate_cpp);
	show_results_for("integrate_cpp", results);
	run_experiments(results, integrate_cpp_cs);
	show_results_for("integrate_cpp_cs", results);
	run_experiments(results, integrate_cpp_atomic);
	show_results_for("integrate_cpp_atomic", results);
#if !defined( __GNUC__) || (__GNUC__ > 10)
	run_experiments(results, integrate_cpp_reduce_1);
	show_results_for("integrate_cpp_reduce_1", results);
#endif
	run_experiments(results, integrate_cpp_reduce_2);
	show_results_for("integrate_cpp_reduce_2", results);
	free(results);
    return 0;
}