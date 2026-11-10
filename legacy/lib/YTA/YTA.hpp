#ifndef _YTAH
#define _YTAH

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <dataStructures.hpp>
#include <utils.hpp>
#include <time.h>


/*

        Array Allocation and De-Allocation

*/

void YTA_baseFree2D(sYTA *info, meshInfo *target, meshInfo *rec)
{
    /*
        YTA_baseFree2D Function:
        Inputs:
            - pointer to sYTA struct
            - pointer to target (meshInfo) struct
            - pointer to rec (meshInfo) struct
        Outputs:
            - None.
        
        This function is simply a hub for the memory freeing operations for the
        bulk of the arrays whithin the base YTA in 2D.
    */
    // target
    free(target->TP_Total);
    free(target->C00);
    free(target->S00);
    free(target->S01);
    free(target->S11);

    // rec
    free(rec->TP_Total);
    free(rec->C00);
    free(rec->S00);
    free(rec->S01);
    free(rec->S11);

    free(rec->localChord);
    free(rec->localChord_b4);
    free(rec->localTP);
    free(rec->localTP_b4);

    // info
    free(info->chordChange);
    free(info->tpChange);

    return;
}


void YTA_baseAlloc2D(options *opts, sYTA *info, meshInfo *target, meshInfo *rec)
{
    /*
        YTA_baseAlloc2D Function:
        Inputs:
            - pointer to opts struct
            - pointer to sYTA struct
            - pointer to target (meshInfo) struct
            - pointer to rec (meshInfo) struct
        Outputs:
            - None.
        
        This function is simply a hub for the memory allocation operations from the
        YTA-2D code. This significantly de-clutters the main function and makes the 
        allocation - de-allocation route more simple to manage from the programming
        side.

    */

    // gotta allocate all arrays in target

    target->S00 = (double *)malloc(sizeof(double) * opts->structSize);
    target->S11 = (double *)malloc(sizeof(double) * opts->structSize);
    target->S01 = (double *)malloc(sizeof(double) * opts->structSize);

    target->C00 = (double *)malloc(sizeof(double) * rec->largestSide);

    target->TP_Total = (int *)malloc(sizeof(int) * opts->structSize);


    // memset target

    memset(target->S00, 0, sizeof(double) * opts->structSize);
    memset(target->S11, 0, sizeof(double) * opts->structSize);
    memset(target->S01, 0, sizeof(double) * opts->structSize);

    memset(target->C00, 0, sizeof(double) * rec->largestSide);

    memset(target->TP_Total, 0, sizeof(int) * opts->structSize);

    // malloc in rec

    rec->S00 = (double *)malloc(sizeof(double) * opts->structSize);
    rec->S01 = (double *)malloc(sizeof(double) * opts->structSize);
    rec->S11 = (double *)malloc(sizeof(double) * opts->structSize);

    rec->C00 = (double *)malloc(sizeof(double) * rec->largestSide);

    rec->TP_Total = (int *)malloc(sizeof(int) * opts->structSize);

    // memset in rec

    memset(rec->S00, 0, sizeof(double) * opts->structSize);
    memset(rec->S11, 0, sizeof(double) * opts->structSize);
    memset(rec->S01, 0, sizeof(double) * opts->structSize);

    memset(rec->C00, 0, sizeof(double) * rec->largestSide);

    memset(rec->TP_Total, 0, sizeof(int) * opts->structSize);

    // rec and info needs correlation function changes

    rec->localTP    = (int *)malloc(sizeof(int) * opts->structSize);
    rec->localTP_b4 = (int *)malloc(sizeof(int) * opts->structSize);
    info->tpChange  = (int *)malloc(sizeof(int) * opts->structSize);

    rec->localChord     = (int *)malloc(sizeof(int) * rec->largestSide);
    rec->localChord_b4  = (int *)malloc(sizeof(int) * rec->largestSide);
    info->chordChange   = (int *)malloc(sizeof(int) * rec->largestSide);

    // set those to 0 as well

    memset(rec->localTP,    0, sizeof(int) * opts->structSize);
    memset(rec->localTP_b4, 0, sizeof(int) * opts->structSize);
    memset(info->tpChange,  0, sizeof(int) * opts->structSize);
    
    memset(rec->localChord,     0, sizeof(int) * rec->largestSide);
    memset(rec->localChord_b4,  0, sizeof(int) * rec->largestSide);
    memset(info->chordChange,   0, sizeof(int) * rec->largestSide);

    return;
}

