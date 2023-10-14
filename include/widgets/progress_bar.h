#ifndef SMOLL_WIDGETS__PROGRESS_BAR_H
#define SMOLL_WIDGETS__PROGRESS_BAR_H

#include "../base_widget.h"

typedef struct progress_bar_private progress_bar_private;

typedef struct progress_bar
{
  const char* debug_name;

  base_widget* base;

  progress_bar_private* private_data;
} progress_bar;

typedef struct result_progress_bar_ptr
{
  bool ok;
  union
  {
    progress_bar* value;
    const char* error;
  };
} result_progress_bar_ptr;

result_progress_bar_ptr progress_bar_new(base_widget* parent_base,
                                         uint8 percent,
                                         color foreground,
                                         color background);

result_progress_bar_ptr
progress_bar_new_with_debug_name(base_widget* parent_base,
                                 uint8 percent,
                                 color foreground,
                                 color background,
                                 const char* debug_name);

result_uint8 progress_bar_get_percent(const progress_bar* bar);

result_color progress_bar_get_foreground(const progress_bar* bar);

result_color progress_bar_get_background(const progress_bar* bar);

result_void progress_bar_set_percent(progress_bar* bar, uint8 percent);

result_void progress_bar_set_foreground(progress_bar* bar, color foreground);

result_void progress_bar_set_background(progress_bar* bar, color background);

#endif