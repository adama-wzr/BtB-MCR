#ifndef _OPTS
#define _OPTS

#include <stdio.h>
#include <string>
#include <dataStructures.hpp>
#include <stdbool.h>
#include <fstream>
#include <cfloat>
#include <vector>
#include <iostream>
#include <stdlib.h>

int printOpts(options *opts)
{
    /*
        Function printOpts:
        Inputs:
            - pointer to options struct
        Ouputs:
            - none.
        Function will print user entered options (or defaults) to
        the command line. The function also does basic error checking.
    */
    
    printf("------------------------------------\n\n");
    printf("            BtB - Options:          \n\n");
    printf("------------------------------------\n\n");

    if(opts->nD != 2 && opts->nD != 3)
    {
        printf("Error, nD not acceptable (nD = %d)\n", opts->nD);
        return 1;
    }

    printf("nD: %d\n", opts->nD);
    printf("Input Name: %s\n", opts->inputName);
    printf("Output Name: %s\n", opts->outputName);
    printf("Target Width: %d\n", opts->targetWidth);
    printf("Target Height: %d\n", opts->targetHeight);

    if(opts->nD == 3)
        printf("Target Depth: %d\n", opts->targetDepth);
    
    printf("------------------------------------\n\n");
    printf("            Sim - Options:          \n\n");

    printf("RSPM Radius: %d\n", opts->RSPM_R);
    printf("Min. Energy: %1.3e\n", opts->minEnergy);
    printf("Max. Iter %ld\n", opts->maxIter);

    return 0;
}

void readOpts(options *opts, char *inputTXT)
{
    /*
        Function readOpts:
        Inputs:
            - pointer to options struct
        Outputs:
            - none.

        Function will read the user entered options and store it
        in the opts struct.
    */
    // Pre-allocate arrays for reading
    std::string myText;

    char tempC[1000];
    double tempD;
    char tempFilenames[1000];
    std::ifstream InputFile(inputTXT);
    
    // set defaults

    opts->targetWidth = 100;
    opts->targetHeight = 100;
    opts->targetDepth = 1;
    opts->nD = 2;
    opts->verbose = 1;

    opts->maxIter = 1e9;
    opts->minEnergy = 1e-6;

    // Allocate memory for reading
    
    opts->inputName  = (char*)malloc(1000*sizeof(char));
	opts->outputName = (char*)malloc(1000*sizeof(char));

    // sprintf(opts->outputName, "defOut.csv");
    // sprintf(opts->inputName, "defIn.csv");

    // read input file

    while (std::getline(InputFile, myText))
    {

        sscanf(myText.c_str(), "%s %lf", tempC, &tempD);
        if (strcmp(tempC, "nD:") == 0)
        {
            opts->nD = tempD;
        }
        else if (strcmp(tempC, "width:") == 0)
        {
            opts->targetWidth = (int)tempD;
        }
        else if (strcmp(tempC, "height:") == 0)
        {
            opts->targetHeight = (int)tempD;
        }
        else if (strcmp(tempC, "depth:") == 0)
        {
            opts->targetDepth = (int)tempD;
        }
        else if (strcmp(tempC, "RSPM_R:") == 0)
        {
            opts->RSPM_R = (int)tempD;
        }
        else if (strcmp(tempC, "inputName:") == 0)
        {
            sscanf(myText.c_str(), "%s %s", tempC, tempFilenames);
            strcpy(opts->inputName, tempFilenames);
        }
        else if (strcmp(tempC, "outputName:") == 0)
        {
            sscanf(myText.c_str(), "%s %s", tempC, tempFilenames);
            strcpy(opts->outputName, tempFilenames);
        }
        else if (strcmp(tempC, "Iter:") == 0)
        {
            double temp = tempD;
            opts->maxIter = (long int)temp;
        }
        else if (strcmp(tempC, "Energy:") == 0)
        {
            opts->minEnergy = tempD;
        }
    }

    InputFile.close();

    return;
}

#endif