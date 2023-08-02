#include "../../include/widgets/button.h"
#include <stdlib.h>
#include <string.h>
#include "../../include/command_buffer.h"
#include "../../include/macros.h"

/// @brief Button states.
typedef enum button_state
{
  /// @brief Button is in normal state.
  ///        Button uses default foreground, background colors to render.
  BUTTON_NORMAL,

  /// @brief Button is hovered.
  ///        Button uses hover foreground, background colors to render.
  BUTTON_HOVERED,

  /// @brief Button is clicked.
  ///        Button uses click foreground, background colors to render.
  BUTTON_CLICKED
} button_state;

/// @brief Button widget.
struct button
{
  /// @brief Base widget for this button.
  base_widget* base;

  /// @brief X-axis padding.
  ///        padding-x = padding-left = padding-right
  uint16 padding_x;

  /// @brief Y-axis padding.
  ///        padding-y = padding-top = padding-bottom
  uint16 padding_y;

  /// @brief Foreground color of button when it is hovered.
  color hover_foreground;

  /// @brief Background color of button when it is hovered.
  color hover_background;

  /// @brief Foreground color of button when it is clicked
  ///        (or) more precise, when mouse button is down.
  color click_foreground;

  /// @brief Background color of button when it is clicked
  ///        (or) more precise, when mouse button is down.
  color click_background;

  /// @brief State of button.
  button_state state;

  /// @brief Button text.
  char* text;

  /// @brief User mouse button down callback.
  ///        This callback should be explicitly set by user.
  void (*user_mouse_button_down_callback)(button* btn,
                                          const mouse_button_event event);

  /// @brief User mouse button up callback.
  ///        This callback should be explicitly set by user.
  void (*user_mouse_button_up_callback)(button* btn,
                                        const mouse_button_event event);

  /// @brief User mouse enter callback.
  ///        This callback should be explicitly set by user.
  void (*user_mouse_enter_callback)(button* btn,
                                    const mouse_motion_event event);

  /// @brief User mouse leave callback.
  ///        This callback should be explicitly set by user.
  void (*user_mouse_leave_callback)(button* btn,
                                    const mouse_motion_event event);
};

/// @brief Default callback function for internal bounding rect callback.
/// @param widget pointer to base widget.
/// @return Returns rect struct.
static rect default_internal_get_bounding_rect_callback(base_widget* widget);

/// @brief Default callback function for internal fit layout callback.
/// @param widget pointer to base widget.
/// @return Bool result.
static result_bool default_internal_fit_layout_callback(base_widget* widget);

/// @brief Default callback function for internal render callback.
/// @param widget pointer to base widget.
/// @return Bool result.
static result_bool default_internal_render_callback(const base_widget* widget);

/// @brief Default callback function for button's mouse button down callback.
/// @param widget pointer to base widget.
/// @param event mouse button event.
/// @return Boolean.
static bool default_mouse_button_down_callback(base_widget* widget,
                                               const mouse_button_event event);

/// @brief Default callback function for button's mouse button up callback.
/// @param widget pointer to base widget.
/// @param event mouse button event.
/// @return Boolean.
static bool default_mouse_button_up_callback(base_widget* widget,
                                             const mouse_button_event event);

/// @brief Default callback function for button's mouse enter callback.
/// @param widget pointer to base widget.
/// @param event mouse motion event.
/// @return Boolean.
static bool default_mouse_enter_callback(base_widget* widget,
                                         const mouse_motion_event event);

/// @brief Default callback function for button's mouse leave callback.
/// @param widget pointer to base widget.
/// @param event mouse motion event.
/// @return Boolean.
static bool default_mouse_leave_callback(base_widget* widget,
                                         const mouse_motion_event event);

result_button_ptr button_new(const char* text)
{
  if(!text)
  {
    return error(result_button_ptr,
                 "Cannot create button with text pointing to NULL!");
  }

  button* btn = (button*)calloc(1, sizeof(button));
  if(!btn)
  {
    return error(result_button_ptr,
                 "Unable to allocate memory for button widget!");
  }

  result_base_widget_ptr _ = base_widget_new();
  if(!_.ok)
  {
    return error(result_button_ptr, _.error);
  }

  btn->base = _.value;
  btn->base->derived = btn;

  btn->base->internal_get_bounding_rect_callback =
    default_internal_get_bounding_rect_callback;
  btn->base->internal_fit_layout_callback =
    default_internal_fit_layout_callback;
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

  btn->state = BUTTON_NORMAL;

  btn->text = NULL;
  btn->text = strdup(text);
  if(!btn->text)
  {
    base_widget_free(btn->base);
    free(btn);
    return error(result_button_ptr, "Cannot copy text for button widget!");
  }

  // calling fit layout to initalize the bounding rect properly
  btn->base->internal_fit_layout_callback(btn->base);

  return ok(result_button_ptr, btn);
}

