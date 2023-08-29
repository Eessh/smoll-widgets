cmake_minimum_required(VERSION 3.25)

project(sdl2_cairo_backend)

set(CMAKE_C_STANDARD 99)

include_directories(
  ${PROJECT_SOURCE_DIR}/cairo-windows-1.17.2/include
  ${PROJECT_SOURCE_DIR}/SDL2-2.26.5/x86_64-w64-mingw32/include
  ${PROJECT_SOURCE_DIR}/SDL2-2.26.5/x86_64-w64-mingw32/include/SDL2
)

find_library(cairo cairo ${PROJECT_SOURCE_DIR}/cairo-windows-1.17.2/lib/x64)
find_library(SDL2 libSDL2 ${PROJECT_SOURCE_DIR}/SDL2-2.26.5/x86_64-w64-mingw32/lib)
find_library(SDL2main libSDL2main ${PROJECT_SOURCE_DIR}/SDL2-2.26.5/x86_64-w64-mingw32/lib)

add_executable(sdl2_cairo_backend
  ${PROJECT_SOURCE_DIR}/sdl2_cairo_backend.c
  ${PROJECT_SOURCE_DIR}/sdl2_cairo_example.c
)

target_link_libraries(sdl2_cairo_backend smoll-widgets ${cairo} ${SDL2main} ${SDL2})
