#ifndef SMOLL_WIDGETS__BOX_H
#define SMOLL_WIDGETS__BOX_H

#include "../base_widget.h"

/// @brief Box Widget.
///        Use `box_new()` to create this widget.
///        Acts as a static non-autoresizable container. Can only resize it
///        by directly changing width (or) height.
///        Preferably use this box widget as root widget.
typedef struct box
{
  /// @brief X-axis padding. This will be the value for both padding-left
  ///        and padding-right.
  uint16 padding_x;

  /// @brief Y-axis padding. This will be the value for both padding-top
  ///        and padding-bottom.
  uint16 padding_y;

  /// @brief Background color of box.
  color background;

  /// @brief Pointer to base widget of box.
  base_widget* base;
} box;

/// @brief Box pointer result.
typedef struct result_box_ptr
{
  bool ok;
  union
  {
    box* value;
    const char* error;
  };
} result_box_ptr;

/// @brief Creates new box widget.
/// @param parent_base pointer to base widget of parent.
/// @return Box pointer result.
result_box_ptr box_new(base_widget* parent_base, flex_direction direction);

#endif
