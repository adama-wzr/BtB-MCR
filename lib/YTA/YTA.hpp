#ifndef YTA
#define YTA

#include <ImportAll_core.hpp>
#include <ImportAll_utils.hpp>
#include <stdio.h>
#include <omp.h>
#include <iostream>
#include <ctime>
#include <filesystem>


#ifdef USE_MPI
#include <mpi.h>
#endif

/*
 *
 *  Utils around YTA
 *
 * */

void YTA_baseFree2D(sYTA *info, meshInfo *target, meshInfo *rec);


void YTA_baseAlloc2D(sYTA *info, meshInfo *target, meshInfo *rec);

void saveStack(unsigned char *recData, meshInfo *rec);

/*
 *
 *  Local Correlation Functions
 *
 * */


void localChord(meshInfo *rec, unsigned char *recData, int row, int col, int swapFlag);

void localTP(sYTA *opts, meshInfo *rec, unsigned char *recData, int row, int col, int swapFlag);

void localChord3D(meshInfo *rec, unsigned char *recData, int slice, int row, int col, int swapFlag);

void localTP3D(sYTA *opts, meshInfo *rec, unsigned char *recData, int slice, int row, int col, int swapFlag);

/*
 *
 *  Actual YTA Implementation
 *
 * */

#ifdef USE_MPI 
    int YTA_2D(options *opts, MPI_Comm Comm);
    int YTA_3D(options *opts, MPI_Comm comm);
#else
    int YTA_2D(options *opts);
#endif



#endif
