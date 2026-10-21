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
