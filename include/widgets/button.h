#ifndef SMOLL_WIDGETS__BUTTON_H
#define SMOLL_WIDGETS__BUTTON_H

#include "../base_widget.h"

typedef struct button button;

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
result_button_ptr button_new(const char* text);
result_void button_free(button* btn);

// Base widget
result_base_widget_ptr button_base(const button* btn);

// Getters
result_int16 button_get_x(const button* btn);
result_int16 button_get_y(const button* btn);
result_uint16 button_get_w(const button* btn);
result_uint16 button_get_h(const button* btn);
result_uint16 button_get_padding_x(const button* btn);
result_uint16 button_get_padding_y(const button* btn);
result_rect button_get_bounding_rect(const button* btn);
result_color button_get_foreground(const button* btn);
result_color button_get_background(const button* btn);
result_color button_get_hover_foreground(const button* btn);
result_color button_get_hover_background(const button* btn);
result_color button_get_click_foreground(const button* btn);
result_color button_get_click_background(const button* btn);
result_const_char_ptr button_get_text(const button* btn);

// Setters
result_void button_set_x(button* btn, int16 x);
result_void button_set_y(button* btn, int16 y);
result_void button_set_padding_x(button* btn, uint16 padding_x);
result_void button_set_padding_y(button* btn, uint16 padding_y);
result_void button_set_foreground(button* btn, color foreground);
result_void button_set_background(button* btn, color background);
result_void button_set_hover_foreground(button* btn, color hover_foreground);
result_void button_set_hover_background(button* btn, color hover_background);
result_void button_set_click_foreground(button* btn, color click_foreground);
result_void button_set_click_background(button* btn, color click_background);
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