
#include <YTA.hpp>


/*
 *
 *  Initializers and Free's
 *
 *
 * */


int YTA_2D_Init(options *opts, meshInfo *target, meshInfo *rec, sYTA *simInfo, unsigned char *targetData, unsigned char *recData)
{
    // binarize the target

    for(int i = 0; i < target->nElements; i++)
    {
        if(targetData[i] < opts->TH)
        {
            targetData[i] = 0;
        }else
        {
            targetData[i] = 1;
        }
    }

    return 0;
}


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
    int rank;
    MPI_Comm_rank(comm, &rank);

    std::cout << "Hello from MPI Rank" << rank << "\n";

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

    t1 = MPI_Wtime();

    // Allocated necessary arrays

    rec.height = opts->targetHeight;
    rec.width = opts->targetWidth;

    rec.nElements = rec.height * rec.width;

    rec.largestSide = (rec.height > rec.width) ? rec.height : rec.width;

    // allocate arrays for reconstruction
    unsigned char *recData = (unsigned char *)malloc(sizeof(char) * rec.nElements);
    memset(recData, 0, sizeof(char) * rec.nElements);

    YTA_baseAlloc2D(&simInfo, &target, &rec);

    int *change_coordinates, *all_coordinates;

    int* swap_flag;

    change_coordinates = (int *)malloc(sizeof(int)*simInfo.localIter*6);

    if (rank == 0)
    {
        all_coordinates = (int *)malloc(sizeof(int)*simInfo.localIter*6*opts->nThreads);
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

        long int iterCount = 0;
        while(iterCount < simInfo.rootIter && simInfo.Ecurrent > simInfo.MinEnergy)
        {
            if (iterCount != 0 && simInfo.Ecurrent > simInfo.Eswap)
                simInfo.Ecurrent = simInfo.Eswap;

            // Pick coords
            simInfo.tempCoord1[0] = rand() % (int)(rec.height);
            simInfo.tempCoord1[1] = rand() % (int)(rec.width);

            simInfo.tempCoord2[0] = rand() % (int)(rec.height);
            simInfo.tempCoord2[1] = rand() % (int)(rec.width);

            while(detectInterface2D(recData, &rec, simInfo.tempCoord1[0], simInfo.tempCoord1[0]) == 1)
            {
                simInfo.tempCoord1[0] = rand() % (int)(rec.height);
                simInfo.tempCoord1[1] = rand() % (int)(rec.width);
            }

            while(detectInterface2D(recData, &rec, simInfo.tempCoord2[0], simInfo.tempCoord2[0]) == 1)
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
                    while(detectInterface2D(recData, &rec, simInfo.tempCoord2[0], simInfo.tempCoord2[0]) == 1)
                    {
                        simInfo.tempCoord2[0] = rand() % (int)(rec.height);
                        simInfo.tempCoord2[1] = rand() % (int)(rec.width);
                    }
                }
            }

            // For a fact different phases and both interfaces
            // Set correlation changes to 0
            memset(rec.localTP, 0, sizeof(double)*simInfo.structSize);
            memset(rec.localTP_b4, 0, sizeof(double)*simInfo.structSize);
            memset(simInfo.tpChange, 0, sizeof(double)*simInfo.structSize);

            memset(rec.localChord, 0, sizeof(double)*rec.largestSide);
            memset(rec.localChord_b4, 0, sizeof(double)*rec.largestSide);
            memset(simInfo.chordChange, 0, sizeof(double)*rec.largestSide);


            // make sure a = 0 before swap and b = 1;
            if(recData[simInfo.tempCoord1[0]*rec.width + simInfo.tempCoord1[1]] == 0)
            {
                simInfo.a[0] = simInfo.tempCoord1[0];
                simInfo.a[1] = simInfo.tempCoord1[1];
                simInfo.b[0] = simInfo.tempCoord2[0];
                simInfo.b[1] = simInfo.tempCoord2[1];
            }

            // local costs
            localTP(&simInfo, &rec, recData, simInfo.b[1], simInfo.b[0], 0);

            localChord(&rec, recData, simInfo.b[1], simInfo.b[0], 0);
            localChord(&rec, recData, simInfo.a[1], simInfo.a[0], 0);

            // complete the swap
            recData[simInfo.a[1]*rec.width + simInfo.a[0]] = 1;
            recData[simInfo.b[1]*rec.width + simInfo.b[0]] = 0;

            // evaluate local correlation again
            localTP(&simInfo, &rec, recData, simInfo.a[1], simInfo.a[0], 1);

            localChord(&rec, recData, simInfo.b[1], simInfo.b[0], 1);
            localChord(&rec, recData, simInfo.a[1], simInfo.a[0], 1);

            // get two-point correlation change
            for(int i = 0; i < simInfo.structSize; i++)
            {
                simInfo.tpChange[i] = rec.localTP[i] - rec.localTP_b4[i];
            }

            simInfo.totalChordChange = 0;
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
                        rec.C00[i] += 2*simInfo.chordChange[i];
                    }
                    rec.ChordTotal += simInfo.totalChordChange;
                }
                else
                {
                    // no mutation, revert swap
                    if (recData[simInfo.tempCoord1[1]*rec.width + simInfo.tempCoord1[0]] == 0)
                    {
                        recData[simInfo.tempCoord1[1]*rec.width + simInfo.tempCoord1[0]] = 1;
                        recData[simInfo.tempCoord2[1]*rec.width + simInfo.tempCoord2[0]] = 0;
                    }
                    else
                    {
                        recData[simInfo.tempCoord1[1]*rec.width + simInfo.tempCoord1[0]] = 0;
                        recData[simInfo.tempCoord2[1]*rec.width + simInfo.tempCoord2[0]] = 1;
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
                    rec.C00[i] += 2*simInfo.chordChange[i];
                }
                rec.ChordTotal += simInfo.totalChordChange;
            }

            if(iterCount % 100000 == 0)
            {
                printf("Iter = %d, Mut = %d, Swap = %d, Energy = %1.3e\n", iterCount, simInfo.mutCount, simInfo.swapCount, simInfo.Ecurrent);
            }
            iterCount++;
        }

        // Out of the root main Loop, let's save the image just in case
        stbi_write_jpg("Test.jpg", rec.width, rec.height, 1, recData, 100);
    }

    // workers section of the code

    // Get relevant target correlation functions
    MPI_Bcast(target.S11, simInfo.structSize, MPI_DOUBLE, 0, comm);
    MPI_Bcast(target.C00, rec.largestSide, MPI_DOUBLE, 0, comm);

    MPI_Barrier(comm);

    return 0;
}
