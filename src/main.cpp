/*
 *
 * Welcome to BtB-MCR
 *
 *
 * Main Contributors:
 *
 * - Andre Adam :)
 *
 * */

// #include <cstdio>
// #include <stdlib.h>
// #include <YTA.hpp>
// #include <core.hpp>
// #include <utils.hpp>

#include <ImportAll_utils.hpp>
#include <ImportAll_core.hpp>
#include <YTA.hpp>

#ifdef USE_MPI
#include <mpi.h>
#endif

int main(int argc, char *argv[])
{
#ifdef USE_MPI
    int rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    options opts;

    parseGeneralInput(&opts); 

    if(rank == 0)
    {
        testCORF();
    }

    MPI_Barrier(MPI_COMM_WORLD);

    YTA_2D(&opts);

    MPI_Finalize();

#else
    options opts;
    int stat = parseGeneralInput(&opts);
    if (stat == 1)
    {
        return 1;
    }

    YTA_2D(&opts);
    testCORF();

#endif

    // Structs
    // options opts;
    // meshInfo target;
    // meshInfo rec;
    //
    // char filename[50];
    //
    // sprintf(filename, "input.txt");
    //
    // // Parse opts
    // readOpts(&opts, filename);
    //
    // if (opts.verbose)
    //     printOpts(&opts);
    //
    // // The only model that currently exists is the YTA
    //
    // YTA2D(&opts);
    //
    // return 0;
    //
    // // read image (test)
    //
    // unsigned char* simTarget;
    //
    // if (readTarget2D(&opts, &target, &simTarget))
    // {
    //     printf("Error, couldn't read input or nChannels != 1\n");
    //     return 1;
    // }
    //
    // // binarize
    //
    // for(int row = 0; row < target.height; row++)
    // {
    //     for(int col = 0; col < target.width; col++)
    //     {
    //         if (simTarget[row*target.width + col] < opts.TH)
    //             simTarget[row*target.width + col] = 0;
    //         else
    //             simTarget[row*target.width + col] = 1;
    //     }
    // }

    // print image
    // printf("Printing Output:\n");
    // writeTarget2D(&opts, &target, simTarget);

    return 0;
}
