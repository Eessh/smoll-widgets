#ifndef SMOLL_WIDGETS__FLEX_CLOUMN_VIEW_H
#define SMOLL_WIDGETS__FLEX_CLOUMN_VIEW_H

#include "../base_widget.h"

typedef struct flex_column_view
{
  uint16 padding_x;

  uint16 padding_y;

  uint16 gap;

  color background;

  base_widget* base;
} flex_column_view;

typedef struct result_flex_column_view
{
  bool ok;
  union
  {
    flex_column_view* value;
    const char* error;
  };
} result_flex_column_view;

result_flex_column_view flex_column_view_new(base_widget* parent_base);

#endif