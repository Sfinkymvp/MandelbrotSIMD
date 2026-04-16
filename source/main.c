#include <stdio.h>

#include "fractal.h"


int main()
{
#ifdef BENCHMARK 

    unsigned long long totalTicks = RunFractal(BENCH_ITERS);
    // double average = (double)totalTicks / BENCH_ITERS;

    printf("[Benchmark]\n");
    printf("  Configuration:    %s\n", CONFIG);
    printf("  Total iterations: %d\n", BENCH_ITERS);
    printf("  Total ticks:      %llu\n", totalTicks);
    // printf("  Average ticks:    %.2f\n", average);
    printf("  Average ticks:    %llu\n", totalTicks / BENCH_ITERS);
#else

    RunFractal();
#endif // BENCHMARK
    return 0;
}
