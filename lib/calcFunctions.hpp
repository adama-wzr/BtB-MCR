#ifndef _CORR
#define _CORR

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>    // not sure if this is needed yet
#include <dataStructures.hpp>


/*

    Volume Fraction Functions:

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
        
        Function calculates porosity of binary image, i.e. 0 = pore and
        1 = particle by default.
    */

    double porosity = 0;
    long int count = 0;

    for(int i = 0; i < height*width; i++)
    {
        if(arr[i] == 0)
            count++; 
    }

    return porosity = (double) count / (height*width);
}

/*

    Correlation Functions:

*/

void TP2D(unsigned char *arr, options *opts, meshInfo *mesh)
{
    /*
        Function TP2D:
        Inputs:
            - pointer to target binary array
            - pointer to options
            - pointer to mesh
        Outputs:
            - None.
        
        Function will calculate the two-point correlation function for the input
        target array and store it in the appropriate places under mesh.
    */


    int blk = opts->blockSize;

    for(int index = 0; index < mesh->nElements; index++)
    {
        int row = index / mesh->width;
        int col = index - row*mesh->width;
        for(int u = col - blk/2; u <= col + blk/2; u++)
        {
            for(int v = row - blk/2; v <= row + blk/2; v++)
            {
                int tempCol = u;
                int tempRow = v;

                if (u < 0)
                {
                    tempCol = mesh->width + u;
                }
                else if(u > (mesh->width - 1))
                {
                    tempCol = u - mesh->width;
                }

                if (v < 0)
                {
                    tempRow = mesh->height + v;
                }
                else if(v > (mesh->height - 1))
                {
                    tempRow = v - mesh->height;
                }

                int distance = (int) pow((row - v), 2) + (int) pow((col - u), 2);

                if (arr[index] == 1 && arr[tempRow*mesh->width + tempCol] == 1)
                {
                    mesh->S11[distance]++;
                }
                else if(arr[index] == 0 && arr[tempRow*mesh->width + tempCol] == 0)
                {
                    mesh->S00[distance]++;
                }
                else
                {
                    mesh->S01[distance]++;
                }
            }
        }
    }

    return;
}


void CL2D(unsigned char *arr, options *opts, meshInfo *mesh)
{
    /*
        Function CL2D:
        Inputs:
            - pointer to target array arr
            - pointer to opts struct
            - pointer to mesh struct
            - integer POI (0 or 1)
        Outputs:
            - none
        
        The function will calculate the CL function in 2D for the probability
        of a chord of length L to lay entirely in pore space if randomly dropped 
        on the image. Periodic boundaries assumed, and scan only x and y
        directions (no diagonal chords).
    */

    // x-direction
    for(int row = 0; row < mesh->height; row++)
    {
        for(int col = 0; col < mesh->width; col++)
        {
            int i = row * mesh->width + col;

            if (arr[i] == 1)
                continue;
            
            // arr[i] = 0 for sure
            for(int n = 1; n < mesh->width - col; n++)
            {
                if(col + n + 1 == mesh->width)
                {
                    mesh->C00[n+1] += 1;
                    break;
                }
                if (arr[i + n] == 1)
                {
                    mesh->C00[n + 1] += 1;
                    col = col + n + 1;
                    break;
                }
            }
        }
    }

    // y-direction
    for(int col = 0; col < mesh->width; col++)
    {
        for(int row = 0; row < mesh->height; row++)
        {
            int i = row * mesh->width + col;

            if (arr[i] == 1)
                continue;

            // arr[i] = 0 for sure
            for(int n = 1; n < mesh->height - row; n++)
            {
                if(row + n + 1 == mesh->height)
                {
                    mesh->C00[n+1] += 1;
                    break;
                }

                if (arr[(row + n)*mesh->width + col] == 1)
                {
                    mesh->C00[n + 1] +=1;
                    row = row + n + 1; 
                }
            }
        }
    }

    return;
}

/*

    Physical Descriptors:

*/

// such empty, wow...

#endif