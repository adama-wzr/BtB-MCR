
#include <correlationFunctions.hpp>


void testCORF()
{
    printf("Reached CORF!\n");

    return;
}



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
            count+=1;
    }

    porosity = (double) count/ (double) (height*width);

    return porosity;
}

/*

    Correlation Functions:

*/

void TP2D(unsigned char *arr, sYTA *opts, meshInfo *mesh)
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


    int blk = opts->blk_size;

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
    int newIdx;
    bool pFlag;
    // x-direction
    for(int row = 0; row < mesh->height; row++)
    {
        pFlag = false;
        for(int col = 0; col < mesh->width; col++)
        {
            int i = row * mesh->width + col;

            if (arr[i] == 1)
            {
                pFlag = true;
                continue;
            }

            if(pFlag == false)
            {
                continue;
            }

            // arr[i] = 0 for sure
            for(int n = 0; n < mesh->width; n++)
            {
                if( n+col >= mesh->width)
                {
                    newIdx = row*mesh->width + (n+col-mesh->width);
                }else
                {
                    newIdx = i + n;
                }

                if(n + 1 == mesh->width)
                {
                    mesh->C00[n+1] += 1;
                    break;
                }
                if (arr[newIdx] == 1)
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
        pFlag = false;
        for(int row = 0; row < mesh->height; row++)
        {
            int i = row * mesh->width + col;

            if (arr[i] == 1)
            {
                pFlag = true;
                continue;
            }
            if (pFlag == false)
                continue;

            // arr[i] = 0 for sure
            for(int n = 0; n < mesh->height; n++)
            {
                if (row + n >= mesh->height)
                {
                    newIdx = (row + n - mesh->height)*mesh->width + col;
                }
                else
                {
                    newIdx = (row + n)*mesh->height + col;
                }

                if(row + n + 1 == mesh->height)
                {
                    mesh->C00[n+1] += 1;
                    break;
                }

                if (arr[newIdx] == 1)
                {
                    mesh->C00[n + 1] +=1;
                    row = row + n + 1; 
                    break;
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
