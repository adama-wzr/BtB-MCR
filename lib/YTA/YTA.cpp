
#include <YTA.hpp>

int YTA_2D(options *opts)
{
    fflush(stdout);
    omp_set_num_threads(opts->nThreads);

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        printf("Hello from thread %d out of %d\n", tid, opts->nThreads);
    }

    return 0;
}
