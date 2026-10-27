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

/*
 *
 *  Utils around YTA
 *
 * */

int YTA_2D_Init(options *opts, meshInfo *target, meshInfo *rec, sYTA *simInfo, unsigned char *targetData, unsigned char *recData);


void YTA_baseFree2D(sYTA *info, meshInfo *target, meshInfo *rec);


void YTA_baseAlloc2D(options *opts, sYTA *info, meshInfo *target, meshInfo *rec);


/*
 *
 *  Local Correlation Functions
 *
 * */


void localChord(meshInfo *rec, unsigned char *recData, int row, int col, int swapFlag);

void localTP(options *opts, meshInfo *rec, unsigned char *recData, int row, int col, int swapFlag);


/*
 *
 *  Actual YTA Implementation
 *
 * */

#ifdef USE_MPI 
    int YTA_2D(options *opts, MPI_Comm Comm);

#else
    int YTA_2D(options *opts);
#endif



#endif
