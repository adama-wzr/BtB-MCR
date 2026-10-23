
#include <YTA.hpp>

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

int YTA_2D(options *opts, MPI_Comm comm)
{
    int rank;
    MPI_Comm_rank(comm, &rank);

    std::cout << "Hello from MPI Rank" << rank << "\n";

    meshInfo rec;
    meshInfo target;
    sYTA simInfo;

    simInfo.lambda = opts->lambda;
    simInfo.T0 = 0;


    if(rank == 0)
    {
        unsigned char *targetData;
        if (readTarget2D(opts, &target, &targetData));
        {
            printf("Error, couldn't read input target\n");
            MPI_Barrier(comm);
            return 0;
        }
        unsigned char *recData = (unsigned char*)malloc(sizeof(unsigned char*)*opts->targetHeight*opts->targetWidth);
        YTA_2D_Init(opts, &target, &rec, &simInfo, targetData, recData);
    }

    MPI_Barrier(comm);

    return 0;
}
