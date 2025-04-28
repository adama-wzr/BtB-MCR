#ifndef _YTAH
#define _YTAH

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <dataStructures.hpp>
#include <utils.hpp>


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

#endif