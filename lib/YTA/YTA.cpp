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

    // seed rng
    time_t t;
    srand((unsigned ) time(&t));

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

    simInfo.iterCount = 0;
    simInfo.swapCount = 0;
    simInfo.mutCount = 0;

    simInfo.Eswap = simInfo.Ecurrent;

    while (simInfo.iterCount < opts->maxIter && simInfo.Ecurrent > opts->minEnergy)
    {
        if(simInfo.iterCount % 10000 == 0)
        {
            printf("Iter %ld, Swaps %d, Energy %1.3e, pore = %1.3e\n",
                simInfo.iterCount, simInfo.swapCount, simInfo.Ecurrent, calc_pore2D(recData, rec.height, rec.width));
        }
        // update E-current, if applicable
        if (simInfo.Ecurrent > simInfo.Eswap)
        {
            simInfo.Ecurrent = simInfo.Eswap;
        }

        // select two points to swap

        simInfo.tempCoord1[0] = rand() % rec.height;
        simInfo.tempCoord1[1] = rand() % rec.width;

        simInfo.tempCoord2[0] = rand() % rec.height;
        simInfo.tempCoord2[1] = rand() % rec.width;

        // ensure point one is interface

        while (detectInterface2D(recData, &rec, simInfo.tempCoord1[0], simInfo.tempCoord1[1]) != 1)
        {
            // re-draw coordinates until it is an interface
            simInfo.tempCoord1[0] = rand() % rec.height;
            simInfo.tempCoord1[1] = rand() % rec.width;
        }

        // ensure point two is interface and different from point one
        while (detectInterface2D(recData, &rec, simInfo.tempCoord2[0], simInfo.tempCoord2[1]) != 1
                || recData[simInfo.tempCoord1[0] * rec.width + simInfo.tempCoord1[1]] ==  recData[simInfo.tempCoord2[0] * rec.width + simInfo.tempCoord2[1]])
        {
            // re-draw coordinates until it is an interface
            simInfo.tempCoord2[0] = rand() % rec.height;
            simInfo.tempCoord2[1] = rand() % rec.width;
        }

        // set local correlations to 0
        memset(rec.localChord, 0, sizeof(int) * rec.largestSide);
        memset(rec.localChord_b4, 0, sizeof(int) * rec.largestSide);
        memset(simInfo.chordChange, 0, sizeof(int) * rec.largestSide);

        simInfo.totalChordChange = 0;

        memset(rec.localTP, 0, sizeof(int) * opts->structSize);
        memset(rec.localTP_b4, 0, sizeof(int) * opts->structSize);
        memset(simInfo.tpChange, 0, sizeof(int) * opts->structSize);

        // assign a and b

        if(recData[simInfo.tempCoord1[0] * rec.width + simInfo.tempCoord1[1]] == 0)
        {
            simInfo.a[0] = simInfo.tempCoord1[0];
            simInfo.a[1] = simInfo.tempCoord1[1];
            simInfo.b[0] = simInfo.tempCoord2[0];
            simInfo.b[1] = simInfo.tempCoord2[1];
        }else
        {
            simInfo.a[0] = simInfo.tempCoord2[0];
            simInfo.a[1] = simInfo.tempCoord2[1];
            simInfo.b[0] = simInfo.tempCoord1[0];
            simInfo.b[1] = simInfo.tempCoord1[1];
        }

        // calculate local TP at b
        localTP(opts, &rec, recData, simInfo.b[0], simInfo.b[1], 0);

        // calculate local CL at a and b
        localChord(&rec, recData, simInfo.a[0], simInfo.a[1], 0);
        localChord(&rec, recData, simInfo.b[0], simInfo.b[1], 0);

        // make the swap
        if (recData[simInfo.tempCoord1[0] * rec.width + simInfo.tempCoord1[1]] == 0)
        {
            recData[simInfo.tempCoord1[0] * rec.width + simInfo.tempCoord1[1]] = 1;
            recData[simInfo.tempCoord2[0] * rec.width + simInfo.tempCoord2[1]] = 0;
        } else
        {
            recData[simInfo.tempCoord1[0] * rec.width + simInfo.tempCoord1[1]] = 0;
            recData[simInfo.tempCoord2[0] * rec.width + simInfo.tempCoord2[1]] = 1;
        }
        
        // calculate local TP at a
        localTP(opts, &rec, recData, simInfo.a[0], simInfo.a[1], 1);

        // calculate local CL and a and b
        localChord(&rec, recData, simInfo.a[0], simInfo.a[1], 1);
        localChord(&rec, recData, simInfo.b[0], simInfo.b[1], 1);

        // evaluate change in TP

        for (int i = 0; i<opts->structSize; i++){
            simInfo.tpChange[i] = rec.localTP[i] - rec.localTP_b4[i];
        }

        // Get change in chord length as well

        for(int i  = 0; i<rec.largestSide; i++){
            simInfo.chordChange[i] = rec.localChord[i] - rec.localChord_b4[i];
            simInfo.totalChordChange += rec.localChord[i] - rec.localChord_b4[i];
        }

        // calculate E_swap

        simInfo.Eswap = 0;

        // S11 contribution
        for(int i = 0; i < opts->structSize; i++)
        {
            if(target.S11[i] == 0)
                continue;
            simInfo.Eswap += pow(
                ((rec.S11[i] + 2*simInfo.tpChange[i])/rec.TP_Total[i] - target.S11[i]/target.TP_Total[i]), 2);
        }

        // CL contribution
        for(int i = 0; i < target.largestSide; i++)
        {
            simInfo.Eswap += pow(
                ((rec.C00[i]+simInfo.chordChange[i])/(rec.ChordTotal + simInfo.totalChordChange) - target.C00[i]/target.ChordTotal), 2);
        }

        if (simInfo.Eswap - simInfo.Ecurrent > 0)
        {
            // attempt mutation
            // for now just reverse the swap back to original
            if (recData[simInfo.tempCoord1[0] * rec.width + simInfo.tempCoord1[1]] == 0)
            {
                recData[simInfo.tempCoord1[0] * rec.width + simInfo.tempCoord1[1]] = 1;
                recData[simInfo.tempCoord2[0] * rec.width + simInfo.tempCoord2[1]] = 0;
            } else
            {
                recData[simInfo.tempCoord1[0] * rec.width + simInfo.tempCoord1[1]] = 0;
                recData[simInfo.tempCoord2[0] * rec.width + simInfo.tempCoord2[1]] = 1;
            }
        } else
        {
            // keep changes, update correlations
            simInfo.swapCount++;
            for(int i = 0; i<opts->structSize; i++){
                rec.S11[i] += 2*simInfo.tpChange[i];
            }
            for(int i = 0; i<rec.largestSide; i++){
                rec.C00[i] += simInfo.chordChange[i];
            }
            rec.ChordTotal += simInfo.totalChordChange;
            simInfo.Ecurrent = simInfo.Eswap;
        }

        // increase iteration count
        simInfo.iterCount++;
    }

    // if iter != 0 and E_current > E_swap (set e_current = e_swap at i == 0, remove condition)

    // select two points to swap, criteria = both interfaces and different phases

    // memset local correlations to zero
    
    // calculate local correlations

    // swap

    // calculate local correlations again

    // Compare changes, calculate the new energy E_swap

    // Evaluate criteria (ignore, swap, or mutate)

    // print a test image to see what happened 

    for(int i = 0; i < rec.nElements; i++)
    {
        recData[i] = recData[i] * 254;
    }

    writeTarget2D(opts, &rec, recData);

    // memory management

    YTA_baseFree2D(&simInfo, &target, &rec);

    return 0;
}