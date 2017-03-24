cmake_minimum_required(VERSION 3.6)
project(Computer_Graphics)


find_package(SDL)


if ( NOT SDL_FOUND )
    message ( FATAL_ERROR "SDL not found!" )
endif ( NOT SDL_FOUND )


include_directories(${SDL_INCLUDE_DIR})
link_libraries(${SDL_LIBRARY})


INCLUDE_DIRECTORIES($(S_DIR)/SDLauxiliary.h)
INCLUDE_DIRECTORIES($(S_DIR)/TestModel.h)

file(GLOB SRC
        "Source/*.h"
        "Source/*.cpp"
        )

add_executable(Main ${SRC} Source/TGAImage.cpp Source/TGAImage.cpp Source/TGAImage.h)

set(EXECUTABLE_OUTPUT_PATH "..")



