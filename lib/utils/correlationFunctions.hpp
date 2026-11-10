#ifndef _CORF
#define _CORF

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>    // not sure if this is needed yet
#include <ImportAll_core.hpp>

void testCORF();

double calc_pore2D(unsigned char* arr, int height, int width);

void TP2D(unsigned char *arr,  sYTA *opts, meshInfo *mesh);

void CL2D(unsigned char *arr, options *opts, meshInfo *mesh);



#endif
