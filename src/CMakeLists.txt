# main executable

add_executable(BtB_main
    main.cpp
)

target_link_libraries(BtB_main
    PUBLIC
        core_lib
        yta_lib
        utils_lib
)

find_package(MPI)
if(MPI_CXX_FOUND)
    target_link_libraries(BtB_main PRIVATE MPI::MPI_CXX)
endif()
