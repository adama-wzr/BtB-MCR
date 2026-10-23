#ifndef YTA
#define YTA

#include <ImportAll_core.hpp>
#include <stdio.h>
#include <omp.h>
#include <iostream>

#ifdef USE_MPI
#include <mpi.h>
#endif


int YTA_2D(options *opts);


#endif
