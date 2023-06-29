#include "../../include/widgets/button.h"
#include "../../include/backend_api.h"
#include "../../include/smoll_context.h"
#include "../../include/command_buffer.h"
#include <stdlib.h>
#include <string.h>

struct button {
  base_widget* base;

  // paddings
  uint16 padding_x, padding_y;

  // hover & click colors
  color hover_foreground, hover_background;
  color click_foreground, click_background;

  // render colors
  color render_foreground, render_background;

  // button text
  char* text;

  // user callbacks
  void (*user_mouse_button_down_callback)(button* btn, const mouse_button_event event);
  void (*user_mouse_button_up_callback)(button* btn, const mouse_button_event event);
  void (*user_mouse_enter_callback)(button* btn, const mouse_motion_event event);
  void (*user_mouse_leave_callback)(button* btn, const mouse_motion_event event);
};

static rect default_internal_get_bounding_rect_callback(base_widget* widget);
static result_bool default_internal_fit_layout_callback(base_widget* widget);
static result_bool default_internal_render_callback(const base_widget* widget);
static bool default_mouse_button_down_callback(base_widget* widget, const mouse_button_event event);
static bool default_mouse_button_up_callback(base_widget* widget, const mouse_button_event event);
static bool default_mouse_enter_callback(base_widget* widget, const mouse_motion_event event);
static bool default_mouse_leave_callback(base_widget* widget, const mouse_motion_event event);

result_button_ptr button_new(smoll_context* context, const char* text) {
  if (!text) {
    return error(result_button_ptr, "Cannot create button with text pointing to NULL!");
  }

  button* btn = (button*)calloc(1, sizeof(button));
  if (!btn) {
    return error(result_button_ptr, "Unable to allocate memory for button widget!");
  }

  result_base_widget_ptr _ = base_widget_new(context);
  if (!_.ok) {
    return error(result_button_ptr, _.error);
  }

  btn->base = _.value;
  btn->base->derived = btn;

  btn->base->internal_get_bounding_rect_callback = default_internal_get_bounding_rect_callback;
  btn->base->internal_fit_layout_callback = default_internal_fit_layout_callback;
  btn->base->internal_render_callback = default_internal_render_callback;

  btn->base->mouse_button_down_callback = default_mouse_button_down_callback;
  btn->base->mouse_button_up_callback = default_mouse_button_up_callback;
  btn->base->mouse_enter_callback = default_mouse_enter_callback;
  btn->base->mouse_leave_callback = default_mouse_leave_callback;

  btn->padding_x = 0;
  btn->padding_y = 0;

  btn->hover_foreground = (color){255, 255, 255, 255};
  btn->hover_background = (color){0, 0, 0, 255};

  btn->click_foreground = (color){255, 255, 255, 255};
  btn->click_background = (color){0, 0, 0, 255};

  btn->render_foreground = btn->base->foreground;
  btn->render_background = btn->base->background;

  btn->text = NULL;
  btn->text = strdup(text);
  if (!btn->text) {
    base_widget_free(btn->base);
    free(btn);
    return error(result_button_ptr, "Cannot copy text for button widget!");
  }

  // calling fit layout to initalize the bounding rect properly
  btn->base->internal_fit_layout_callback(btn->base);

  return ok(result_button_ptr, btn);
}

result_void button_free(button* btn) {
  if (!btn) {
    return error(result_void, "Attempt to free a NULL pointing button widget!");
  }

  free(btn->text);
  result_void _ = base_widget_free(btn->base);
  if (!_.ok) {
    return _;
  }
  free(btn);

  return ok_void();
}

result_int16 button_get_x(const button* btn) {
  if (!btn) {
    return error(result_int16, "Cannot get x-coordinate of NULL pointing button widget!");
  }

  return ok(result_int16, btn->base->x);
}

result_int16 button_get_y(const button* btn) {
  if (!btn) {
    return error(result_int16, "Cannot get y-coordinate of NULL pointing button widget!");
  }

  return ok(result_int16, btn->base->y);
}

