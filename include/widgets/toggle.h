#ifndef SMOLL_WIDGETS__TOGGLE_H
#define SMOLL_WIDGETS__TOGGLE_H

#include "../base_widget.h"

typedef struct toggle toggle;

typedef struct toggle_private toggle_private;

struct toggle
{
  float32 handle_width_fraction;

  uint16 padding_x;

  uint16 padding_y;

  color on_handle_color;

  color on_background;

  color off_handle_color;

  color off_background;

  base_widget* base;

  toggle_private* private;
};

typedef struct result_toggle_ptr
{
  bool ok;
  union
  {
    toggle* value;
    const char* error;
  };
} result_toggle_ptr;

result_toggle_ptr toggle_new(base_widget* parent_base);

result_void toggle_set_on_callback(toggle* t, void (*callback)(toggle*));

result_void toggle_set_off_callback(toggle* t, void (*callback)(toggle*));

#endif