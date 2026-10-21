# core CMAKE
add_library(core_lib STATIC)

# define source files

target_sources(core_lib
    PRIVATE
        core.cpp
    PUBLIC
        ImportAll_core
        core.hpp
)

target_include_directories(core_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
