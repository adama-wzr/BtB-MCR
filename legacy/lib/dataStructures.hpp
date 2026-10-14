#ifndef _STRUCT
#define _STRUCT

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
    int *TP_Total;
    long int ChordTotal;
    // Correlation Function Changes
    int *localTP_b4;
    int *localTP;
    int *localChord;
    int *localChord_b4;
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
    int RSPM_R;             // RSPM Radius
    int nD;                 // controls number of dimensions
    long int maxIter;       // Cutoff number of iterations
    double minEnergy;       // Cutoff energy
    int TH;                 // Binary threshold
    int verbose;            // controls printing to command line (1 == true)
    int blockSize;          // S11 block size
    int structSize;         // amount of data in a block
    double lambda;          // cooling rate for Simulated Annealing
}options;

typedef struct
{
    // Temperature related parameters
    double Tk;
    double T0;
    double lambda;
    // Simulation results
    int mutCount;
    int swapCount;
    long int iterCount;
    // temp swap variables
    int a[3];
    int b[3];
    int tempCoord1[3];
    int tempCoord2[3];
    double Ecurrent;
    double Eswap;
    int *chordChange;
    int *tpChange;
    int totalChordChange;
}sYTA;

#endif