#include "../../include/widgets/button.h"
#include <stdlib.h>
#include <string.h>
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
struct button_private
{
  /// @brief State of button.
  button_state state;

  /// @brief Button text.
  char* text;

  /// @brief User mouse button down callback.
  ///        This callback should be explicitly set by user.
  void (*user_mouse_button_down_callback)(button* btn,
                                          mouse_button_event event);

  /// @brief User mouse button up callback.
  ///        This callback should be explicitly set by user.
  void (*user_mouse_button_up_callback)(button* btn, mouse_button_event event);

  /// @brief User mouse enter callback.
  ///        This callback should be explicitly set by user.
  void (*user_mouse_enter_callback)(button* btn, mouse_motion_event event);

  /// @brief User mouse leave callback.
  ///        This callback should be explicitly set by user.
  void (*user_mouse_leave_callback)(button* btn, mouse_motion_event event);
};

/// @brief Default callback function for internal bounding rect callback.
/// @param widget pointer to base widget.
/// @return Returns rect struct.
static rect
default_internal_get_bounding_rect_callback(const base_widget* widget);

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
                                               mouse_button_event event);

/// @brief Default callback function for button's mouse button up callback.
/// @param widget pointer to base widget.
/// @param event mouse button event.
/// @return Boolean.
static bool default_mouse_button_up_callback(base_widget* widget,
                                             mouse_button_event event);

/// @brief Default callback function for button's mouse enter callback.
/// @param widget pointer to base widget.
/// @param event mouse motion event.
/// @return Boolean.
static bool default_mouse_enter_callback(base_widget* widget,
                                         mouse_motion_event event);

/// @brief Default callback function for button's mouse leave callback.
/// @param widget pointer to base widget.
/// @param event mouse motion event.
/// @return Boolean.
static bool default_mouse_leave_callback(base_widget* widget,
                                         mouse_motion_event event);

result_button_ptr button_new(base_widget* parent_base, const char* text)
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
    free(btn);
    return error(result_button_ptr, _.error);
  }

  button_private* btn_private =
    (button_private*)calloc(1, sizeof(button_private));
  if(!btn_private)
  {
    base_widget_free(_.value);
    free(btn);
    return error(result_button_ptr,
                 "Unable to allocate memory for private fields of button!");
  }

  btn->base = _.value;
  btn->base->derived = btn;
  btn->base->parent = parent_base;

  btn->private = btn_private;

  // assigning context
  if(parent_base)
  {
    btn->base->context = parent_base->context;
  }

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

  btn->private->state = BUTTON_NORMAL;

  btn->private->text = NULL;
  btn->private->text = _strdup(text);
  if(!btn->private->text)
  {
    base_widget_free(btn->base);
    free(btn);
    return error(result_button_ptr, "Cannot copy text for button widget!");
  }

  if(parent_base)
  {
    // calling fit layout to initialize the bounding rect properly
    btn->base->internal_fit_layout_callback(btn->base);
  }

  return ok(result_button_ptr, btn);
}

result_void button_free(button* btn)
{
  if(!btn)
  {
    return error(result_void, "Attempt to free a NULL pointing button widget!");
  }

  // freeing button text
  free(btn->private->text);

  // freeing button private fields
  free(btn->private);

  // freeing button base widget
  result_void _ = base_widget_free(btn->base);
  if(!_.ok)
  {
    return _;
  }

  free(btn);

  return ok_void();
}

result_const_char_ptr button_get_text(const button* btn)
{
  if(!btn)
  {
    return error(result_const_char_ptr,
                 "Cannot get text of NULL pointing button widget!");
  }

  return ok(result_const_char_ptr, (const char*)btn->private->text);
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

  free(btn->private->text);
  btn->private->text = temp;

  btn->base->internal_fit_layout_callback(btn->base);
  btn->base->internal_render_callback(btn->base);

  return ok_void();
}

result_void button_register_mouse_down_callback(
  button* btn, void (*callback)(button*, mouse_button_event))
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

  btn->private->user_mouse_button_down_callback = callback;

  return ok_void();
}

result_void
button_register_mouse_up_callback(button* btn,
                                  void (*callback)(button*, mouse_button_event))
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

  btn->private->user_mouse_button_up_callback = callback;

  return ok_void();
}

result_void button_register_mouse_enter_callback(
  button* btn, void (*callback)(button*, mouse_motion_event))
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

  btn->private->user_mouse_enter_callback = callback;

  return ok_void();
}

result_void button_register_mouse_leave_callback(
  button* btn, void (*callback)(button*, mouse_motion_event))
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

  btn->private->user_mouse_leave_callback = callback;

  return ok_void();
}

static rect
default_internal_get_bounding_rect_callback(const base_widget* widget)
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

  button* btn = (button*)widget->derived;

  result_text_dimensions ___ = widget->context->backend->get_text_dimensions(
    btn->private->text, widget->context->font, widget->context->font_size);
  if(!___.ok)
  {
    return error(result_bool, ___.error);
  }
  text_dimensions dimensions = ___.value;

  widget->w = dimensions.w + 2 * btn->padding_x;
  widget->h = dimensions.h + 2 * btn->padding_y;

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

  color foreground =
    btn->private->state == BUTTON_NORMAL
      ? btn->foreground
      : (btn->private->state == BUTTON_HOVERED ? btn->hover_foreground
                                               : btn->click_foreground);
  color background =
    btn->private->state == BUTTON_NORMAL
      ? btn->background
      : (btn->private->state == BUTTON_HOVERED ? btn->hover_background
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
                                           btn->private->text,
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
                                               mouse_button_event event)
{
  button* btn = (button*)widget->derived;
  btn->private->state = BUTTON_CLICKED;

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  // calling user registered event callback
  // maybe we should call this on another thread
  // as this will block the ui thread
  if(btn->private->user_mouse_button_down_callback)
  {
    btn->private->user_mouse_button_down_callback(btn, event);
  }

  return _.value;
}

static bool default_mouse_button_up_callback(base_widget* widget,
                                             mouse_button_event event)
{
  button* btn = (button*)widget->derived;
  btn->private->state = BUTTON_HOVERED;

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  // calling user registered event callback
  // maybe we should call this on another thread
  // as this will block the ui thread
  if(btn->private->user_mouse_button_up_callback)
  {
    btn->private->user_mouse_button_up_callback(btn, event);
  }

  return _.value;
}

static bool default_mouse_enter_callback(base_widget* widget,
                                         mouse_motion_event event)
{
  button* btn = (button*)widget->derived;
  btn->private->state = BUTTON_HOVERED;

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  // calling user registered event callback
  // maybe we should call this on another thread
  // as this will block the ui thread
  if(btn->private->user_mouse_enter_callback)
  {
    btn->private->user_mouse_enter_callback(btn, event);
  }

  return _.value;
}

static bool default_mouse_leave_callback(base_widget* widget,
                                         mouse_motion_event event)
{
  button* btn = (button*)widget->derived;
  btn->private->state = BUTTON_NORMAL;

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  // calling user registered event callback
  // maybe we should call this on another thread
  // as this will block the ui thread
  if(btn->private->user_mouse_leave_callback)
  {
    btn->private->user_mouse_leave_callback(btn, event);
  }

  return _.value;
}