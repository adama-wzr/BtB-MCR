#include <cstdio>
#include <stdlib.h>
#include <lib/YTA/YTA.cpp>
#include <lib/dataStructures.hpp>
#include <lib/userOpts.hpp>

int main(int argc, char *argv[])
{
    // Structs
    options opts;
    meshInfo target;
    meshInfo rec;

    char filename[50];

    sprintf(filename, "input.txt");

    // Parse opts
    readOpts(&opts, filename);

    if (opts.verbose)
        printOpts(&opts);
    
    // The only model that currently exists is the YTA
    
    YTA2D(&opts);

    // read image (test)
    
    unsigned char* simTarget;
    
    if (readTarget2D(&opts, &target, &simTarget))
    {
        printf("Error, couldn't read input or nChannels != 1\n");
        return 1;
    }

    // binarize

    for(int row = 0; row < target.height; row++)
    {
        for(int col = 0; col < target.width; col++)
        {
            if (simTarget[row*target.width + col] < opts.TH)
                simTarget[row*target.width + col] = 0;
            else
                simTarget[row*target.width + col] = 1;
        }
    }

    // print image
    // printf("Printing Output:\n");
    // writeTarget2D(&opts, &target, simTarget);

    return 0;
}
