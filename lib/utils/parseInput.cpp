
#include <parseInput.hpp>


int parseGeneralInput(options *opts)
{
    // assume filename
    std::string filename = "general_input.txt";
    // // try to open
    // if (std::filesystem::exists(filename) == false)
    // {
    //     printf("Couldn't find input file general_input.txt, please verify that it exists.\n");
    //     return 1;
    // }

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

    while(std::getline(InputFile, fileText))
    {
        sscanf(fileText.c_str(), "%s %lf", tempC, &tempD);
        if (strcmp(tempC, "nThreads:") == 0)
        {
            opts->nThreads = (int)tempD;
        }
    }

    if (opts->nThreads > 20)
    {
        printf("Anomalous nThreads, exiting....\n");
        return 1;
    }

    return 0;
}