/*

    Local Correlations

*/

void localChord(meshInfo *rec, unsigned char *recData, int row, int col, int swapFlag)
{
    /*
        Function localChord
        Inputs:
            - pointer to mesh struct
            - pointer to reconstruction data
            - row and col of point of interest
            - swapFlag
        Outputs:
            - None
        
        Function will calculate the local chord-length functions that are
        affected by the point (row, col). The chord-length values are stored
        in the appropriate array according to swapFlag (0 means b4, 1 after).
    */

    for(int i = 0; i < rec->height; i++)
    {
        if(recData[i * rec->width + col] == 1)
            continue;
        for(int n = 0; n < rec->height; n++)
        {
            if(i + n + 1 == rec->height)
            {
                if(swapFlag == 0)
                    rec->localChord_b4[n+1] += 1;
                else
                    rec->localChord[n+1] +=1;
                break;
            }

            if(recData[(i + n)*rec->width + col] == 1)
            {
                if(swapFlag == 0)
                    rec->localChord_b4[n + 1] += 1;
                else
                    rec->localChord[n + 1] += 1;
                i = i + n + 1;
                break;
            }
        }
    }

    for(int j = 0; j < rec->width; j++)
    {
        if(recData[row * rec->width + j] == 1)
            continue;
        for(int n = 0; n < rec->width; n++)
        {
            if (j + n + 1 == rec->width)
            {
                if(swapFlag == 0)
                    rec->localChord_b4[n+1] += 1;
                else
                    rec->localChord[n+1] += 1;
                
                break;
            }

            if(recData[row * rec->width + j + n] == 1)
            {
                if(swapFlag == 0)
                    rec->localChord_b4[n + 1] += 1;
                else
                    rec->localChord[n + 1] += 1;
                j = j + n + 1;
                break;
            }
        }
    }
    return;
}

void localTP(options *opts, meshInfo *rec, unsigned char *recData, int row, int col, int swapFlag)
{
    /*
        Function localTP:
        Input:
            - pointer to options struct
            - pointer to mesh struct
            - pointer to reconstruction data
            - row and col of interest
            - swapFlag
        Output:
            - none.
        
        Function will calculate the local two-point correlation at row and col.
        If the flag is 0, store at rec.tp_b4, if not store at rec.local_tp
    */

    for(int i = row - opts->blockSize/2; i <= row + opts->blockSize/2; i++)
    {
        for(int j = col - opts->blockSize/2; j <= col + opts->blockSize/2; j++)
        {
            int tempCol = j;
            int tempRow = i;

            // periodic boundary in j
            if (j < 0)
            {
                tempCol = rec->width + j;
            }
            else if (j > rec->width - 1)
            {
                tempCol = j - rec->width;
            }

            // periodic boundary in i
            if (i < 0)
            {
                tempRow = rec->height + i;
            }
            else if (i > rec->height - 1)
            {
                tempRow = i - rec->height;
            }
            int distance = (int)pow((i - row), 2) + (int)pow((j - col), 2);
            if(recData[row * rec->width + col] == recData[tempRow * rec->width + tempCol])
            {
                if (swapFlag == 0)
                    rec->localTP_b4[distance] += 1;
                else
                    rec->localTP[distance] += 1;
            }
        }
    }

    return;
}

#endif