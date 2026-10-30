
#include <parseInput.hpp>

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
    printf("Binary Threshold: %d\n", opts->TH);
    printf("Target Width: %d\n", opts->targetWidth);
    printf("Target Height: %d\n", opts->targetHeight);

    if(opts->nD == 3)
        printf("Target Depth: %d\n", opts->targetDepth);

    return 0;
}

int printOptsYTA(sYTA *yta_opts)
{
    printf("------------------------------------\n\n");
    printf("            Sim - Options:          \n\n");
    printf("------------------------------------\n\n");

    printf("RSPM Radius: %d\n", yta_opts->RSPM_R);
    printf("Min. Energy: %1.3e\n", yta_opts->MinEnergy);
    printf("Max. Iter: %ld\n", yta_opts->maxIter);
    printf("Block Size: %d\n", yta_opts->blk_size);
    printf("Struct Size: %d\n", yta_opts->structSize);

    return 0;
}

int parseGeneralInput(options *opts)
{
    // assume filename
    std::string filename = "general_input.txt";

    if(FILE *TEST = fopen(filename.c_str(), "r"))
    {
        fclose(TEST);
    }else
    {
        printf("Couldn't find input file general_input.txt, please verify that it exists.\n");
        return 1;

    }

    // declare defaults

    opts->nThreads = 1;

    // read file

    std::ifstream InputFile(filename);
    std::string fileText;

    double tempD;
    char tempC[1000];
    char tempFilenames[1000];
    // set defaults

    opts->targetWidth = 100;
    opts->targetHeight = 100;
    opts->targetDepth = 1;
    opts->nD = 2;
    opts->verbose = 1;

    opts->nThreads = 1;

    // Allocate memory for reading
    
    opts->inputName  = (char*)malloc(1000*sizeof(char));
	opts->outputName = (char*)malloc(1000*sizeof(char));

    // sprintf(opts->outputName, "defOut.csv");
    // sprintf(opts->inputName, "defIn.csv");

    // read input file

    while (std::getline(InputFile, fileText))
    {
        sscanf(fileText.c_str(), "%s %lf", tempC, &tempD);
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
        else if (strcmp(tempC, "inputName:") == 0)
        {
            sscanf(fileText.c_str(), "%s %s", tempC, tempFilenames);
            strcpy(opts->inputName, tempFilenames);
        }
        else if (strcmp(tempC, "outputName:") == 0)
        {
            sscanf(fileText.c_str(), "%s %s", tempC, tempFilenames);
            strcpy(opts->outputName, tempFilenames);
        }
        else if (strcmp(tempC, "TH:") == 0)
        {
            opts->TH = (int)tempD;
        }
        else if(strcmp(tempC, "SimType:") == 0)
        {
            opts->simType = (int) tempD;
        }
    }

    // update struct size based on block size
    InputFile.close();

    return 0;
}


int parseInputYTA(sYTA *yta_opts)
{
    // assume filename
    std::string filename = "yta_opts.txt";

    if(FILE *TEST = fopen(filename.c_str(), "r"))
    {
        fclose(TEST);
    }else
    {
        printf("Couldn't find input file yta_opts.txt, please verify that it exists.\n");
        return 1;

    }

    // declare defaults

    yta_opts->MinEnergy = 0;

    // read file

    std::ifstream InputFile(filename);
    std::string fileText;

    double tempD;
    char tempC[1000];
    char tempFilenames[1000];
    // set defaults

    yta_opts->blk_size = 20;
    yta_opts->lambda = 0.995;

    yta_opts->maxIter = 1e6;
    yta_opts->rootIter = 10;
    yta_opts->localIter = 1e5;
    yta_opts->RSPM_R = 10;

    yta_opts->mutCount = 0;
    yta_opts->swapCount = 0;
    // read input file

    while (std::getline(InputFile, fileText))
    {
        sscanf(fileText.c_str(), "%s %lf", tempC, &tempD);
        if (strcmp(tempC, "blw_size:") == 0)
        {
            yta_opts->blk_size = (unsigned int)tempD;
        }
        else if (strcmp(tempC, "maxIter:") == 0)
        {
            yta_opts->maxIter = (unsigned long int)tempD;
        }
        else if (strcmp(tempC, "minEnerg:") == 0)
        {
            yta_opts->MinEnergy = tempD;
        }
        else if (strcmp(tempC, "rootIter:") == 0)
        {
            yta_opts->rootIter = (unsigned long int)tempD;
        }
        else if (strcmp(tempC, "localIter:") == 0)
        {
            yta_opts->localIter = (unsigned long int)tempD;
        }
        else if (strcmp(tempC, "RSPM_R:") == 0)
        {
            yta_opts->RSPM_R = (unsigned int)tempD;
        }
        else if (strcmp(tempC, "Lambda:") == 0)
        {
            yta_opts->lambda = tempD;
        }
    }

    // update struct size based on block size

    yta_opts->structSize = (yta_opts->blk_size * yta_opts->blk_size/ 2) + 1;

    InputFile.close();

    return 0;

}
