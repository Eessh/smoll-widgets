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

/// @brief Button widget private data.
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

/// @brief Default callback function for internal derived free callback.
/// @param widget pointer to base widget.
/// @return Void.
static void default_internal_derived_free_callback(base_widget* widget);

/// @brief Default callback function for internal fit layout callback.
/// @param widget pointer to base widget.
/// @return Bool result.
static result_sizing_delta
default_internal_fit_layout_callback(base_widget* widget,
                                     bool call_on_children);

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

  result_base_widget_ptr _ = base_widget_new(FLEX_ITEM);
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

  btn->private_data = btn_private;

  // attaching this button to parent's children list
  if(parent_base)
  {
    base_widget_add_child(parent_base, btn->base);
  }

  // assigning context
  if(parent_base)
  {
    btn->base->context = parent_base->context;
  }

  btn->base->internal_fit_layout_callback =
    default_internal_fit_layout_callback;
  btn->base->internal_render_callback = default_internal_render_callback;

  btn->base->internal_derived_free_callback =
    default_internal_derived_free_callback;

  btn->base->mouse_button_down_callback = default_mouse_button_down_callback;
  btn->base->mouse_button_up_callback = default_mouse_button_up_callback;
  btn->base->mouse_enter_callback = default_mouse_enter_callback;
  btn->base->mouse_leave_callback = default_mouse_leave_callback;

  btn->padding_x = 0;
  btn->padding_y = 0;

  btn->border_radius = 0;

  btn->hover_foreground = (color){255, 255, 255, 255};
  btn->hover_background = (color){0, 0, 0, 255};

  btn->click_foreground = (color){255, 255, 255, 255};
  btn->click_background = (color){0, 0, 0, 255};

  btn->private_data->state = BUTTON_NORMAL;

  btn->private_data->text = NULL;
  btn->private_data->text = strdup(text);
  if(!btn->private_data->text)
  {
    base_widget_free(btn->base);
    free(btn);
    return error(result_button_ptr, "Cannot copy text for button widget!");
  }

  return ok(result_button_ptr, btn);
}

result_button_ptr button_new_with_debug_name(base_widget* parent_base,
                                             const char* text,
                                             const char* debug_name)
{
  result_button_ptr _ = button_new(parent_base, text);
  if(!_.ok)
  {
    return _;
  }

  _.value->base->debug_name = debug_name;

  trace("Button: created with debug-name: %s", debug_name);

  return _;
}

result_const_char_ptr button_get_text(const button* btn)
{
  if(!btn)
  {
    return error(result_const_char_ptr,
                 "Cannot get text of NULL pointing button widget!");
  }

  return ok(result_const_char_ptr, (const char*)btn->private_data->text);
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

  free(btn->private_data->text);
  btn->private_data->text = temp;

  // btn->base->internal_fit_layout_callback(btn->base, false);
  // btn->base->internal_render_callback(btn->base);
  common_internal_adjust_layout(btn->base);

  return ok_void();
}

result_void button_set_mouse_down_callback(button* btn,
                                           void (*callback)(button*,
                                                            mouse_button_event))
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

  btn->private_data->user_mouse_button_down_callback = callback;

  return ok_void();
}

result_void button_set_mouse_up_callback(button* btn,
                                         void (*callback)(button*,
                                                          mouse_button_event))
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

  btn->private_data->user_mouse_button_up_callback = callback;

  return ok_void();
}

result_void
button_set_mouse_enter_callback(button* btn,
                                void (*callback)(button*, mouse_motion_event))
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

  btn->private_data->user_mouse_enter_callback = callback;

  return ok_void();
}

result_void
button_set_mouse_leave_callback(button* btn,
                                void (*callback)(button*, mouse_motion_event))
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

  btn->private_data->user_mouse_leave_callback = callback;

  return ok_void();
}

static void default_internal_derived_free_callback(base_widget* widget)
{
  trace("Button(%s): internal-derived-free()", widget->debug_name);

  button* btn = (button*)widget->derived;
  // freeing button text
  free(btn->private_data->text);
  // freeing button private struct
  free(btn->private_data);

  // freeing button object
  // freeing base_widget is taken care by internal_free_callback
  free(btn);
}

