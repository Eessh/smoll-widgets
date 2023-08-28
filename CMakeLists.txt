cmake_minimum_required(VERSION 3.25)

project(smoll-widgets)

set(CMAKE_C_STANDARD 99)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/include/widgets
)

add_library(smoll-widgets STATIC
  ${PROJECT_SOURCE_DIR}/src/base_widget.c
  ${PROJECT_SOURCE_DIR}/src/command_buffer.c
  ${PROJECT_SOURCE_DIR}/src/internal_context.c
  ${PROJECT_SOURCE_DIR}/src/smoll_context.c
  ${PROJECT_SOURCE_DIR}/src/widgets/box.c
  ${PROJECT_SOURCE_DIR}/src/widgets/button.c
  ${PROJECT_SOURCE_DIR}/src/widgets/checkbox.c
  ${PROJECT_SOURCE_DIR}/src/widgets/flex_column_view.c
  ${PROJECT_SOURCE_DIR}/src/widgets/flex_row_view.c
  ${PROJECT_SOURCE_DIR}/src/widgets/progress_bar.c
  ${PROJECT_SOURCE_DIR}/src/widgets/toggle.c
)
