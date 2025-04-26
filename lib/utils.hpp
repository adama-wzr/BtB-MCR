#ifndef _UTILS
#define _UTILS

#include <dataStructures.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <time.h>

/*

    Initialization

*/

void RSPM2D(options *opts, meshInfo *mesh, unsigned char *target_arr, double targetVF)
{
    /*
        Function RSPM2D:
        Inputs: 
            - pointer to options
            - pointer to mesh
            - poiter to the target array
            - target VF
        Output:
            - none
        
        Function will perform the operation for random-sphere packing
        on the selected domain, while targeting a certain VF.
    */

    double VF = 0;

    // seed random with time
    time_t t;

    srand((unsigned ) time(&t));

    // main loop

    while(VF < targetVF)
    {
        // randomly draw circle center and radius
        int radius = rand() % (opts->RSPM_R);
        int center_i = rand() % (int)(mesh->height);
        int center_j = rand() % (int)(mesh->width);
        
        // Draw the circle
    }


    return;
}

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