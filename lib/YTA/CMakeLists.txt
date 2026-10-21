# YTA CMAKE

add_library(yta_lib STATIC)

# define source files

target_sources(yta_lib
    PRIVATE
        YTA.cpp
    PUBLIC
        YTA.hpp
)

target_link_libraries(yta_lib PUBLIC OpenMP::OpenMP_CXX)

target_include_directories(yta_lib PUBLIC "../core/")
target_include_directories(yta_lib PUBLIC "../utils/")
target_include_directories(yta_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
