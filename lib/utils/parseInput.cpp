
#include <parseInput.hpp>

int parseGeneralInput(options *opts)
{
    // assume filename
    std::string filename = "general_input.txt";
    // try to open
    if (std::filesystem::exists(filename) == false)
    {
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

    return 0;
}
