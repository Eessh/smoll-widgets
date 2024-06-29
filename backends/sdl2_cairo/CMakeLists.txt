cmake_minimum_required(VERSION 3.25)
set(CMAKE_C_STANDARD 17)

project(sdl2_cairo_backend)

include_directories(
  ${PROJECT_SOURCE_DIR}/cairo-windows-1.17.2/include
  if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    ${PROJECT_SOURCE_DIR}/SDL2-Devel-2.30.4/MSVC/include
  else()
    ${PROJECT_SOURCE_DIR}/SDL2-2.26.5/x86_64-w64-mingw32/include
    ${PROJECT_SOURCE_DIR}/SDL2-2.26.5/x86_64-w64-mingw32/include/SDL2
  endif()
)

find_library(cairo cairo ${PROJECT_SOURCE_DIR}/cairo-windows-1.17.2/lib/x64)
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  find_library(SDL2 SDL2 ${PROJECT_SOURCE_DIR}/SDL2-Devel-2.30.4/MSVC/lib/x64)
  find_library(SDL2main SDL2main ${PROJECT_SOURCE_DIR}/SDL2-Devel-2.30.4/MSVC/lib/x64)
else()
  find_library(SDL2 libSDL2 ${PROJECT_SOURCE_DIR}/SDL2-2.26.5/x86_64-w64-mingw32/lib)
  find_library(SDL2main libSDL2main ${PROJECT_SOURCE_DIR}/SDL2-2.26.5/x86_64-w64-mingw32/lib)
endif()

add_executable(sdl2_cairo_backend
  ${PROJECT_SOURCE_DIR}/sdl2_cairo_backend.c
  ${PROJECT_SOURCE_DIR}/sdl2_cairo_example.c
)

target_link_libraries(sdl2_cairo_backend PRIVATE smoll-widgets ${cairo} ${SDL2main} ${SDL2})

add_custom_command(TARGET sdl2_cairo_backend POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  "${PROJECT_SOURCE_DIR}/cairo-windows-1.17.2/lib/x64/cairo.dll"
  $<TARGET_FILE_DIR:sdl2_cairo_backend>
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_custom_command(TARGET sdl2_cairo_backend POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/SDL2-Devel-2.30.4/MSVC/lib/x64/SDL2.dll"
    $<TARGET_FILE_DIR:sdl2_cairo_backend>
  )
else()
  add_custom_command(TARGET sdl2_cairo_backend POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/SDL2-Devel-2.30.4/MinGW/x86_64-w64-mingw32/bin/x64/SDL2.dll"
    $<TARGET_FILE_DIR:sdl2_cairo_backend>
  )
endif()
