#ifndef SMOLL_WIDGETS__VIEW_H
#define SMOLL_WIDGETS__VIEW_H

#include "../base_widget.h"

typedef enum view_layout_type
{
  FLEX_ROW,
  FLEX_COL,
  FLEX_ROW_WRAP,
  FLEX_COL_WRAP
} view_layout_type;

typedef struct view_private view_private;

typedef struct view
{
  base_widget* base;

  view_private* private_data;
} view;

typedef struct result_view_ptr
{
  bool ok;
  union
  {
    view* value;
    const char* error;
  };
} result_view_ptr;

result_view_ptr view_new(base_widget* parent_base,
                         view_layout_type layout_type);

result_void view_set_layout_type(view* v, view_layout_type layout_type);

#endif