result_uint16 button_get_w(const button* btn) {
  if (!btn) {
    return error(result_uint16, "Cannot get width of NULL pointing button widget!");
  }

  return ok(result_uint16, btn->base->w);
}

result_uint16 button_get_h(const button* btn) {
  if (!btn) {
    return error(result_uint16, "Cannot get height of NULL pointing button widget!");
  }

  return ok(result_uint16, btn->base->h);
}

result_uint16 button_get_padding_x(const button* btn) {
  if (!btn) {
    return error(result_uint16, "Cannot get padding-x of NULL pointing button widget!");
  }

  return ok(result_uint16, btn->padding_x);
}

result_uint16 button_get_padding_y(const button* btn) {
  if (!btn) {
    return error(result_uint16, "Cannot get padding-y of NULL pointing button widget!");
  }

  return ok(result_uint16, btn->padding_y);
}

result_rect button_get_bounding_rect(const button* btn) {
  if (!btn) {
    return error(result_rect, "Cannot get bounding rect of NULL pointing button widget!");
  }

  rect bounding_rect = (rect) {
    .x = btn->base->x,
    .y = btn->base->y,
    .w = btn->base->w,
    .h = btn->base->h
  };

  return ok(result_rect, bounding_rect);
}

result_color button_get_foreground(const button* btn) {
  if (!btn) {
    return error(result_color, "Cannot get foreground color of NULL pointing button widget!");
  }

  return ok(result_color, btn->base->foreground);
}

result_color button_get_background(const button* btn) {
  if (!btn) {
    return error(result_color, "Cannot get background color of NULL pointing button widget!");
  }

  return ok(result_color, btn->base->background);
}

result_color button_get_hover_foreground(const button* btn) {
  if (!btn) {
    return error(result_color, "Cannot get hover foreground of NULL pointing button widget!");
  }

  return ok(result_color, btn->hover_foreground);
}

result_color button_get_hover_background(const button* btn) {
  if (!btn) {
    return error(result_color, "Cannot get hover background of NULL pointing button widget!");
  }

  return ok(result_color, btn->hover_background);
}

result_color button_get_click_foreground(const button* btn) {
  if (!btn) {
    return error(result_color, "Cannot get click foreground of NULL pointing button widget!");
  }

  return ok(result_color, btn->click_foreground);
}

result_color button_get_click_background(const button* btn) {
  if (!btn) {
    return error(result_color, "Cannot get click background of NULL pointing button widget!");
  }

  return ok(result_color, btn->click_background);
}

result_const_char_ptr button_get_text(const button* btn) {
  if (!btn) {
    return error(result_const_char_ptr, "Cannot get text of NULL pointing button widget!");
  }

  return ok(result_const_char_ptr, (const char*)btn->text);
}

result_void button_set_x(button* btn, const int16 x) {
  if (!btn) {
    return error(result_void, "Cannot set x-coordinate of NULL pointing button widget!");
  }

  btn->base->x = x;

  return ok_void();
}

result_void button_set_y(button* btn, const int16 y) {
  if (!btn) {
    return error(result_void, "Cannot set y-coordinate of NULL pointing button widget!");
  }

  btn->base->y = y;

  return ok_void();
}

result_void button_set_padding_x(button* btn, const uint16 padding_x) {
  if (!btn) {
    return error(result_void, "Cannot set padding-x of NULL pointing button widget!");
  }

  btn->padding_x = padding_x;

  return ok_void();
}

result_void button_set_padding_y(button* btn, const uint16 padding_y) {
  if (!btn) {
    return error(result_void, "Cannot set padding-y of NULL pointing button widget!");
  }

  btn->padding_y = padding_y;

  return ok_void();
}

result_void button_set_foreground(button* btn, const color foreground) {
  if (!btn) {
    return error(result_void, "Cannot set foreground color of NULL pointing button widget!");
  }

  btn->base->foreground = foreground;

  return ok_void();
}

