# main executable

add_executable(BtB_main
    main.cpp
)

target_link_libraries(BtB_main
    PUBLIC
        core_lib
        models_lib
        utils_lib
)
