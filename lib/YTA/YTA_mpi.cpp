
#include <YTA.hpp>

int YTA_2D(options *opts)
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::cout << "Hello from MPI Rank" << rank << "\n";

    return 0;
}