result_void button_free(button* btn)
{
  if(!btn)
  {
    return error(result_void, "Attempt to free a NULL pointing button widget!");
  }

  free(btn->text);
  result_void _ = base_widget_free(btn->base);
  if(!_.ok)
  {
    return _;
  }
  free(btn);

  return ok_void();
}

result_base_widget_ptr button_base(const button* btn)
{
  if(!btn)
  {
    return error(result_base_widget_ptr,
                 "Cannot get base widget of button pointing to NULL!");
  }

  return ok(result_base_widget_ptr, btn->base);
}

result_int16 button_get_x(const button* btn)
{
  if(!btn)
  {
    return error(result_int16,
                 "Cannot get x-coordinate of NULL pointing button widget!");
  }

  return ok(result_int16, btn->base->x);
}

result_int16 button_get_y(const button* btn)
{
  if(!btn)
  {
    return error(result_int16,
                 "Cannot get y-coordinate of NULL pointing button widget!");
  }

  return ok(result_int16, btn->base->y);
}

result_uint16 button_get_w(const button* btn)
{
  if(!btn)
  {
    return error(result_uint16,
                 "Cannot get width of NULL pointing button widget!");
  }

  return ok(result_uint16, btn->base->w);
}

result_uint16 button_get_h(const button* btn)
{
  if(!btn)
  {
    return error(result_uint16,
                 "Cannot get height of NULL pointing button widget!");
  }

  return ok(result_uint16, btn->base->h);
}

result_uint16 button_get_padding_x(const button* btn)
{
  if(!btn)
  {
    return error(result_uint16,
                 "Cannot get padding-x of NULL pointing button widget!");
  }

  return ok(result_uint16, btn->padding_x);
}

result_uint16 button_get_padding_y(const button* btn)
{
  if(!btn)
  {
    return error(result_uint16,
                 "Cannot get padding-y of NULL pointing button widget!");
  }

  return ok(result_uint16, btn->padding_y);
}

result_rect button_get_bounding_rect(const button* btn)
{
  if(!btn)
  {
    return error(result_rect,
                 "Cannot get bounding rect of NULL pointing button widget!");
  }

  rect bounding_rect = (rect){
    .x = btn->base->x, .y = btn->base->y, .w = btn->base->w, .h = btn->base->h};

  return ok(result_rect, bounding_rect);
}

result_color button_get_foreground(const button* btn)
{
  if(!btn)
  {
    return error(result_color,
                 "Cannot get foreground color of NULL pointing button widget!");
  }

  return ok(result_color, btn->base->foreground);
}

result_color button_get_background(const button* btn)
{
  if(!btn)
  {
    return error(result_color,
                 "Cannot get background color of NULL pointing button widget!");
  }

  return ok(result_color, btn->base->background);
}

result_color button_get_hover_foreground(const button* btn)
{
  if(!btn)
  {
    return error(result_color,
                 "Cannot get hover foreground of NULL pointing button widget!");
  }

  return ok(result_color, btn->hover_foreground);
}

result_color button_get_hover_background(const button* btn)
{
  if(!btn)
  {
    return error(result_color,
                 "Cannot get hover background of NULL pointing button widget!");
  }

  return ok(result_color, btn->hover_background);
}

result_color button_get_click_foreground(const button* btn)
{
  if(!btn)
  {
    return error(result_color,
                 "Cannot get click foreground of NULL pointing button widget!");
  }

  return ok(result_color, btn->click_foreground);
}

result_color button_get_click_background(const button* btn)
{
  if(!btn)
  {
    return error(result_color,
                 "Cannot get click background of NULL pointing button widget!");
  }

  return ok(result_color, btn->click_background);
}

result_const_char_ptr button_get_text(const button* btn)
{
  if(!btn)
  {
    return error(result_const_char_ptr,
                 "Cannot get text of NULL pointing button widget!");
  }

  return ok(result_const_char_ptr, (const char*)btn->text);
}

result_void button_set_x(button* btn, const int16 x)
{
  if(!btn)
  {
    return error(result_void,
                 "Cannot set x-coordinate of NULL pointing button widget!");
  }

  btn->base->x = x;

  return ok_void();
}

result_void button_set_y(button* btn, const int16 y)
{
  if(!btn)
  {
    return error(result_void,
                 "Cannot set y-coordinate of NULL pointing button widget!");
  }

  btn->base->y = y;

  return ok_void();
}