result_void button_set_background(button* btn, const color background) {
  if (!btn) {
    return error(result_void, "Cannot set background color of NULL pointing button widget!");
  }

  btn->base->background = background;

  return ok_void();
}

result_void button_set_hover_foreground(button* btn, const color hover_foreground) {
  if (!btn) {
    return error(result_void, "Cannot set hover foreground color of NULL pointing button widget!");
  }

  btn->hover_foreground = hover_foreground;

  return ok_void();
}

result_void button_set_hover_background(button* btn, const color hover_background) {
  if (!btn) {
    return error(result_void, "Cannot set hover background color of NULL pointing button widget!");
  }

  btn->hover_background = hover_background;

  return ok_void();
}

result_void button_set_click_foreground(button* btn, const color click_foreground) {
  if (!btn) {
    return error(result_void, "Cannot set click foreground color of NULL pointing button widget!");
  }

  btn->click_foreground = click_foreground;

  return ok_void();
}

result_void button_set_click_background(button* btn, const color click_background) {
  if (!btn) {
    return error(result_void, "Cannot set click background color of NULL pointing button widget!");
  }

  btn->click_background = click_background;

  return ok_void();
}

result_void button_set_text(button* btn, const char* text) {
  if (!btn) {
    return error(result_void, "Cannot set text of NULL pointing button widget!");
  }

  char* temp = NULL;
  temp = strdup(text);
  if (!text) {
    return error(result_void, "Unable to copy text into button widget!");
  }

  free(btn->text);
  btn->text = temp;

  btn->base->internal_fit_layout_callback(btn->base);
  btn->base->internal_render_callback(btn->base);

  return ok_void();
}

result_void button_register_mouse_down_callback(button* btn, void (*callback)(button*, const mouse_button_event)) {
  if (!btn) {
    return error(result_void, "Cannot register mouse down callback for NULL pointing button widget!");
  }

  if (!callback) {
    return error(result_void, "Cannot register NULL pointing mouse down callback to button widget!");
  }

  btn->user_mouse_button_down_callback = callback;

  return ok_void();
}

result_void button_register_mouse_up_callback(button* btn, void (*callback)(button*, const mouse_button_event)) {
  if (!btn) {
    return error(result_void, "Cannot register mouse up callback for NULL pointing button widget!");
  }

  if (!callback) {
    return error(result_void, "Cannot register NULL pointing mouse up callback to button widget!");
  }

  btn->user_mouse_button_up_callback = callback;

  return ok_void();
}

result_void button_register_mouse_enter_callback(button* btn, void (*callback)(button*, const mouse_motion_event)) {
  if (!btn) {
    return error(result_void, "Cannot register mouse enter callback for NULL pointing button widget!");
  }

  if (!callback) {
    return error(result_void, "Cannot register NULL pointing mouse enter callback to button widget!");
  }

  btn->user_mouse_enter_callback = callback;

  return ok_void();
}

result_void button_register_mouse_leave_callback(button* btn, void (*callback)(button*, const mouse_motion_event)) {
  if (!btn) {
    return error(result_void, "Cannot register mouse leave callback for NULL pointing button widget!");
  }

  if (!callback) {
    return error(result_void, "Cannot register NULL pointing mouse leave callback to button widget!");
  }

  btn->user_mouse_leave_callback = callback;

  return ok_void();
}

static rect default_internal_get_bounding_rect_callback(base_widget* widget) {
  return (rect) {
    .x = widget->x,
    .y = widget->y,
    .w = widget->w,
    .h = widget->h
  };
}

