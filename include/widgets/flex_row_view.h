#ifndef SMOLL_WIDGETS__FLEX_ROW_VIEW_H
#define SMOLL_WIDGETS__FLEX_ROW_VIEW_H

#include "../base_widget.h"

typedef struct flex_row_view
{
  uint16 padding_x;

  uint16 padding_y;

  uint16 gap;

  color background;

  base_widget* base;
} flex_row_view;

typedef struct result_flex_row_view
{
  bool ok;
  union
  {
    flex_row_view* value;
    const char* error;
  };
} result_flex_row_view;

result_flex_row_view flex_row_view_new(base_widget* parent_base);

#endif