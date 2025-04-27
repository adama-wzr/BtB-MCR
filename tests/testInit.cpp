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
    meshInfo rec;

    // Init flag

    bool RSPM = true;

    if (opts.RSPM_R < 1)
        RSPM = false;
    
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

    // Gather data for reconstruction

    acceptableInput = false;

    while(!acceptableInput)
    {
        printf("Enter Width (number of pixels):\n");
        std::cin >> opts.targetWidth;
        acceptableInput = true;
        if(opts.targetWidth < 50)
        {
            printf("Value too small, please try again.\n");
            acceptableInput = false;
        }
    }

    acceptableInput = false;

    while(!acceptableInput)
    {
        printf("Enter Height (number of pixels):\n");
        std::cin >> opts.targetHeight;
        acceptableInput = true;
        if(opts.targetHeight < 50)
        {
            printf("Value too small, please try again.\n");
            acceptableInput = false;
        }
    }

    // create reconstruction array

    rec.width = opts.targetWidth;
    rec.height = opts.targetWidth;
    rec.nElements = rec.width * rec.height;

    unsigned char *recData = (unsigned char *)malloc(sizeof(char) * rec.nElements);
    memset(recData, 0, sizeof(char) * rec.nElements);

    // calc target VF

    target.porosity = calc_pore2D(target_data, target.height, target.width);

    target.VF = 1.0 - target.porosity;

    printf("Target VF = %1.3e\n", target.VF);

    // Get radius for RSPM

    acceptableInput = false;

    while (!acceptableInput)
    {
        printf("Please enter radius for RSPM:\n");
        printf("Note: radius = 0 is just random noise\n");
        std::cin >> opts.RSPM_R;

        acceptableInput = true;
        if (opts.RSPM_R < 0)
        {
            printf("Please enter number equal or larger than 0.\n");
            acceptableInput = false;
        }
    }
    
    // initialize reconstruction array

    if (opts.RSPM_R == 0)
    {
        // generate random noise to match VF
        randomNoise2D(&rec, recData, target.VF);
    }else
    {
        // RSPM
        RSPM2D(&opts, &rec, recData, target.VF);
        // Correct data
        correctVF2D(&rec, recData, target.VF);
    }

    rec.porosity = calc_pore2D(recData, rec.height, rec.width);
    rec.VF = 1.0 - rec.porosity;

    printf("Rec. VF = %1.3e\n", rec.VF);

    // save?

    for(int i = 0; i < rec.nElements; i++)
    {
        // convert solid to white
        recData[i] = recData[i] * 255;
    }

    opts.outputName = (char*)malloc(100*sizeof(char));

    sprintf(opts.outputName, "test.jpg");
    writeTarget2D(&opts, &rec, recData);

    return 0;
}