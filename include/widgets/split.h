#ifndef SMOLL_WIDGETS__SPLIT_H
#define SMOLL_WIDGETS__SPLIT_H

#include "../base_widget.h"

typedef enum split_direction
{
  SPLIT_DIRECTION_HORIZONTAL,
  SPLIT_DIRECTION_VERTICAL
} split_direction;

typedef struct split_private split_private;

typedef struct split
{
  split_direction direction;
  color split_color, split_hover_color, split_click_color;

  base_widget* base;

  split_private* private_data;
} split;

typedef struct result_split_ptr
{
  bool ok;
  union
  {
    split* value;
    const char* error;
  };
} result_split_ptr;

result_split_ptr split_new(base_widget* parent_base,
                           split_direction direction,
                           base_widget* first_widget,
                           base_widget* second_widget);

result_void split_set_size(split* split_widget, uint8 size);

#endif