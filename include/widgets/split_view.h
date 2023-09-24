#ifndef SMOLL_WIDGETS__SPLIT_VIEW_H
#define SMOLL_WIDGETS__SPLIT_VIEW_H

#include "../base_widget.h"

typedef enum split_type
{
  SPLIT_HORIZONTAL,
  SPLIT_VERTICAL
} split_type;

typedef struct split_view_private split_view_private;

typedef struct split_view
{
  split_type type;

  uint8 handle_size;
  color handle_color, handle_hover_color, handle_click_color;

  base_widget* base;

  split_view_private* private_data;
} split_view;

typedef struct result_split_view_ptr
{
  bool ok;
  union
  {
    split_view* value;
    const char* error;
  };
} result_split_view_ptr;

result_split_view_ptr split_view_new(base_widget* parent_base, split_type type);

result_void split_view_connect_children(split_view* view,
                                        base_widget* first_child,
                                        base_widget* second_child);

#endif // SMOLL_WIDGETS__SPLIT_VIEW_H