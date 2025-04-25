#ifndef _UTILS
#define _UTILS

#include <dataStructures.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


/*

    Calculating Image Properties

*/

double calc_pore2D(unsigned char* arr, int height, int width)
{
    /*
        Function calc_pore2D:
        Inputs:
            - pointer to structure array
            - height value (number of pixels)
            - width value (pixels)
        Outputs:
            - returns the porosity
    */

    double porosity = 0;


    return porosity;
}

/*

    Initialization

*/

/*

    Read and Write Images

*/

void writeTarget2D(options *opts, meshInfo *mesh, unsigned char *target_array)
{
    /*
        Function writeTarget2D:
        Inputs:
            - pointer to options struct
            - pointer to mesh strutc
            - pointer to target array
        Outputs:
            - none.
        
        Function will save the image using stb_image_write
    */

    int nChannels = 1;

    stbi_write_jpg(opts->outputName, mesh->width, mesh->height, nChannels, target_array, 100);

    return;
}

int readTarget2D(options *opts, meshInfo *mesh, unsigned char **target_array)
{
    /*
        Function readTarget2D:
        Inputs:
            - pointer to options struct
            - pointer to mesh struct
            - pointer to the target array to save data
        Outputs:
            - none.
        
        Function will read the target structure from opts, store mesh information
        in mesh, and the figure in target_array.

        Function returns error if nChannels != 1. (only grayscale 8-bit)
    
    */

    int nChannels;

    *target_array = stbi_load(opts->inputName, &mesh->width, &mesh->height, &nChannels, 1);

    if(nChannels != 1)
        return 1;

    return 0;
}

#endif