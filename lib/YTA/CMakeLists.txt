# YTA CMAKE

add_library(yta_lib STATIC)

# define source files

# target_sources(yta_lib
#     PRIVATE
#         YTA.cpp
#     PUBLIC
#         YTA.hpp
# )



# Check for optional MPI

find_package(MPI)

if(MPI_CXX_FOUND)
    message(STATUS "MPI Found, enabling parallel processing features.")

    target_sources(yta_lib
        PRIVATE
            YTA_mpi.cpp
        PUBLIC
            YTA.hpp
    )

    target_link_libraries(yta_lib PRIVATE MPI::MPI_CXX)
    target_compile_definitions(yta_lib PUBLIC USE_MPI)

else()
    target_sources(yta_lib
        PRIVATE
            YTA.cpp
        PUBLIC
            YTA.hpp
    )
endif()

target_link_libraries(yta_lib PUBLIC OpenMP::OpenMP_CXX)

target_include_directories(yta_lib PUBLIC "../core/")
target_include_directories(yta_lib PUBLIC "../utils/")
target_include_directories(yta_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
