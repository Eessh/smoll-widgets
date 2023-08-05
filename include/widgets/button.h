#ifndef SMOLL_WIDGETS__BUTTON_H
#define SMOLL_WIDGETS__BUTTON_H

#include "../base_widget.h"

typedef struct button button;
typedef struct button_private button_private;

struct button
{
  uint16 padding_x;
  uint16 padding_y;
  color foreground;
  color background;
  color hover_foreground;
  color hover_background;
  color click_foreground;
  color click_background;

  base_widget* base;

  button_private* private;
};

typedef struct result_button_ptr
{
  bool ok;
  union
  {
    button* value;
    const char* error;
  };
} result_button_ptr;

// Constructor & Destructors
result_button_ptr button_new(base_widget* parent_base, const char* text);
result_void button_free(button* btn);

// Getters
result_const_char_ptr button_get_text(const button* btn);

// Setters
result_void button_set_text(button* btn, const char* text);

// Callback registerers
result_void button_register_mouse_down_callback(
  button* btn, void (*callback)(button*, mouse_button_event));
result_void button_register_mouse_up_callback(
  button* btn, void (*callback)(button*, mouse_button_event));
result_void button_register_mouse_enter_callback(
  button* btn, void (*callback)(button*, mouse_motion_event));
result_void button_register_mouse_leave_callback(
  button* btn, void (*callback)(button*, mouse_motion_event));

#endif