static result_sizing_delta
default_internal_fit_layout_callback(base_widget* widget, bool call_on_children)
{
  trace("Button(%s): internal-fit-layout()", widget->debug_name);

  button* btn = (button*)widget->derived;

  result_text_dimensions ___ = widget->context->backend->get_text_dimensions(
    btn->private_data->text, widget->context->font, widget->context->font_size);
  if(!___.ok)
  {
    return error(result_sizing_delta, ___.error);
  }
  text_dimensions dimensions = ___.value;

  debug("  > Text: \"%s\", dimensions: %d, %d",
        btn->private_data->text,
        dimensions.w,
        dimensions.h);
  debug("  > Widget dimensions: %d, %d | padding: %d, %d",
        widget->w,
        widget->h,
        btn->padding_x,
        btn->padding_y);

  uint16 new_w = dimensions.w + 2 * btn->padding_x;
  uint16 new_h = dimensions.h + 2 * btn->padding_y;
  sizing_delta deltas = {.x = (int16)new_w - (int16)widget->w,
                         .y = (int16)new_h - (int16)widget->h};

  info("  > sizing-deltas: (%d, %d)", deltas.x, deltas.y);

  // setting widget's sizing, so no need to set need resizing flag
  widget->w = new_w;
  widget->h = new_h;

  return ok(result_sizing_delta, deltas);
}

static result_bool default_internal_render_callback(const base_widget* widget)
{
  button* btn = (button*)widget->derived;

  color foreground =
    btn->private_data->state == BUTTON_NORMAL
      ? btn->foreground
      : (btn->private_data->state == BUTTON_HOVERED ? btn->hover_foreground
                                                    : btn->click_foreground);
  color background =
    btn->private_data->state == BUTTON_NORMAL
      ? btn->background
      : (btn->private_data->state == BUTTON_HOVERED ? btn->hover_background
                                                    : btn->click_background);

  trace("Button(%s): internal-render(), text: \"%s\", (x, y, w, h): (%d, %d, "
        "%d, %d), background: (%d, %d, %d, %d), foreground: (%d, %d, %d, %d)",
        widget->debug_name,
        btn->private_data->text,
        widget->x,
        widget->y,
        widget->w,
        widget->h,
        background.r,
        background.g,
        background.b,
        background.a,
        foreground.r,
        foreground.g,
        foreground.b,
        foreground.a);

  rect bounding_rect = common_internal_get_bounding_rect(widget);

  if(btn->border_radius == 0)
  {
    result_void __ = command_buffer_add_render_rect_command(
      widget->context->cmd_buffer, bounding_rect, background);
    if(!__.ok)
    {
      return error(result_bool, __.error);
    }
  }
  else
  {
    result_void __ = command_buffer_add_render_rounded_rect_command(
      widget->context->cmd_buffer,
      bounding_rect,
      btn->border_radius,
      background);
    if(!__.ok)
    {
      return error(result_bool, __.error);
    }
  }

  bounding_rect.x += (int16)(btn->padding_x);
  bounding_rect.y += (int16)(btn->padding_y);

  result_void ___ = command_buffer_add_render_text_command(
    widget->context->cmd_buffer,
    btn->private_data->text,
    foreground,
    (point){.x = bounding_rect.x, .y = bounding_rect.y});
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
  btn->private_data->state = BUTTON_CLICKED;

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  // calling user registered event callback
  // maybe we should call this on another thread
  // as this will block the ui thread
  if(btn->private_data->user_mouse_button_down_callback)
  {
    btn->private_data->user_mouse_button_down_callback(btn, event);
  }

  return _.value;
}

static bool default_mouse_button_up_callback(base_widget* widget,
                                             mouse_button_event event)
{
  button* btn = (button*)widget->derived;
  btn->private_data->state = BUTTON_HOVERED;

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  // calling user registered event callback
  // maybe we should call this on another thread
  // as this will block the ui thread
  if(btn->private_data->user_mouse_button_up_callback)
  {
    btn->private_data->user_mouse_button_up_callback(btn, event);
  }

  return _.value;
}

static bool default_mouse_enter_callback(base_widget* widget,
                                         mouse_motion_event event)
{
  button* btn = (button*)widget->derived;
  btn->private_data->state = BUTTON_HOVERED;

  {
    result_void _ = command_buffer_add_set_cursor_command(
      widget->context->cmd_buffer, SET_CURSOR_HAND);
    if(!_.ok)
    {
      return false;
    }
  }

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  // calling user registered event callback
  // maybe we should call this on another thread
  // as this will block the ui thread
  if(btn->private_data->user_mouse_enter_callback)
  {
    btn->private_data->user_mouse_enter_callback(btn, event);
  }

  return _.value;
}

static bool default_mouse_leave_callback(base_widget* widget,
                                         mouse_motion_event event)
{
  button* btn = (button*)widget->derived;
  btn->private_data->state = BUTTON_NORMAL;

  {
    result_void _ = command_buffer_add_set_cursor_command(
      widget->context->cmd_buffer, SET_CURSOR_ARROW);
    if(!_.ok)
    {
      return false;
    }
  }

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  // calling user registered event callback
  // maybe we should call this on another thread
  // as this will block the ui thread
  if(btn->private_data->user_mouse_leave_callback)
  {
    btn->private_data->user_mouse_leave_callback(btn, event);
  }

  return _.value;
}
