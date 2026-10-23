#ifndef _UTILS
#define _UTILS

#include <ImportAll_core.hpp>
#include <correlationFunctions.hpp>

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <time.h>

/*

    Initialization

*/

void correctVF2D(meshInfo *mesh, unsigned char *target_arr, double targetVF);

void randomNoise2D(meshInfo *mesh, unsigned char *target_arr, double targetVF);

void RSPM2D(options *opts, meshInfo *mesh, unsigned char *target_arr, double targetVF);
/*

    Read and Write Images

*/

void writeTarget2D(options *opts, meshInfo *mesh, unsigned char *target_array);

int readTarget2D(options *opts, meshInfo *mesh, unsigned char **target_array);

/*

    Other:

*/

int detectInterface2D(unsigned char *recData, meshInfo *rec, int row, int col);

#endif
