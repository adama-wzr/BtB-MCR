#ifndef _CORE
#define _CORE


typedef struct
{
    // This function will hold information related to the structures
    int width;
    int height;
    int depth;
    long int nElements;
    int largestSide;
    // Include pointers to correlation functions
    double *S11;
    double *S01;
    double *S00;
    double *C00;
    // Total TP
    long int *TP_Total;
    long int ChordTotal;
    // Correlation Function Changes
    long int *localTP_b4;
    long int *localTP;
    long int *localChord;
    long int *localChord_b4;
    // physical descriptors
    double VF;
    double porosity;
} meshInfo;

typedef struct
{
    char* inputName;        // expected input file name
    char* outputName;       // expected output file name
    int simType;            // dictates type of simulation
    int targetWidth;        // desired width (pixels)
    int targetHeight;       // desired height (pixels)
    int targetDepth;        // desired depth (pixels)
    // int RSPM_R;             // RSPM Radius
    int nD;                 // controls number of dimensions
    // long int maxIter;       // Cutoff number of iterations
    // double minEnergy;       // Cutoff energy
    int TH;                 // Binary threshold
    int verbose;            // controls printing to command line (1 == true)
    // int blockSize;          // S11 block size
    // int structSize;         // amount of data in a block
    double lambda;          // cooling rate for Simulated Annealing
    int nThreads;
}options;

typedef struct
{
    // usr Input Params
    unsigned int blk_size;
    unsigned long int maxIter;
    unsigned long int rootIter;
    unsigned long int localIter;
    double MinEnergy;
    unsigned int RSPM_R;
    // others
    unsigned int structSize;
    // Temperature related parameters
    double Tk;
    double T0;
    double lambda;
    // Simulation results
    long int mutCount;
    long int swapCount;
    long int iterCount;
    // temp swap variables
    int a[3];
    int b[3];
    int tempCoord1[3];
    int tempCoord2[3];
    double Ecurrent;
    double Eswap;
    long int *chordChange;
    long int *tpChange;
    long int totalChordChange;
}sYTA;

#endif
