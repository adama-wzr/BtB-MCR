#ifndef YTA
#define YTA

#include <ImportAll_core.hpp>
#include <ImportAll_utils.hpp>
#include <stdio.h>
#include <omp.h>
#include <iostream>

#ifdef USE_MPI
#include <mpi.h>
#endif


int YTA_2D_Init(options *opts, meshInfo *target, meshInfo *rec, sYTA *simInfo, unsigned char *targetData, unsigned char *recData);



#ifdef USE_MPI 
    int YTA_2D(options *opts, MPI_Comm Comm);

#else
    int YTA_2D(options *opts);
#endif



#endif
