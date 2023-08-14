#ifndef SMOLL_WIDGETS__PROGRESS_BAR_H
#define SMOLL_WIDGETS__PROGRESS_BAR_H

#include "../base_widget.h"

typedef struct progress_bar_private progress_bar_private;

typedef struct progress_bar
{
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

result_void progress_bar_set_percent(progress_bar* bar, uint8 percent);

result_void progress_bar_set_foreground(progress_bar* bar, color foreground);

result_void progress_bar_set_background(progress_bar* bar, color background);

#endif