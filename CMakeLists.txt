
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# version information---------------------------------------------------
project(BtB-MCR LANGUAGES CXX VERSION 1.0)

# # find and include MPI
# find_package(MPI REQUIRED)

include_directories(SYSTEM ${MPI_INCLUDE_PATH})

# Include directories -------------------------
include_directories(
    BtB-MCR PUBLIC
    "${PROJECT_SOURCE_DIR}"
    "${PROJECT_SOURCE_DIR}/lib"
    "${PROJECT_SOURCE_DIR}/lib/YTA"
    "${PROJECT_SOURCE_DIR}/tests"
)

# add executable 

add_executable(
    BtB-MCR
    src/main.cpp
)

# Link MPI library
# target_link_libraries(BtB-MCR ${MPI_C_LIBRARIES})