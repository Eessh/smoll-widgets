#ifndef SMOLL_WIDGETS__BOX_H
#define SMOLL_WIDGETS__BOX_H

#include "../base_widget.h"

typedef struct box box;

struct box
{
  uint16 padding_x;
  uint16 padding_y;

  color background;

  base_widget* base;
};

typedef struct result_box_ptr
{
  bool ok;
  union
  {
    box* value;
    const char* error;
  };
} result_box_ptr;

result_box_ptr box_new(base_widget* parent_base);

#endif