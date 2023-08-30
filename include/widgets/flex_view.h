#ifndef SMOLL_WIDGETS__FLEX_VIEW_H
#define SMOLL_WIDGETS__FLEX_VIEW_H

#include "../base_widget.h"

typedef struct flex_view
{
  color background;

  base_widget* base;
} flex_view;

typedef struct result_flex_view_ptr
{
  bool ok;
  union
  {
    flex_view* value;
    const char* error;
  };
} result_flex_view_ptr;

result_flex_view_ptr flex_view_new(base_widget* parent_base,
                                   flex_direction direction);

#endif // SMOLL_WIDGETS__FLEX_VIEW_H
