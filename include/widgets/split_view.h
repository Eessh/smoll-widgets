#ifndef SMOLL_WIDGETS__SPLIT_VIEW_H
#define SMOLL_WIDGETS__SPLIT_VIEW_H

#include "../base_widget.h"

/// Type of `split_view` either horizontal or vertical.
typedef enum split_type
{
  /// Splitter is horizontal, resizes height of both views.
  SPLIT_HORIZONTAL,

  /// Splitter is vertical, resizes width of both views.
  SPLIT_VERTICAL
} split_type;

/// Split-View widget.
typedef struct split_view
{
  /// Type of split either horizontal or vertical.
  split_type type;

  /// Splitter handle size in pixels.
  uint8 handle_size;

  /// Splitter handle color.
  color handle_color;

  /// Splitter handle hover-color.
  color handle_hover_color;

  /// Splitter handle click-color.
  color handle_click_color;

  /// Pointer to base widget.
  base_widget* base;
} split_view;

/// Split-View widget result.
typedef struct result_split_view_ptr
{
  bool ok;
  union
  {
    split_view* value;
    const char* error;
  };
} result_split_view_ptr;

/// Creates a new split_view widget with given split type.
result_split_view_ptr split_view_new(base_widget* parent_base, split_type type);

/// Connects the children to the split views.
result_void split_view_connect_children(split_view* view,
                                        base_widget* first_child,
                                        base_widget* second_child);

#endif // SMOLL_WIDGETS__SPLIT_VIEW_H
