#ifndef _STRUCT
#define _STRUCT

typedef struct
{
    // This function will hold information related to the structures
    int width;
    int height;
    int depth;
    long int nElements;
} meshInfo;

typedef struct
{
    char* inputName;        // expected input file name
    char* outputName;       // expected output file name
    int simType;            // dictates type of simulation
    int targetWidth;        // desired width (pixels)
    int targetHeight;       // desired height (pixels)
    int targetDepth;        // desired depth (pixels)
    int RSPM_R;             // RSPM Radius
    int nD;                 // controls number of dimensions
    long int maxIter;       // Cutoff number of iterations
    double minEnergy;       // Cutoff energy
    int TH;                // Binary threshold
    int verbose;            // controls printing to command line (1 == true)
}options;

#endif