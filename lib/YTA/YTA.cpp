#include <mpi/mpi.h>
#include <YTA.hpp>

int MPI_HelloWorld(int nCores)
{
    printf("I'm here!\n");

    meshInfo mesh;

    mesh.width = 10;
    mesh.height = 100;
    mesh.depth = 100;

    printf("Mesh set!\n");

    return 0;
}