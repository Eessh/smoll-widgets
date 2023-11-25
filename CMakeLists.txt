cmake_minimum_required(VERSION 3.25)

project(smoll-widgets)

set(CMAKE_C_STANDARD 99)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/include/widgets
  ${PROJECT_SOURCE_DIR}/log-boii
)

add_library(smoll-widgets STATIC
  ${PROJECT_SOURCE_DIR}/log-boii/log_boii.c
  ${PROJECT_SOURCE_DIR}/src/base_widget.c
  ${PROJECT_SOURCE_DIR}/src/command_buffer.c
  ${PROJECT_SOURCE_DIR}/src/internal_context.c
  ${PROJECT_SOURCE_DIR}/src/smoll_context.c
  ${PROJECT_SOURCE_DIR}/src/widgets/box.c
  ${PROJECT_SOURCE_DIR}/src/widgets/button.c
  ${PROJECT_SOURCE_DIR}/src/widgets/checkbox.c
  ${PROJECT_SOURCE_DIR}/src/widgets/flex_view.c
  ${PROJECT_SOURCE_DIR}/src/widgets/list_view.c
  ${PROJECT_SOURCE_DIR}/src/widgets/progress_bar.c
  ${PROJECT_SOURCE_DIR}/src/widgets/split.c
  ${PROJECT_SOURCE_DIR}/src/widgets/split_view.c
  ${PROJECT_SOURCE_DIR}/src/widgets/toggle.c
)

# Startup example powered by SDL2+Cairo backend.
# Useful for testing and also SDL2 is cross-platform.
add_subdirectory(backends/sdl2_cairo)
