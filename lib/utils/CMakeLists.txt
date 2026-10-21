# utils CMake

add_library(utils_lib STATIC)

# define source files

target_sources(utils_lib
    PRIVATE
        parseInput.cpp
        correlationFunctions.cpp
    PUBLIC
        parseInput.hpp
        ImportAll_utils.hpp
        correlationFunctions.hpp
        stb_image.h
        stb_image_write.h
)

target_include_directories(utils_lib PUBLIC "../core/")
target_include_directories(utils_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
