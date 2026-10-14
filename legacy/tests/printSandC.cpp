#include <utils.hpp>
#include <stdbool.h>
#include <stdio.h>
#include <dataStructures.hpp>
#include <calcFunctions.hpp>
#include <iostream>
#include <fstream>


int main(int argc, char *argv[])
{
    // declare data structures
    options opts;
    meshInfo target;

    // Print a hello or something

    printf("---------------------------------------------------------\n\n");
    printf("     Running Test: Print Characterization Functions      \n\n");
    printf("---------------------------------------------------------\n\n");
    
    // read user info
    bool acceptableInput = false;

    char inputName[100];

    printf("Enter Target File Name:\n");

    std::cin >> inputName;

    opts.inputName = inputName;

    // try to read this input

    unsigned char *target_data;

    if(readTarget2D(&opts, &target, &target_data) == 1)
    {
        printf("Error reading image. Exiting...\n");
        return 1;
    }

    // Enter threshold

    acceptableInput = false;

    while(!acceptableInput)
    {
        printf("Enter Threshold (0 < TH < 255)\n");
        std::cin >> opts.TH;
        acceptableInput = true;

        if (opts.TH <= 0 || opts.TH >= 255)
        {
            printf("Value Entered is not acceptable. TH = %d\n", opts.TH);
            printf("Please Enter again:\n");
        }
    }

    // binarize target_data
    target.nElements = target.height * target.width;
    for (int i = 0; i < target.nElements; i++)
    {
        if(target_data[i] < opts.TH)
        {
            target_data[i] = 0;
        } else
        {
            target_data[i] = 1;
        }
    }

    // If successful, get input on neighborhood size for correlation functions

    int block_size;

    acceptableInput = false;

    while(!acceptableInput)
    {
        printf("Block Size:\n");
        std::cin >> block_size;

        acceptableInput = true;

        if (block_size < 1)
        {
            printf("Please enter another input\n");
            printf("Value = %d not acceptable\n", block_size);
            acceptableInput = false;
        }
    }

    opts.blockSize = block_size;
    opts.structSize = (block_size * block_size) / 2 + 1;

    //get largest side
    // notation (var) = (condition) ? (val iftrue) : (val iffalse)
    target.largestSide = (target.height > target.width) ? target.height : target.width;

    // Allocate Space for Correlation Functions

    target.S00 = (double *)malloc(sizeof(double) * opts.structSize);
    target.S11 = (double *)malloc(sizeof(double) * opts.structSize);
    target.S01 = (double *)malloc(sizeof(double) * opts.structSize);

    target.C00 = (double *)malloc(sizeof(double) * target.largestSide);

    // set memory

    memset(target.S00, 0, sizeof(double) * opts.structSize);
    memset(target.S11, 0, sizeof(double) * opts.structSize);
    memset(target.S01, 0, sizeof(double) * opts.structSize);

    memset(target.C00, 0, sizeof(double) * target.largestSide);

    // calculate initial correlations
    TP2D(target_data, &opts, &target);
    CL2D(target_data, &opts, &target);


    // print outputs

    FILE *OUT = fopen("testCorrelation.csv", "w+");

    fprintf(OUT, "Dist,C11,S11\n");
    
    for(int i = 0; i < opts.structSize; i++)
    {
        if (i < target.largestSide)
        {
            fprintf(OUT, "%d,%lf,%lf\n", i, target.C00[i], target.S11[i]);
        }
        else
        {
            fprintf(OUT, "%d,%lf,%lf\n", i, 0.0, target.S11[i]);
        }
    }

    fclose(OUT);


    return 0;
}