static result_bool default_internal_fit_layout_callback(base_widget* widget) {
  if (!widget) {
    return error(result_bool, "Cannot process internal fit layout callback on a NULL pointed base widget!");
  }

  button* btn = (button*)widget->derived;

  result_const_char_ptr _ = smoll_context_get_font(widget->context);
  if (!_.ok) {
    return error(result_bool, _.error);
  }
  const char* font = _.value;

  result_uint8 __ = smoll_context_get_font_size(widget->context);
  if (!__.ok) {
    return error(result_bool, __.error);
  }
  uint8 font_size = __.value;

  result_text_dimensions ___ = backend_api_get_text_dimensions(btn->text, font, font_size);
  if (!___.ok) {
    return error(result_bool, ___.error);
  }
  text_dimensions dimens = ___.value;

  widget->w = dimens.w + 2*btn->padding_x;
  widget->h = dimens.h + 2*btn->padding_y;

  return ok(result_bool, true);
}

static result_bool default_internal_render_callback(const base_widget* widget) {
  if (!widget) {
    return error(result_bool, "Cannot process internal render callback on a NULL pointed base widget!");
  }

  button* btn = (button*)widget->derived;

  result_command_buffer_ptr _ = smoll_context_get_command_buffer(widget->context);
  if (!_.ok) {
    return error(result_bool, _.error);
  }
  command_buffer* cmd_buffer = _.value;

  result_void __ = command_buffer_add_render_rect_command(cmd_buffer, widget->internal_get_bounding_rect_callback(widget), btn->render_background);
  if (!__.ok) {
    return error(result_bool, __.error);
  }

  rect text_bounding_rect = widget->internal_get_bounding_rect_callback(widget);
  text_bounding_rect.x += btn->padding_x;
  text_bounding_rect.y += btn->padding_y;
  text_bounding_rect.w -= 2*btn->padding_x;
  text_bounding_rect.h -= 2*btn->padding_y;

  result_void ___ = command_buffer_add_render_text_command(cmd_buffer, btn->text, btn->render_foreground, text_bounding_rect, btn->render_background);
  if (!___.ok) {
    return error(result_bool, ___.error);
  }

  return ok(result_bool, true);
}

static bool default_mouse_button_down_callback(base_widget* widget, const mouse_button_event event) {
  button* btn = (button*)widget->derived;
  btn->render_foreground = btn->click_foreground;
  btn->render_background = btn->click_background;

  result_bool _ = widget->internal_render_callback(widget);
  if (!_.ok) {
    return false;
  }

  // calling user registered event callback
  // maybe we should call this on another thread
  // as this will block the ui thread
  if (btn->user_mouse_button_down_callback) {
    btn->user_mouse_button_down_callback(btn, event);
  }

  return _.value;
}

static bool default_mouse_button_up_callback(base_widget* widget, const mouse_button_event event) {
  button* btn = (button*)widget->derived;
  btn->render_foreground = widget->foreground;
  btn->render_background = widget->background;

  result_bool _ = widget->internal_render_callback(widget);
  if (!_.ok) {
    return false;
  }

  // calling user registered event callback
  // maybe we should call this on another thread
  // as this will block the ui thread
  if (btn->user_mouse_button_up_callback) {
    btn->user_mouse_button_up_callback(btn, event);
  }

  return _.value;
}

static bool default_mouse_enter_callback(base_widget* widget, const mouse_motion_event event) {
  button* btn = (button*)widget->derived;
  btn->render_foreground = btn->hover_foreground;
  btn->render_background = btn->hover_background;

  result_bool _ = widget->internal_render_callback(widget);
  if (!_.ok) {
    return false;
  }

  // calling user registered event callback
  // maybe we should call this on another thread
  // as this will block the ui thread
  if (btn->user_mouse_enter_callback) {
    btn->user_mouse_enter_callback(btn, event);
  }

  return _.value;
}

static bool default_mouse_leave_callback(base_widget* widget, const mouse_motion_event event) {
  button* btn = (button*)widget->derived;
  btn->render_foreground = widget->foreground;
  btn->render_background = widget->background;

  result_bool _ = widget->internal_render_callback(widget);
  if (!_.ok) {
    return false;
  }

  // calling user registered event callback
  // maybe we should call this on another thread
  // as this will block the ui thread
  if (btn->user_mouse_leave_callback) {
    btn->user_mouse_leave_callback(btn, event);
  }

  return _.value;
}