result_void button_set_padding_x(button* btn, const uint16 padding_x)
{
  if(!btn)
  {
    return error(result_void,
                 "Cannot set padding-x of NULL pointing button widget!");
  }

  btn->padding_x = padding_x;

  return ok_void();
}

result_void button_set_padding_y(button* btn, const uint16 padding_y)
{
  if(!btn)
  {
    return error(result_void,
                 "Cannot set padding-y of NULL pointing button widget!");
  }

  btn->padding_y = padding_y;

  return ok_void();
}

result_void button_set_foreground(button* btn, const color foreground)
{
  if(!btn)
  {
    return error(result_void,
                 "Cannot set foreground color of NULL pointing button widget!");
  }

  btn->base->foreground = foreground;

  return ok_void();
}

result_void button_set_background(button* btn, const color background)
{
  if(!btn)
  {
    return error(result_void,
                 "Cannot set background color of NULL pointing button widget!");
  }

  btn->base->background = background;

  return ok_void();
}

result_void button_set_hover_foreground(button* btn,
                                        const color hover_foreground)
{
  if(!btn)
  {
    return error(
      result_void,
      "Cannot set hover foreground color of NULL pointing button widget!");
  }

  btn->hover_foreground = hover_foreground;

  return ok_void();
}

result_void button_set_hover_background(button* btn,
                                        const color hover_background)
{
  if(!btn)
  {
    return error(
      result_void,
      "Cannot set hover background color of NULL pointing button widget!");
  }

  btn->hover_background = hover_background;

  return ok_void();
}

result_void button_set_click_foreground(button* btn,
                                        const color click_foreground)
{
  if(!btn)
  {
    return error(
      result_void,
      "Cannot set click foreground color of NULL pointing button widget!");
  }

  btn->click_foreground = click_foreground;

  return ok_void();
}

result_void button_set_click_background(button* btn,
                                        const color click_background)
{
  if(!btn)
  {
    return error(
      result_void,
      "Cannot set click background color of NULL pointing button widget!");
  }

  btn->click_background = click_background;

  return ok_void();
}

result_void button_set_text(button* btn, const char* text)
{
  if(!btn)
  {
    return error(result_void,
                 "Cannot set text of NULL pointing button widget!");
  }

  char* temp = NULL;
  temp = strdup(text);
  if(!text)
  {
    return error(result_void, "Unable to copy text into button widget!");
  }

  free(btn->text);
  btn->text = temp;

  btn->base->internal_fit_layout_callback(btn->base);
  btn->base->internal_render_callback(btn->base);

  return ok_void();
}

result_void button_register_mouse_down_callback(
  button* btn, void (*callback)(button*, const mouse_button_event))
{
  if(!btn)
  {
    return error(
      result_void,
      "Cannot register mouse down callback for NULL pointing button widget!");
  }

  if(!callback)
  {
    return error(
      result_void,
      "Cannot register NULL pointing mouse down callback to button widget!");
  }

  btn->user_mouse_button_down_callback = callback;

  return ok_void();
}

result_void button_register_mouse_up_callback(
  button* btn, void (*callback)(button*, const mouse_button_event))
{
  if(!btn)
  {
    return error(
      result_void,
      "Cannot register mouse up callback for NULL pointing button widget!");
  }

  if(!callback)
  {
    return error(
      result_void,
      "Cannot register NULL pointing mouse up callback to button widget!");
  }

  btn->user_mouse_button_up_callback = callback;

  return ok_void();
}

result_void button_register_mouse_enter_callback(
  button* btn, void (*callback)(button*, const mouse_motion_event))
{
  if(!btn)
  {
    return error(result_void,
                 "Cannot register mouse enter callback for NULL "
                 "pointing button widget!");
  }

  if(!callback)
  {
    return error(
      result_void,
      "Cannot register NULL pointing mouse enter callback to button widget!");
  }

  btn->user_mouse_enter_callback = callback;

  return ok_void();
}

result_void button_register_mouse_leave_callback(
  button* btn, void (*callback)(button*, const mouse_motion_event))
{
  if(!btn)
  {
    return error(result_void,
                 "Cannot register mouse leave callback for NULL "
                 "pointing button widget!");
  }

  if(!callback)
  {
    return error(
      result_void,
      "Cannot register NULL pointing mouse leave callback to button widget!");
  }

  btn->user_mouse_leave_callback = callback;

  return ok_void();
}

