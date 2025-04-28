#include <YTA.hpp>


int YTA2D(options *opts)
{
    // declare structs
    meshInfo rec;
    meshInfo target;
    sYTA simInfo;

    // get struct-size
    opts->structSize = (opts->blockSize * opts->blockSize) / 2 + 1;

    // read target
    unsigned char *targetData;

    if (readTarget2D(opts, &target, &targetData))
    {
        printf("Error, couldn't read input or nChannels != 1\n");
        return 1;
    }

    // binarize

    for(int i = 0; i < target.nElements; i++)
    {
        if(targetData[i] < opts->TH)
        {
            targetData[i] = 0;
        }
        else
        {
            targetData[i] = 1;
        }
    }

    // get porosity

    target.porosity = calc_pore2D(targetData, target.height, target.width);
    target.VF = 1.0 - target.porosity;

    printf("Target Data Read. Height = %d, Width = %d, Porosity = %1.2lf\n",
        target.height, target.width, target.porosity);

    // get largest side

    rec.height = opts->targetHeight;
    rec.width = opts->targetWidth;

    rec.nElements = rec.height * rec.width;

    target.largestSide = (target.height > target.width) ? target.height : target.width;
    rec.largestSide = (rec.height > rec.width) ? rec.height : rec.width;


    // allocate arrays for reconstruction
    unsigned char *recData = (unsigned char *)malloc(sizeof(char) * rec.nElements);
    memset(recData, 0, sizeof(char) * rec.nElements);

    YTA_baseAlloc2D(opts, &simInfo, &target, &rec);

    // test if it worked

    for(int i = 0; i < opts->structSize; i++)
    {
        if(target.S00[i] != target.S00[i])
        {
            printf("NaN found at %d\n", i);
        }
    }

    // RSPM
    RSPM2D(opts, &rec, recData, target.VF);

    // Correct data
    correctVF2D(&rec, recData, target.VF);

    // calculate initial correlations target
    TP2D(targetData, opts, &target);
    CL2D(targetData, opts, &target);

    // same for rec
    TP2D(recData, opts, &rec);
    CL2D(recData, opts, &rec);

    // get energy

    // 2 point correlation
    simInfo.Ecurrent = 0;

    for (int i = 0; i < opts->structSize; i++)
    {
        target.TP_Total[i] = target.S00[i] + target.S01[i] + target.S11[i];
        rec.TP_Total[i] = rec.S00[i] + rec.S01[i] + rec.S11[i];
        if(target.S11[i] != 0)
        {
            simInfo.Ecurrent += pow((rec.S11[i]/rec.TP_Total[i] - target.S11[i]/target.TP_Total[i]),2);
        }
    }

    // Chord-Length

    // Chord-Length

    target.ChordTotal = 0;
    rec.ChordTotal = 0;

    for(int i = 0; i < target.largestSide; i++)
    {
        rec.ChordTotal += rec.C00[i];
        target.ChordTotal += target.C00[i];
    }

    for (int i = 0; i < target.largestSide; i++)
    {
        simInfo.Ecurrent += pow((target.C00[i]/target.ChordTotal - rec.C00[i]/rec.ChordTotal), 2);
    }

    printf("Current Energy: %1.3e\n", simInfo.Ecurrent);

    /*
    
        Main Loop Below:
    
    */

    // if iter != 0 and E_current > E_swap (set e_current = e_swap at i == 0, remove condition)

    // select two points to swap, criteria = both interfaces and different phases

    // memset local correlations to zero
    
    // calculate local correlations

    // swap

    // calculate local correlations again

    // Compare changes, calculate the new energy E_swap

    // Evaluate criteria (ignore, swap, or mutate)

    // iter++, repeat

    YTA_baseFree2D(&simInfo, &target, &rec);

    return 0;
}