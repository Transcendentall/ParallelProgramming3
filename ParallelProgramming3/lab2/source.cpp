#include <iostream>
#include "experiments.h"
#include "generator.h"

int main() {
    unsigned T = get_num_threads();
    int N = 10000000;
    unsigned* V = new unsigned[N];
    run_experiments_for(V, N, randomize, 1, 99);
    return 0;
}