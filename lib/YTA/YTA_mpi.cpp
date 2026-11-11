
#include <YTA.hpp>


/*
 *
 *  Initializers and Free's
 *
 *
 * */


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


void YTA_baseAlloc2D(sYTA *info, meshInfo *target, meshInfo *rec)
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

    target->S00 = (double *)malloc(sizeof(double) * info->structSize);
    target->S11 = (double *)malloc(sizeof(double) * info->structSize);
    target->S01 = (double *)malloc(sizeof(double) * info->structSize);

    target->C00 = (double *)malloc(sizeof(double) * rec->largestSide);

    target->TP_Total = (int *)malloc(sizeof(int) * info->structSize);


    // memset target

    memset(target->S00, 0, sizeof(double) * info->structSize);
    memset(target->S11, 0, sizeof(double) * info->structSize);
    memset(target->S01, 0, sizeof(double) * info->structSize);

    memset(target->C00, 0, sizeof(double) * rec->largestSide);

    memset(target->TP_Total, 0, sizeof(int) * info->structSize);

    // malloc in rec

    rec->S00 = (double *)malloc(sizeof(double) * info->structSize);
    rec->S01 = (double *)malloc(sizeof(double) * info->structSize);
    rec->S11 = (double *)malloc(sizeof(double) * info->structSize);

    rec->C00 = (double *)malloc(sizeof(double) * rec->largestSide);

    rec->TP_Total = (int *)malloc(sizeof(int) * info->structSize);

    // memset in rec

    memset(rec->S00, 0, sizeof(double) * info->structSize);
    memset(rec->S11, 0, sizeof(double) * info->structSize);
    memset(rec->S01, 0, sizeof(double) * info->structSize);

    memset(rec->C00, 0, sizeof(double) * rec->largestSide);

    memset(rec->TP_Total, 0, sizeof(int) * info->structSize);

    // rec and info needs correlation function changes

    rec->localTP    = (int *)malloc(sizeof(int) * info->structSize);
    rec->localTP_b4 = (int *)malloc(sizeof(int) * info->structSize);
    info->tpChange  = (int *)malloc(sizeof(int) * info->structSize);

    rec->localChord     = (int *)malloc(sizeof(int) * rec->largestSide);
    rec->localChord_b4  = (int *)malloc(sizeof(int) * rec->largestSide);
    info->chordChange   = (int *)malloc(sizeof(int) * rec->largestSide);

    // set those to 0 as well
    
    memset(rec->localTP,    0, sizeof(int) * info->structSize);
    memset(rec->localTP_b4, 0, sizeof(int) * info->structSize);
    memset(info->tpChange,  0, sizeof(int) * info->structSize);

    memset(rec->localChord,     0, sizeof(int) * rec->largestSide);
    memset(rec->localChord_b4,  0, sizeof(int) * rec->largestSide);
    memset(info->chordChange,   0, sizeof(int) * rec->largestSide);

    // Init the total counts
    target->ChordTotal = 0;
    rec->ChordTotal = 0;
    info->Ecurrent = 0;
    info->Eswap = 0;
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
    int newIdx;
    bool pFlag = false;
    for(int i = 0; i < rec->height; i++)
    {
        if(recData[i * rec->width + col] == 1)
        {
            pFlag = true;
            continue;
        }
        if(pFlag == false)
            continue;
        // now it's 0 for sure
        for(int n = 1; n < rec->height; n++)
        {
            if(i+n >= rec->height)
            {
                newIdx = (i+n - rec->height)*rec->width + col;
            }
            else
            {
                newIdx = (i+n)*rec->width + col;
            }
            if(n + 1 == rec->height)
            {
                if(swapFlag == 0)
                    rec->localChord_b4[n] += 1;
                else
                    rec->localChord[n] +=1;
                break;
            }

            if(recData[newIdx] == 1)
            {
                if(swapFlag == 0)
                    rec->localChord_b4[n] += 1;
                else
                    rec->localChord[n] += 1;
                i = i + n + 1;
                break;
            }
        }
    }

    pFlag = false;

    for(int j = 0; j < rec->width; j++)
    {
        if(recData[row * rec->width + j] == 1)
        {
            pFlag = true;
            continue;
        }
        if(pFlag == false)
            continue;
        // now it's zero for sure
        for(int n = 1; n < rec->width; n++)
        {
            if(j+n >= rec->width)
            {
                newIdx = row*rec->width + j + n - rec->width;
            }
            else
            {
                newIdx = row*rec->width + j + n;
            }

            if (n + 1 == rec->width)
            {
                if(swapFlag == 0)
                    rec->localChord_b4[n] += 1;
                else
                    rec->localChord[n] += 1;
                break;
            }

            if(recData[newIdx] == 1)
            {
                if(swapFlag == 0)
                    rec->localChord_b4[n] += 1;
                else
                    rec->localChord[n] += 1;
                j = j + n + 1;
                break;
            }
        }
    }
    return;
}

