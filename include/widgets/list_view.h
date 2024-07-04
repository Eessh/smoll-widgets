#ifndef SMOLL_WIDGETS__LIST_VIEW
#define SMOLL_WIDGETS__LIST_VIEW

#include "../base_widget.h"

typedef struct list_view_private list_view_private;

typedef struct list_view
{
  color background;

  base_widget* base;

  list_view_private* private_data;
} list_view;

typedef struct result_list_view_ptr
{
  bool ok;
  union
  {
    list_view* value;
    const char* error;
  };
} result_list_view_ptr;

result_list_view_ptr list_view_new(base_widget* parent_base);

result_list_view_ptr list_view_new_with_debug_name(base_widget* parent_base,
                                                   const char* debug_name);

result_float32 list_view_get_scroll_offset(const list_view* view);

result_bool list_view_set_scroll_offset(list_view* view,
                                        const float32* new_scroll_offset);

#endif