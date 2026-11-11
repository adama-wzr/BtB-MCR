#include <utils.hpp>


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

void correctVF3D(meshInfo *mesh, unsigned char *target_arr, double targetVF)
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

    double VF = 1.0 - calc_pore3D(target_arr, mesh->depth, mesh->height, mesh->width);

    if (VF > targetVF)
    {
        while (VF > targetVF)
        {
            int center_i = rand() % (mesh->height);
            int center_j = rand() % (mesh->width);
            int center_k = rand() % (mesh->depth);
            if(target_arr[center_k*mesh->width*mesh->height + center_i * mesh->width + center_j] == 1)
            {
                target_arr[center_k*mesh->width*mesh->height + center_i * mesh->width + center_j] = 0;
                VF -= 1.0 /mesh->nElements;
            }
        }
    } else if(VF < targetVF)
    {
        while (VF < targetVF)
        {
            int center_i = rand() % (mesh->height);
            int center_j = rand() % (mesh->width);
            int center_k = rand() % (mesh->depth);

            if(target_arr[center_k*mesh->width*mesh->height + center_i * mesh->width + center_j] == 0)
            {
                target_arr[center_k*mesh->width*mesh->height + center_i * mesh->width + center_j] = 1;
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

void RSPM2D(unsigned int RSPM_R, meshInfo *mesh, unsigned char *target_arr, double targetVF)
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
        int radius = rand() % (RSPM_R);
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


void RSPM3D(unsigned int RSPM_R, meshInfo *mesh, unsigned char *target_arr, double targetVF)
{
    /*
        Function RSPM3D:
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

        int radius = rand() % (RSPM_R);
        int center_i = rand() % (int)(mesh->height);
        int center_j = rand() % (int)(mesh->width);
        int center_k = rand() % (int)(mesh->depth);

        // draw the circle
        for(int slice = center_k - radius; slice <= center_k + radius; slice++)
        {
            for(int row = center_i - radius; row <= center_i + radius; row++)
            {
                for(int col = center_j - radius; col <= center_j + radius; col++)
                {
                    int tempRow = row;
                    int tempCol = col;
                    int tempSlice = slice;

                    // check periodic boundaries
                    if(row < 0)
                    {
                        tempRow = mesh->height + row;
                    }
                    else if(row > mesh->height - 1)
                    {
                        tempRow = row - mesh->height;
                    }

                    if(col < 0)
                    {
                        tempCol = mesh->width + col;
                    }
                    else if(col > mesh->width - 1)
                    {
                        tempCol = col - mesh->width;
                    }

                    if(slice < 0)
                    {
                        tempSlice = mesh->depth + slice;
                    }
                    else if(slice > mesh->depth - 1)
                    {
                        tempSlice = slice - mesh->depth;
                    }

                    if(pow(slice - center_k,2) + pow(col-center_j,2) + pow(row-center_i,2) < pow(radius,2))
                    {
                        target_arr[tempSlice*mesh->width*mesh->height + tempRow*mesh->width + tempCol] = 1;
                    }
                }
            } // end circle

            VF = 1.0 - calc_pore3D(target_arr, mesh->depth, mesh->height, mesh->width);
        } // end while
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

    if(nChannels == 1)
        return 0;
    else
        return 1;
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
        return 0;

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
        N = row - 1;
    }

    if(recData[S * rec->width + col] != currentPhase || recData[N * rec->width + col] != currentPhase)
        return 0;

    return 1;
}



int detectInterface3D(unsigned char *recData, meshInfo *rec, int slice, int row, int col){


    unsigned char currentPhase = recData[slice*rec->width*rec->height + row*rec->width + col];

    // check other interfaces

    int tempSlice, tempRow, tempCol;
    int S, N, E, W, F, B;

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

    if(recData[slice * rec->width * rec->height + row * rec->width + W] != currentPhase || 
            recData[slice * rec->width * rec->height+ row * rec->width + E] != currentPhase)
        return 0;

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
        N = row - 1;
    }

    if(recData[slice * rec->width * rec->height + S * rec->width + col] != currentPhase || 
            recData[slice * rec->width * rec->height + N * rec->width + col] != currentPhase)
        return 0;

    if(slice == 0)
    {
        F = rec->depth - 1;
        B = slice + 1;
    }
    else if(slice == rec->depth - 1)
    {
        F = slice - 1;
        B = 0;
    }
    else
    {
        F = slice - 1;
        B = slice + 1;
    }

    if(recData[F*rec->width*rec->height + row*rec->width + col] != currentPhase || 
            recData[B*rec->width*rec->height + row*rec->width + col] != currentPhase)
        return 0;

    return 1;
}