void localTP(sYTA *opts, meshInfo *rec, unsigned char *recData, int row, int col, int swapFlag)
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

    for(int i = row - opts->blk_size/2; i <= row + opts->blk_size/2; i++)
    {
        for(int j = col - opts->blk_size/2; j <= col + opts->blk_size/2; j++)
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

/*
 *
 *
 *  Actual YTA implementations
 *
 *
 * */

int YTA_2D(options *opts, MPI_Comm comm)
{
    int rank, nProc;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &nProc);

    meshInfo rec;
    meshInfo target;
    sYTA simInfo;

    if (parseInputYTA(&simInfo) == 1)
    {
        return 1;
    }

    if(opts->verbose && rank == 0)
        printOptsYTA(&simInfo);

    simInfo.T0 = 0;

    clock_t begin = clock();
    double t1, t2;
    long int iterCount = 0;
    t1 = MPI_Wtime();
    // Allocated necessary arrays

    rec.height = opts->targetHeight;
    rec.width = opts->targetWidth;

    rec.nElements = rec.height * rec.width;

    rec.largestSide = (rec.height > rec.width) ? rec.height : rec.width;

    // allocate arrays for reconstruction
    unsigned char *recData = (unsigned char *)malloc(sizeof(unsigned char) * rec.nElements);
    memset(recData, 0, sizeof(unsigned char) * rec.nElements);

    YTA_baseAlloc2D(&simInfo, &target, &rec);

    int *change_coordinates, *all_coordinates;

    int* swap_flag;

    change_coordinates = (int *)malloc(sizeof(int)*simInfo.localIter*4);

    if (rank == 0)
    {
        all_coordinates = (int *)malloc(sizeof(int)*simInfo.localIter*4*nProc);
        swap_flag = (int *)malloc(sizeof(int)*rec.height*rec.width);
    }

    time_t t;

    srand((unsigned ) (rank+1)*time(&t));

    // root section of the code

    if(rank == 0)
    {
        // Step1: Read target 
        unsigned char *targetData;
        if (readTarget2D(opts, &target, &targetData))
        {
            printf("Error, couldn't read input target\n");
            MPI_Barrier(comm);
            return 0;
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
        target.largestSide = (target.height > target.width) ? target.height : target.width;

        if(opts->verbose)
        {
            printf("Target Data Read. Height = %d, Width = %d, Porosity = %1.3e\n", target.height, target.width, target.porosity);
        }

        // Calculate correlation functions of the target
        TP2D(targetData, &simInfo, &target);
        CL2D(targetData, opts, &target);

        // Pass target information to other cores
        MPI_Bcast(target.S11, simInfo.structSize, MPI_DOUBLE, 0, comm);
        MPI_Bcast(target.C00, rec.largestSide, MPI_DOUBLE, 0, comm);

        // RSPM
        RSPM2D(simInfo.RSPM_R, &rec, recData, target.VF);

        // Correct data
        correctVF2D(&rec, recData, target.VF);

        // corr. func for rec
        TP2D(recData, &simInfo, &rec);
        CL2D(recData, opts, &rec);

        rec.porosity = calc_pore2D(recData, rec.height, rec.width);
        rec.VF = 1.0 - rec.porosity;

        // get energy
        simInfo.Ecurrent = 0.0;
        for(int i = 0; i < simInfo.structSize; i++)
        {
            target.TP_Total[i] = target.S11[i] + target.S01[i] + target.S00[i];
            rec.TP_Total[i] = rec.S11[i] + rec.S01[i] + rec.S00[i];
            if(target.TP_Total[i] != 0)
            {
                simInfo.Ecurrent += pow((rec.S11[i]/rec.TP_Total[i] - target.S11[i]/target.TP_Total[i]),2);
            }
        }

        target.ChordTotal = 0;
        rec.ChordTotal = 0;

        for(int i = 0; i < rec.largestSide; i++)
        {
            target.ChordTotal += target.C00[i];
            rec.ChordTotal += rec.C00[i];
        }

        for (int i = 0; i < rec.largestSide; i++)
        {
            simInfo.Ecurrent += pow((rec.C00[i]/rec.ChordTotal - target.C00[i]/target.ChordTotal),2);
        }
        printf("Calculated Energy = %1.3e\n", simInfo.Ecurrent);

        while(iterCount < simInfo.rootIter && simInfo.Ecurrent > simInfo.MinEnergy)
        {
            if (iterCount != 0 && simInfo.Ecurrent > simInfo.Eswap)
                simInfo.Ecurrent = simInfo.Eswap;

            // Pick coords
            simInfo.tempCoord1[0] = rand() % (int)(rec.height);
            simInfo.tempCoord1[1] = rand() % (int)(rec.width);

            simInfo.tempCoord2[0] = rand() % (int)(rec.height);
            simInfo.tempCoord2[1] = rand() % (int)(rec.width);

            while(detectInterface2D(recData, &rec, simInfo.tempCoord1[0], simInfo.tempCoord1[1]) == 1)
            {
                simInfo.tempCoord1[0] = rand() % (int)(rec.height);
                simInfo.tempCoord1[1] = rand() % (int)(rec.width);
            }

            while(detectInterface2D(recData, &rec, simInfo.tempCoord2[0], simInfo.tempCoord2[1]) == 1)
            {
                simInfo.tempCoord2[0] = rand() % (int)(rec.height);
                simInfo.tempCoord2[1] = rand() % (int)(rec.width);
            }

            // make sure they are different phases
            int flag = 0;
            if (recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] == recData[simInfo.tempCoord2[0]*rec.width + simInfo.tempCoord2[1]])
                flag = 1;

            while (flag == 1)
            {
                if (recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] == recData[simInfo.tempCoord2[0]*rec.width + simInfo.tempCoord2[1]])
                    flag = 1;
                else
                    flag = 0;

                if (flag == 1)
                {
                    simInfo.tempCoord2[0] = rand() % (int)(rec.height);
                    simInfo.tempCoord2[1] = rand() % (int)(rec.width);
                    while(detectInterface2D(recData, &rec, simInfo.tempCoord2[0], simInfo.tempCoord2[1]) == 1)
                    {
                        simInfo.tempCoord2[0] = rand() % (int)(rec.height);
                        simInfo.tempCoord2[1] = rand() % (int)(rec.width);
                    }
                }
            }

            // For a fact different phases and both interfaces
            // Set correlation changes to 0
            memset(rec.localTP, 0, sizeof(int)*simInfo.structSize);
            memset(rec.localTP_b4, 0, sizeof(int)*simInfo.structSize);
            memset(simInfo.tpChange, 0, sizeof(int)*simInfo.structSize);

            memset(rec.localChord, 0, sizeof(int)*rec.largestSide);
            memset(rec.localChord_b4, 0, sizeof(int)*rec.largestSide);
            memset(simInfo.chordChange, 0, sizeof(int)*rec.largestSide);

            simInfo.totalChordChange = 0;

            // make sure a = 0 before swap and b = 1;
            if(recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] == 0)
            {
                simInfo.a[0] = simInfo.tempCoord1[0];
                simInfo.a[1] = simInfo.tempCoord1[1];
                simInfo.b[0] = simInfo.tempCoord2[0];
                simInfo.b[1] = simInfo.tempCoord2[1];
            }
            else
            {
                simInfo.b[0] = simInfo.tempCoord1[0];
                simInfo.b[1] = simInfo.tempCoord1[1];
                simInfo.a[0] = simInfo.tempCoord2[0];
                simInfo.a[1] = simInfo.tempCoord2[1];
            }

            // local costs
            localTP(&simInfo, &rec, recData, simInfo.b[0], simInfo.b[1], 0);

            localChord(&rec, recData, simInfo.b[0], simInfo.b[1], 0);
            localChord(&rec, recData, simInfo.a[0], simInfo.a[1], 0);

            // complete the swap
            recData[simInfo.a[0]*rec.width + simInfo.a[1]] = 1;
            recData[simInfo.b[0]*rec.width + simInfo.b[1]] = 0;

            // evaluate local correlation again
            localTP(&simInfo, &rec, recData, simInfo.a[0], simInfo.a[1], 1);

            localChord(&rec, recData, simInfo.b[0], simInfo.b[1], 1);
            localChord(&rec, recData, simInfo.a[0], simInfo.a[1], 1);

            // get two-point correlation change
            for(int i = 0; i < simInfo.structSize; i++)
            {
                simInfo.tpChange[i] = rec.localTP[i] - rec.localTP_b4[i];
            }


            // get chord-length change
            for (int i = 0; i < rec.largestSide; i++)
            {
                simInfo.chordChange[i] = rec.localChord[i] - rec.localChord_b4[i];
                simInfo.totalChordChange += rec.localChord[i] - rec.localChord_b4[i];
            }

            // calculate Eswap
            simInfo.Eswap = 0;
            for (int i = 0; i < simInfo.structSize; i++)
            {
                if(target.S11[i] != 0)
                {
                    simInfo.Eswap += pow(((rec.S11[i] + 2*simInfo.tpChange[i])/rec.TP_Total[i] - target.S11[i]/target.TP_Total[i]),2);
                }
            }

            for (int i = 0; i<rec.largestSide; i++)
            {
                simInfo.Eswap += pow((rec.C00[i] + simInfo.chordChange[i])/(rec.ChordTotal + simInfo.totalChordChange) - target.C00[i]/target.ChordTotal,2);
            }

            // swap, mutate, or reverse
            if(simInfo.Eswap > simInfo.Ecurrent)
            {   // reverse or mutate
                if(simInfo.T0 == 0)
                {
                    simInfo.T0 = -(simInfo.Eswap - simInfo.Ecurrent)/(simInfo.lambda*log(0.5));
                    printf("Initial T0 = %1.3e, initial energy = %1.3e\n", simInfo.T0, simInfo.Ecurrent);
                }
                simInfo.Tk = simInfo.T0 * pow(simInfo.lambda, simInfo.mutCount);
                double temp1 = (double)rand() / (double)RAND_MAX;

                if(temp1 < exp(-(simInfo.Eswap - simInfo.Ecurrent)/simInfo.Tk))
                {
                    // mutation happens
                    simInfo.mutCount++;
                    simInfo.Ecurrent = simInfo.Eswap;
                    // update correlation functions
                    for (int i = 0; i < simInfo.structSize; i++)
                    {
                        rec.S11[i] += 2*simInfo.tpChange[i];
                    }
                    for(int i = 0; i < rec.largestSide; i++)
                    {
                        rec.C00[i] += simInfo.chordChange[i];
                    }
                    rec.ChordTotal += simInfo.totalChordChange;
                }
                else
                {
                    // no mutation, revert swap
                    if (recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] == 0)
                    {
                        recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] = 1;
                        recData[simInfo.tempCoord2[0]*rec.width + simInfo.tempCoord2[1]] = 0;
                    }
                    else
                    {
                        recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] = 0;
                        recData[simInfo.tempCoord2[0]*rec.width + simInfo.tempCoord2[1]] = 1;
                    }
                }
            }
            else
            {
                // successful swap
                simInfo.swapCount++;
                // update correlations
                simInfo.Ecurrent = simInfo.Eswap;
                // update correlation functions
                for (int i = 0; i < simInfo.structSize; i++)
                {
                    rec.S11[i] += 2*simInfo.tpChange[i];
                }
                for(int i = 0; i < rec.largestSide; i++)
                {
                    rec.C00[i] += simInfo.chordChange[i];
                }
                rec.ChordTotal += simInfo.totalChordChange;
            }

            if(iterCount % 1000000 == 0)
            {
                rec.porosity = calc_pore2D(recData, rec.height, rec.width);
                printf("Iter = %ld, Mut = %d, Swap = %d, Energy = %1.3e\n", iterCount, simInfo.mutCount, simInfo.swapCount, simInfo.Ecurrent);
                fflush(stdout);
            }
            iterCount++;
        }

        printf("Root done with initial loop!\n");

        MPI_Barrier(comm);

        MPI_Bcast(recData, rec.nElements, MPI_UNSIGNED_CHAR, 0, comm);
        MPI_Barrier(comm);
        MPI_Bcast(rec.S11, simInfo.structSize, MPI_DOUBLE, 0, comm);
        MPI_Barrier(comm);
        MPI_Bcast(rec.TP_Total, simInfo.structSize, MPI_INT, 0, comm);
        MPI_Barrier(comm);
        MPI_Bcast(target.TP_Total, simInfo.structSize, MPI_INT, 0, comm);
        MPI_Barrier(comm);
        MPI_Bcast(rec.C00, rec.largestSide, MPI_DOUBLE, 0, comm);
        MPI_Barrier(comm);
        MPI_Bcast(&rec.ChordTotal, 1, MPI_LONG, 0, comm);
        MPI_Barrier(comm);
        MPI_Bcast(&target.ChordTotal, 1, MPI_LONG, 0, comm);
        MPI_Barrier(comm);
        MPI_Bcast(&simInfo.Ecurrent, 1, MPI_DOUBLE, 0, comm);
        MPI_Barrier(comm);

        // communications done, set changes-coords to -1
        for(int i = 0; i < simInfo.localIter; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                change_coordinates[4*i + j] = -1;
            }
        }

        while(true)
        {
            int local_iter = 0;
            int swap_count = 0;

            while(local_iter < simInfo.localIter)
            {
                if(local_iter != 0 && simInfo.Ecurrent > simInfo.Eswap)
                    simInfo.Ecurrent = simInfo.Eswap;

                // repeat the main init loop of the algorithm, whithout mutations this time
                // Pick coords
                simInfo.tempCoord1[0] = rand() % (int)(rec.height);
                simInfo.tempCoord1[1] = rand() % (int)(rec.width);

                simInfo.tempCoord2[0] = rand() % (int)(rec.height);
                simInfo.tempCoord2[1] = rand() % (int)(rec.width);

                while(detectInterface2D(recData, &rec, simInfo.tempCoord1[0], simInfo.tempCoord1[1]) == 1)
                {
                    simInfo.tempCoord1[0] = rand() % (int)(rec.height);
                    simInfo.tempCoord1[1] = rand() % (int)(rec.width);
                }

                while(detectInterface2D(recData, &rec, simInfo.tempCoord2[0], simInfo.tempCoord2[1]) == 1)
                {
                    simInfo.tempCoord2[0] = rand() % (int)(rec.height);
                    simInfo.tempCoord2[1] = rand() % (int)(rec.width);
                }

                // make sure they are different phases
                int flag = 0;
                if (recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] == recData[simInfo.tempCoord2[0]*rec.width + simInfo.tempCoord2[1]])
                    flag = 1;

                while (flag == 1)
                {
                    if (recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] == recData[simInfo.tempCoord2[0]*rec.width + simInfo.tempCoord2[1]])
                        flag = 1;
                    else
                        flag = 0;

                    if (flag == 1)
                    {
                        simInfo.tempCoord2[0] = rand() % (int)(rec.height);
                        simInfo.tempCoord2[1] = rand() % (int)(rec.width);
                        while(detectInterface2D(recData, &rec, simInfo.tempCoord2[0], simInfo.tempCoord2[1]) == 1)
                        {
                            simInfo.tempCoord2[0] = rand() % (int)(rec.height);
                            simInfo.tempCoord2[1] = rand() % (int)(rec.width);
                        }
                    }
                }

                // For a fact different phases and both interfaces
                // Set correlation changes to 0
                memset(rec.localTP, 0, sizeof(int)*simInfo.structSize);
                memset(rec.localTP_b4, 0, sizeof(int)*simInfo.structSize);
                memset(simInfo.tpChange, 0, sizeof(int)*simInfo.structSize);

                memset(rec.localChord, 0, sizeof(int)*rec.largestSide);
                memset(rec.localChord_b4, 0, sizeof(int)*rec.largestSide);
                memset(simInfo.chordChange, 0, sizeof(int)*rec.largestSide);

                simInfo.totalChordChange = 0;

                // make sure a = 0 before swap and b = 1;
                if(recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] == 0)
                {
                    simInfo.a[0] = simInfo.tempCoord1[0];
                    simInfo.a[1] = simInfo.tempCoord1[1];
                    simInfo.b[0] = simInfo.tempCoord2[0];
                    simInfo.b[1] = simInfo.tempCoord2[1];
                }
                else
                {
                    simInfo.b[0] = simInfo.tempCoord1[0];
                    simInfo.b[1] = simInfo.tempCoord1[1];
                    simInfo.a[0] = simInfo.tempCoord2[0];
                    simInfo.a[1] = simInfo.tempCoord2[1];
                }

                // local costs
                localTP(&simInfo, &rec, recData, simInfo.b[0], simInfo.b[1], 0);

                localChord(&rec, recData, simInfo.b[0], simInfo.b[1], 0);
                localChord(&rec, recData, simInfo.a[0], simInfo.a[1], 0);

                // complete the swap
                recData[simInfo.a[0]*rec.width + simInfo.a[1]] = 1;
                recData[simInfo.b[0]*rec.width + simInfo.b[1]] = 0;

                // evaluate local correlation again
                localTP(&simInfo, &rec, recData, simInfo.a[0], simInfo.a[1], 1);

                localChord(&rec, recData, simInfo.b[0], simInfo.b[1], 1);
                localChord(&rec, recData, simInfo.a[0], simInfo.a[1], 1);

                // get two-point correlation change
                for(int i = 0; i < simInfo.structSize; i++)
                {
                    simInfo.tpChange[i] = rec.localTP[i] - rec.localTP_b4[i];
                }


                // get chord-length change
                for (int i = 0; i < rec.largestSide; i++)
                {
                    simInfo.chordChange[i] = rec.localChord[i] - rec.localChord_b4[i];
                    simInfo.totalChordChange += rec.localChord[i] - rec.localChord_b4[i];
                }

                // calculate Eswap
                simInfo.Eswap = 0;
                for (int i = 0; i < simInfo.structSize; i++)
                {
                    if(target.S11[i] != 0)
                    {
                        simInfo.Eswap += pow(((rec.S11[i] + 2*simInfo.tpChange[i])/rec.TP_Total[i] - target.S11[i]/target.TP_Total[i]),2);
                    }
                }

                for (int i = 0; i<rec.largestSide; i++)
                {
                    simInfo.Eswap += pow((rec.C00[i] + simInfo.chordChange[i])/(rec.ChordTotal + simInfo.totalChordChange) - target.C00[i]/target.ChordTotal,2);
                }

                // swap, mutate, or reverse
                if(simInfo.Eswap > simInfo.Ecurrent)
                {
                    // reverse, no mutations allowed
                    if (recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] == 0)
                    {
                        recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] = 1;
                        recData[simInfo.tempCoord2[0]*rec.width + simInfo.tempCoord2[1]] = 0;
                    }
                    else
                    {
                        recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] = 0;
                        recData[simInfo.tempCoord2[0]*rec.width + simInfo.tempCoord2[1]] = 1;
                    }
                }
                else
                {
                    // successful swap
                    simInfo.swapCount++;
                    // update correlations
                    simInfo.Ecurrent = simInfo.Eswap;
                    // update correlation functions
                    for (int i = 0; i < simInfo.structSize; i++)
                    {
                        rec.S11[i] += 2*simInfo.tpChange[i];
                    }
                    for(int i = 0; i < rec.largestSide; i++)
                    {
                        rec.C00[i] += simInfo.chordChange[i];
                    }
                    rec.ChordTotal += simInfo.totalChordChange;

                    change_coordinates[4*swap_count + 0] = simInfo.tempCoord1[0];
                    change_coordinates[4*swap_count + 1] = simInfo.tempCoord1[1];
                    change_coordinates[4*swap_count + 2] = simInfo.tempCoord2[0];
                    change_coordinates[4*swap_count + 3] = simInfo.tempCoord2[1];

                    swap_count++;

                }
                local_iter++;
            }

            printf("rank = %d, Swaps = %d, E_current = %f\n", rank, swap_count, simInfo.Ecurrent);

            MPI_Barrier(comm);

            MPI_Gather(change_coordinates, simInfo.localIter*4, MPI_INT, all_coordinates, simInfo.localIter*4, MPI_INT, 0, comm);

            // evaluate candidates and make changes
            for(int i = simInfo.localIter; i < (simInfo.localIter*(nProc-1)); i++)
            {
                if(all_coordinates[4*i + 0] == -1)
                    continue;

                simInfo.tempCoord1[0] = all_coordinates[4*i + 0];
                simInfo.tempCoord1[1] = all_coordinates[4*i + 1];
                simInfo.tempCoord2[0] = all_coordinates[4*i + 2];
                simInfo.tempCoord2[1] = all_coordinates[4*i + 3];

                if(swap_flag[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] != 0 || swap_flag[simInfo.tempCoord2[0]*rec.width + simInfo.tempCoord2[1]] != 0)
                    continue;
                
                if(recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] == 0 &&
                        recData[simInfo.tempCoord2[0]*rec.width + simInfo.tempCoord2[1]] == 1)
                {
                    simInfo.a[0] = simInfo.tempCoord1[0];
                    simInfo.a[1] = simInfo.tempCoord1[1];
                    simInfo.b[0] = simInfo.tempCoord2[0];
                    simInfo.b[1] = simInfo.tempCoord2[1];
                }else if(recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] == 1 &&
                        recData[simInfo.tempCoord2[0]*rec.width + simInfo.tempCoord2[1]] == 0)
                {
                    simInfo.a[0] = simInfo.tempCoord2[0];
                    simInfo.a[1] = simInfo.tempCoord2[1];
                    simInfo.b[0] = simInfo.tempCoord1[0];
                    simInfo.b[1] = simInfo.tempCoord1[1];
                }
                else
                    continue;

                // zero the local cost functions
                memset(rec.localTP, 0, sizeof(int)*simInfo.structSize);
                memset(rec.localTP_b4, 0, sizeof(int)*simInfo.structSize);
                memset(simInfo.tpChange, 0, sizeof(int)*simInfo.structSize);

                memset(rec.localChord, 0, sizeof(int)*rec.largestSide);
                memset(rec.localChord_b4, 0, sizeof(int)*rec.largestSide);
                memset(simInfo.chordChange, 0, sizeof(int)*rec.largestSide);

                simInfo.totalChordChange = 0;

                // local costs
                localTP(&simInfo, &rec, recData, simInfo.b[0], simInfo.b[1], 0);

                localChord(&rec, recData, simInfo.b[0], simInfo.b[1], 0);
                localChord(&rec, recData, simInfo.a[0], simInfo.a[1], 0);

                // complete the swap
                recData[simInfo.a[0]*rec.width + simInfo.a[1]] = 1;
                recData[simInfo.b[0]*rec.width + simInfo.b[1]] = 0;

                // evaluate local correlation again
                localTP(&simInfo, &rec, recData, simInfo.a[0], simInfo.a[1], 1);

                localChord(&rec, recData, simInfo.b[0], simInfo.b[1], 1);
                localChord(&rec, recData, simInfo.a[0], simInfo.a[1], 1);

                // get two-point correlation change
                for(int i = 0; i < simInfo.structSize; i++)
                {
                    simInfo.tpChange[i] = rec.localTP[i] - rec.localTP_b4[i];
                }


                // get chord-length change
                for (int i = 0; i < rec.largestSide; i++)
                {
                    simInfo.chordChange[i] = rec.localChord[i] - rec.localChord_b4[i];
                    simInfo.totalChordChange += rec.localChord[i] - rec.localChord_b4[i];
                }

                // calculate Eswap
                simInfo.Eswap = 0;
                for (int i = 0; i < simInfo.structSize; i++)
                {
                    if(target.S11[i] != 0)
                    {
                        simInfo.Eswap += pow(((rec.S11[i] + 2*simInfo.tpChange[i])/rec.TP_Total[i] - target.S11[i]/target.TP_Total[i]),2);
                    }
                }

                for (int i = 0; i<rec.largestSide; i++)
                {
                    simInfo.Eswap += pow((rec.C00[i] + simInfo.chordChange[i])/(rec.ChordTotal + simInfo.totalChordChange) - target.C00[i]/target.ChordTotal,2);
                }

                if(simInfo.Eswap > simInfo.Ecurrent)
                {
                    // reverse swap
                    recData[simInfo.a[0]*rec.width + simInfo.a[1]] = 0;
                    recData[simInfo.b[0]*rec.width + simInfo.b[1]] = 1;
                }
                else
                {
                    // update correlations, tick the swap flag
                    simInfo.swapCount++;
                                        // update correlations
                    simInfo.Ecurrent = simInfo.Eswap;
                    // update correlation functions
                    for (int i = 0; i < simInfo.structSize; i++)
                    {
                        rec.S11[i] += 2*simInfo.tpChange[i];
                    }
                    for(int i = 0; i < rec.largestSide; i++)
                    {
                        rec.C00[i] += simInfo.chordChange[i];
                    }
                    rec.ChordTotal += simInfo.totalChordChange;

                    swap_flag[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] = 1;
                    swap_flag[simInfo.tempCoord2[0]*rec.width + simInfo.tempCoord2[1]] = 1;

                    swap_count++; // swap count in this local loop

                }

            }

            iterCount += (nProc)*simInfo.localIter;

            printf("Attempted swaps per core: %ld, Total Iter = %ld, Successful swaps: %d, Energy = %1.3e\n", simInfo.localIter, iterCount, swap_count, simInfo.Ecurrent);


            MPI_Barrier(comm);

            // pass new struc, correlation fnctions, iterCt, Energy, and total_chord
            MPI_Bcast(recData, rec.nElements, MPI_UNSIGNED_CHAR, 0, comm);
            MPI_Barrier(comm);
            MPI_Bcast(rec.S11, simInfo.structSize, MPI_DOUBLE, 0, comm);
            MPI_Barrier(comm);
            MPI_Bcast(rec.C00, rec.largestSide, MPI_DOUBLE, 0, comm);
            MPI_Barrier(comm);
            MPI_Bcast(&rec.ChordTotal, 1, MPI_LONG, 0, comm);
            MPI_Barrier(comm);
            MPI_Bcast(&simInfo.Ecurrent, 1, MPI_DOUBLE, 0, comm);
            MPI_Barrier(comm);
            MPI_Bcast(&iterCount, 1, MPI_LONG, 0, comm);
            MPI_Barrier(comm);

            // check termination criteria

            if(iterCount > simInfo.maxIter)
            {
                printf("Exceeded max. iter. Closing...\n");
                break;
            }
            if(simInfo.Ecurrent < simInfo.MinEnergy)
            {
                printf("Exceeded minimum energy. Closing...\n");
                break;
            }
            // else zero correlation functions and calculate things again

            for (int i = 0; i < simInfo.localIter; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    change_coordinates[i*4 + j] = -1;
                }
                for(int proc = 0; proc < nProc; proc++)
                {
                    for(int j = 0; j < 4; j++)
                    {
                        all_coordinates[i*4 + proc*simInfo.localIter + j] = -1;
                    }
                }
            }

            memset(swap_flag, 0, sizeof(int)*rec.nElements);
        }

        // gc
        // free(targetData);
        // free(all_coordinates);
        // free(swap_flag);

        printf("Job done. Root saving image...\n");

        for(int i = 0; i < rec.height; i++ )
        {
            for(int j = 0; j < rec.width; j++)
            {
                if(recData[i*rec.width + j] == 0)
                {
                    recData[i*rec.width + j] = 0;
                }else{
                    recData[i*rec.width + j] = 254;
                }
            }
        }

        // Out of the root main Loop, let's save the image just in case
        stbi_write_jpg(opts->outputName, (int)rec.width, (int)rec.height, 1, recData, 100);
        printf("Image Saved!\n");
        MPI_Barrier(comm);
        return 0;
    }

    // workers section of the code

    // Get relevant target correlation functions
    MPI_Bcast(target.S11, simInfo.structSize, MPI_DOUBLE, 0, comm);
    MPI_Bcast(target.C00, rec.largestSide, MPI_DOUBLE, 0, comm);

    MPI_Barrier(comm);

    MPI_Bcast(recData, rec.nElements, MPI_UNSIGNED_CHAR, 0, comm);
    MPI_Barrier(comm);
    MPI_Bcast(rec.S11, simInfo.structSize, MPI_DOUBLE, 0, comm);
    MPI_Barrier(comm);
    MPI_Bcast(rec.TP_Total, simInfo.structSize, MPI_INT, 0, comm);
    MPI_Barrier(comm);
    MPI_Bcast(target.TP_Total, simInfo.structSize, MPI_INT, 0, comm);
    MPI_Barrier(comm);
    MPI_Bcast(rec.C00, rec.largestSide, MPI_DOUBLE, 0, comm);
    MPI_Barrier(comm);
    MPI_Bcast(&rec.ChordTotal, 1, MPI_LONG, 0, comm);
    MPI_Barrier(comm);
    MPI_Bcast(&target.ChordTotal, 1, MPI_LONG, 0, comm);
    MPI_Barrier(comm);
    MPI_Bcast(&simInfo.Ecurrent, 1, MPI_DOUBLE, 0, comm);
    MPI_Barrier(comm);

    fprintf(stderr, "Core %d, received all the data!\n", rank);
    fflush(stderr);
    for (int i = 0; i < simInfo.localIter; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            change_coordinates[i*4 + j] = -1;
        }
    }
    printf("Starting Main Loop! Rank: %d\n", rank);
    simInfo.Eswap = 1.0;
    // worker infinite loop
    while(true)
    {
        if(iterCount > simInfo.maxIter)
            break;

        int local_iter = 0;
        int swap_count = 0;

        while(local_iter < simInfo.localIter)
        {
            if(local_iter != 0 && simInfo.Ecurrent > simInfo.Eswap)
                simInfo.Ecurrent = simInfo.Eswap;

            // repeat the main init loop of the algorithm, whithout mutations this time
            // Pick coords
            simInfo.tempCoord1[0] = rand() % (int)(rec.height);
            simInfo.tempCoord1[1] = rand() % (int)(rec.width);

            simInfo.tempCoord2[0] = rand() % (int)(rec.height);
            simInfo.tempCoord2[1] = rand() % (int)(rec.width);

            while(detectInterface2D(recData, &rec, simInfo.tempCoord1[0], simInfo.tempCoord1[1]) == 1)
            {
                simInfo.tempCoord1[0] = rand() % (int)(rec.height);
                simInfo.tempCoord1[1] = rand() % (int)(rec.width);
            }

            while(detectInterface2D(recData, &rec, simInfo.tempCoord2[0], simInfo.tempCoord2[1]) == 1)
            {
                simInfo.tempCoord2[0] = rand() % (int)(rec.height);
                simInfo.tempCoord2[1] = rand() % (int)(rec.width);
            }

            // make sure they are different phases
            int flag = 0;
            if (recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] == recData[simInfo.tempCoord2[0]*rec.width + simInfo.tempCoord2[1]])
                flag = 1;

            while (flag == 1)
            {
                if (recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] == recData[simInfo.tempCoord2[0]*rec.width + simInfo.tempCoord2[1]])
                    flag = 1;
                else
                    flag = 0;

                if (flag == 1)
                {
                    simInfo.tempCoord2[0] = rand() % (int)(rec.height);
                    simInfo.tempCoord2[1] = rand() % (int)(rec.width);
                    while(detectInterface2D(recData, &rec, simInfo.tempCoord2[0], simInfo.tempCoord2[1]) == 1)
                    {
                        simInfo.tempCoord2[0] = rand() % (int)(rec.height);
                        simInfo.tempCoord2[1] = rand() % (int)(rec.width);
                    }
                }
            }

            // For a fact different phases and both interfaces
            // Set correlation changes to 0
            memset(rec.localTP, 0, sizeof(int)*simInfo.structSize);
            memset(rec.localTP_b4, 0, sizeof(int)*simInfo.structSize);
            memset(simInfo.tpChange, 0, sizeof(int)*simInfo.structSize);

            memset(rec.localChord, 0, sizeof(int)*rec.largestSide);
            memset(rec.localChord_b4, 0, sizeof(int)*rec.largestSide);
            memset(simInfo.chordChange, 0, sizeof(int)*rec.largestSide);

            simInfo.totalChordChange = 0;
            // make sure a = 0 before swap and b = 1;
            if(recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] == 0)
            {
                simInfo.a[0] = simInfo.tempCoord1[0];
                simInfo.a[1] = simInfo.tempCoord1[1];
                simInfo.b[0] = simInfo.tempCoord2[0];
                simInfo.b[1] = simInfo.tempCoord2[1];
            }
            else
            {
                simInfo.b[0] = simInfo.tempCoord1[0];
                simInfo.b[1] = simInfo.tempCoord1[1];
                simInfo.a[0] = simInfo.tempCoord2[0];
                simInfo.a[1] = simInfo.tempCoord2[1];
            }
            // local costs
            localTP(&simInfo, &rec, recData, simInfo.b[0], simInfo.b[1], 0);

            localChord(&rec, recData, simInfo.b[0], simInfo.b[1], 0);
            localChord(&rec, recData, simInfo.a[0], simInfo.a[1], 0);

            // complete the swap
            recData[simInfo.a[0]*rec.width + simInfo.a[1]] = 1;
            recData[simInfo.b[0]*rec.width + simInfo.b[1]] = 0;

            // evaluate local correlation again
            localTP(&simInfo, &rec, recData, simInfo.a[0], simInfo.a[1], 1);

            localChord(&rec, recData, simInfo.b[0], simInfo.b[1], 1);
            localChord(&rec, recData, simInfo.a[0], simInfo.a[1], 1);

            // get two-point correlation change
            for(int i = 0; i < simInfo.structSize; i++)
            {
                simInfo.tpChange[i] = rec.localTP[i] - rec.localTP_b4[i];
            }


            // get chord-length change
            for (int i = 0; i < rec.largestSide; i++)
            {
                simInfo.chordChange[i] = rec.localChord[i] - rec.localChord_b4[i];
                simInfo.totalChordChange += rec.localChord[i] - rec.localChord_b4[i];
            }

            // calculate Eswap
            simInfo.Eswap = 0;
            for (int i = 0; i < simInfo.structSize; i++)
            {
                if(target.S11[i] != 0)
                {
                    simInfo.Eswap += pow(((rec.S11[i] + 2*simInfo.tpChange[i])/rec.TP_Total[i] - target.S11[i]/target.TP_Total[i]),2);
                }
            }

            for (int i = 0; i<rec.largestSide; i++)
            {
                simInfo.Eswap += pow((rec.C00[i] + simInfo.chordChange[i])/(rec.ChordTotal + simInfo.totalChordChange) - target.C00[i]/target.ChordTotal,2);
            }
            // swap, mutate, or reverse
            if(simInfo.Eswap > simInfo.Ecurrent)
            {
                // reverse, no mutations allowed
                if (recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] == 0)
                {
                    recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] = 1;
                    recData[simInfo.tempCoord2[0]*rec.width + simInfo.tempCoord2[1]] = 0;
                }
                else
                {
                    recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] = 0;
                    recData[simInfo.tempCoord2[0]*rec.width + simInfo.tempCoord2[1]] = 1;
                }
            }
            else
            {
                // successful swap
                simInfo.swapCount++;
                // update correlations
                simInfo.Ecurrent = simInfo.Eswap;
                // update correlation functions
                for (int i = 0; i < simInfo.structSize; i++)
                {
                    rec.S11[i] += 2*simInfo.tpChange[i];
                }
                for(int i = 0; i < rec.largestSide; i++)
                {
                    rec.C00[i] += simInfo.chordChange[i];
                }
                rec.ChordTotal += simInfo.totalChordChange;
                change_coordinates[4*swap_count + 0] = simInfo.tempCoord1[0];
                change_coordinates[4*swap_count + 1] = simInfo.tempCoord1[1];
                change_coordinates[4*swap_count + 2] = simInfo.tempCoord2[0];
                change_coordinates[4*swap_count + 3] = simInfo.tempCoord2[1];

                swap_count++;
            }
            local_iter++;
        }

        MPI_Barrier(comm);

        MPI_Gather(change_coordinates, simInfo.localIter*4, MPI_INT, all_coordinates, simInfo.localIter*4, MPI_INT, 0, comm);
        MPI_Barrier(comm);

        // pass new struc, correlation fnctions, iterCt, Energy, and total_chord
        MPI_Bcast(recData, rec.nElements, MPI_UNSIGNED_CHAR, 0, comm);
        MPI_Barrier(comm);
        MPI_Bcast(rec.S11, simInfo.structSize, MPI_DOUBLE, 0, comm);
        MPI_Barrier(comm);
        MPI_Bcast(rec.C00, rec.largestSide, MPI_DOUBLE, 0, comm);
        MPI_Barrier(comm);
        MPI_Bcast(&rec.ChordTotal, 1, MPI_LONG, 0, comm);
        MPI_Barrier(comm);
        MPI_Bcast(&simInfo.Ecurrent, 1, MPI_DOUBLE, 0, comm);
        MPI_Barrier(comm);
        MPI_Bcast(&iterCount, 1, MPI_LONG, 0, comm);
        MPI_Barrier(comm);

        // check termination criteria

        if(iterCount > simInfo.maxIter)
        {
            printf("Exceeded max. iter. Closing...\n");
            break;
        }
        if(simInfo.Ecurrent < simInfo.MinEnergy)
        {
            printf("Exceeded minimum energy. Closing...\n");
            break;
        } 

        // else zero correlation functions and calculate things again

        for (int i = 0; i < simInfo.localIter; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                change_coordinates[i*4 + j] = -1;
            }
        }
    }

    MPI_Barrier(comm);
    return 0;
}



int YTA_3D(options *opts, MPI_Comm comm)
{
    int rank, nProc;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &nProc);

    meshInfo rec;
    meshInfo target;
    sYTA simInfo;

    if (parseInputYTA(&simInfo) == 1)
    {
        return 1;
    }

    if(opts->verbose && rank == 0)
        printOptsYTA(&simInfo);

    simInfo.T0 = 0;

    clock_t begin = clock();
    double t1, t2;
    long int iterCount = 0;
    t1 = MPI_Wtime();
    // Allocated necessary arrays

    rec.height = opts->targetHeight;
    rec.width = opts->targetWidth;
    rec.depth = opts->targetDepth;
    return 0;
}
