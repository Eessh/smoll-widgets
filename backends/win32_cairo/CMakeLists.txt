cmake_minimum_required(VERSION 3.25)

project(win32_cairo_backend)

set(CMAKE_C_STANDARD 99)

include_directories(
  ${PROJECT_SOURCE_DIR}/cairo-windows-1.17.2/include
)

find_library(cairo cairo ${PROJECT_SOURCE_DIR}/cairo-windows-1.17.2/lib/x64)

add_executable(win32_cairo_backend
  ${PROJECT_SOURCE_DIR}/win32_cairo_backend.c
  ${PROJECT_SOURCE_DIR}/win32_cairo_example.c
)

target_link_libraries(win32_cairo_backend smoll-widgets ${cairo})