static rect default_internal_get_bounding_rect_callback(base_widget* widget)
{
  return (rect){.x = widget->x, .y = widget->y, .w = widget->w, .h = widget->h};
}

static result_bool default_internal_fit_layout_callback(base_widget* widget)
{
  if(!widget)
  {
    return error(result_bool,
                 "Cannot process internal fit layout callback on "
                 "a NULL pointed base widget!");
  }

  /// FIXME: button default internal fit layout callback

  // button* btn = (button*)widget->derived;

  // result_const_char_ptr _ = smoll_context_get_font(widget->context);
  // if(!_.ok)
  // {
  //   return error(result_bool, _.error);
  // }
  // const char* font = _.value;

  // result_uint8 __ = smoll_context_get_font_size(widget->context);
  // if(!__.ok)
  // {
  //   return error(result_bool, __.error);
  // }
  // uint8 font_size = __.value;

  // result_text_dimensions ___ =
  //   smoll_context_get_backend(widget->context)
  //     .value->get_text_dimensions(btn->text, font, font_size);
  // ___ = widget->context->backend->get_text_dimensions()
  // if(!___.ok)
  // {
  //   return error(result_bool, ___.error);
  // }
  // text_dimensions dimens = ___.value;

  // widget->w = dimens.w + 2 * btn->padding_x;
  // widget->h = dimens.h + 2 * btn->padding_y;

  return ok(result_bool, true);
}

static result_bool default_internal_render_callback(const base_widget* widget)
{
  if(!widget)
  {
    return error(result_bool,
                 "Cannot process internal render callback on a "
                 "NULL pointed base widget!");
  }

  button* btn = (button*)widget->derived;

  color foreground = btn->state == BUTTON_NORMAL
                       ? widget->foreground
                       : (btn->state == BUTTON_HOVERED ? btn->hover_foreground
                                                       : btn->click_foreground);
  color background = btn->state == BUTTON_NORMAL
                       ? widget->background
                       : (btn->state == BUTTON_HOVERED ? btn->hover_background
                                                       : btn->click_background);

  result_void __ = command_buffer_add_render_rect_command(
    widget->context->cmd_buffer,
    widget->internal_get_bounding_rect_callback(widget),
    background);
  if(!__.ok)
  {
    return error(result_bool, __.error);
  }

  rect text_bounding_rect = widget->internal_get_bounding_rect_callback(widget);
  text_bounding_rect.x += btn->padding_x;
  text_bounding_rect.y += btn->padding_y;
  text_bounding_rect.w -= 2 * btn->padding_x;
  text_bounding_rect.h -= 2 * btn->padding_y;

  result_void ___ =
    command_buffer_add_render_text_command(widget->context->cmd_buffer,
                                           btn->text,
                                           foreground,
                                           text_bounding_rect,
                                           background);
  if(!___.ok)
  {
    return error(result_bool, ___.error);
  }

  return ok(result_bool, true);
}

static bool default_mouse_button_down_callback(base_widget* widget,
                                               const mouse_button_event event)
{
  button* btn = (button*)widget->derived;
  btn->state = BUTTON_CLICKED;

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  // calling user registered event callback
  // maybe we should call this on another thread
  // as this will block the ui thread
  if(btn->user_mouse_button_down_callback)
  {
    btn->user_mouse_button_down_callback(btn, event);
  }

  return _.value;
}

static bool default_mouse_button_up_callback(base_widget* widget,
                                             const mouse_button_event event)
{
  button* btn = (button*)widget->derived;
  btn->state = BUTTON_HOVERED;

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  // calling user registered event callback
  // maybe we should call this on another thread
  // as this will block the ui thread
  if(btn->user_mouse_button_up_callback)
  {
    btn->user_mouse_button_up_callback(btn, event);
  }

  return _.value;
}

static bool default_mouse_enter_callback(base_widget* widget,
                                         const mouse_motion_event event)
{
  button* btn = (button*)widget->derived;
  btn->state = BUTTON_HOVERED;

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  // calling user registered event callback
  // maybe we should call this on another thread
  // as this will block the ui thread
  if(btn->user_mouse_enter_callback)
  {
    btn->user_mouse_enter_callback(btn, event);
  }

  return _.value;
}

static bool default_mouse_leave_callback(base_widget* widget,
                                         const mouse_motion_event event)
{
  button* btn = (button*)widget->derived;
  btn->state = BUTTON_NORMAL;

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  // calling user registered event callback
  // maybe we should call this on another thread
  // as this will block the ui thread
  if(btn->user_mouse_leave_callback)
  {
    btn->user_mouse_leave_callback(btn, event);
  }

  return _.value;
}