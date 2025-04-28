#ifndef _UTILS
#define _UTILS

#include <dataStructures.hpp>
#include <calcFunctions.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <time.h>

/*

    Initialization

*/

void correctVF2D(meshInfo *mesh, unsigned char *target_arr, double targetVF)
{
    /*
        Function correctVF:
        Inputs: 
            - pointer to mesh
            - poiter to the target array
            - target VF
        Output:
            - none
        
        Function will correct the VF by adding random pixels.
    */

    double VF = 1.0 - calc_pore2D(target_arr, mesh->height, mesh->width);

    if (VF > targetVF)
    {
        while (VF > targetVF)
        {
            int center_i = rand() % (mesh->height);
            int center_j = rand() % (mesh->width);

            if(target_arr[center_i * mesh->width + center_j] == 1)
            {
                target_arr[center_i * mesh->width + center_j] = 0;
                VF -= 1.0 /mesh->nElements;
            }
        }
    } else if(VF < targetVF)
    {
        while (VF < targetVF)
        {
            int center_i = rand() % (mesh->height);
            int center_j = rand() % (mesh->width);

            if(target_arr[center_i * mesh->width + center_j] == 0)
            {
                target_arr[center_i * mesh->width + center_j] = 1;
                VF += 1.0 /mesh->nElements;
            }
        }
    }


    return; 
}

void randomNoise2D(meshInfo *mesh, unsigned char *target_arr, double targetVF)
{
    /*
        Function randomNoise:
        Inputs: 
            - pointer to mesh
            - poiter to the target array
            - target VF
        Output:
            - none
        
        Function will pack the domain using random noise to begin with.
    */

    // get the current volume fraction

    double VF = 1.0 - calc_pore2D(target_arr, mesh->width, mesh->height);

    // seed random number generator
    time_t t;

    srand((unsigned ) time(&t));

    while (VF < targetVF)
    {
        int center_i = rand() % (mesh->height);
        int center_j = rand() % (mesh->width);

        if(target_arr[center_i*mesh->width + center_j] == 0)
        {
            target_arr[center_i * mesh->width + center_j] = 1;
            VF += 1.0 / mesh->nElements;
        }
    }

    return;
}

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

        for (int row = center_i - radius; row <= center_i + radius; row++)
        {
            for(int col = center_j - radius; col <=center_j + radius; col++)
            {
                int tempCol = col;
                int tempRow = row;

                // check wrapping boundaries
                if (row < 0)
                {
                    tempRow = mesh->height + row;
                }
                else if(row >= mesh->height)
                {
                    tempRow = row - mesh->height;
                }

                if (col < 0)
                {
                    tempCol = mesh->width + col;
                }
                else if(col >= mesh->width)
                {
                    tempCol = col - mesh->width;
                }

                if (pow(row - center_i,2) + pow(col - center_j,2) < pow(radius, 2))
                {
                    target_arr[tempRow*mesh->width + tempCol] = 1;
                }
            }
        }
        VF = 1.0 - calc_pore2D(target_arr, mesh->height, mesh->width);
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

    mesh->nElements = mesh->width * mesh->height;

    if(nChannels != 1)
        return 1;

    return 0;
}

/*

    Other:

*/

int detectInterface2D(unsigned char *recData, meshInfo *rec, int row, int col)
{
    /*
        detectInterface Function:
        Inputs:
            - pointer to reconstruction array
            - pointer to it's data structure
            - row and col values for the point to check
        Output:
            - 0 means not interface, 1 means interface

        Function will check for the four cardinal points for a face interface
        (no vertex interfaces), and return 1 if interface, 0 if not. 
    */

    unsigned char currentPhase = recData[row*rec->width + col];

    // check other interfaces

    int tempRow, tempCol;
    int S, N, E, W;

    if(col == 0)
    {
        E = col + 1;
        W = rec->width - 1;
    } else if(col == rec->width - 1)
    {
        W = col - 1;
        E = 0;
    }
    else
    {
        W = col - 1;
        E = col + 1; 
    }

    if(recData[row * rec->width + W] != currentPhase || recData[row * rec->width + E] != currentPhase)
        return 1;

    if (row == 0)
    {
        S = row + 1;
        N = rec->height - 1;
    } else if (row == rec->height - 1)
    {
        S = 0;
        N = row - 1;
    }
    else
    {
        S = row + 1;
        N = row + 1;
    }

    if(recData[S * rec->width + col] != currentPhase || recData[N * rec->width + col] != currentPhase)
        return 1;

    return 0;
}

#endif