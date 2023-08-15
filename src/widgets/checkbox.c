#include "../../include/widgets/checkbox.h"
#include <stdlib.h>
#include "../../include/macros.h"

typedef enum checkbox_state
{
  TICKED,
  UNTICKED
} checkbox_state;

struct checkbox_private
{
  checkbox_state state;

  color foreground;

  void (*user_ticked_callback)(checkbox*);

  void (*user_unticked_callback)(checkbox*);
};

static result_bool default_internal_render_callback(const base_widget* widget);

static void default_internal_derived_free_callback(base_widget* widget);

static bool default_mouse_button_down_callback(base_widget* widget,
                                               mouse_button_event event);

result_checkbox_ptr checkbox_new(base_widget* parent_base, color foreground)
{
  checkbox* box = (checkbox*)calloc(1, sizeof(checkbox));
  if(!box)
  {
    return error(result_checkbox_ptr,
                 "Unable to allocate memory for checkbox widget!");
  }

  result_base_widget_ptr _ = base_widget_new();
  if(!_.ok)
  {
    free(box);
    return error(result_checkbox_ptr, _.error);
  }

  checkbox_private* box_private =
    (checkbox_private*)calloc(1, sizeof(checkbox_private));
  if(!box_private)
  {
    free(box);
    base_widget_free(_.value);
    return error(
      result_checkbox_ptr,
      "Unable to allocate memory for private fields of checkbox widget!");
  }

  box->base = _.value;
  box->base->derived = box;
  box->base->parent = parent_base;

  if(parent_base)
  {
    base_widget_add_child(parent_base, box->base);
    box->base->context = parent_base->context;
  }

  box->base->internal_render_callback = default_internal_render_callback;
  box->base->internal_derived_free_callback =
    default_internal_derived_free_callback;
  box->base->mouse_button_down_callback = default_mouse_button_down_callback;

  box_private->foreground = foreground;
  box_private->state = UNTICKED;
  box_private->user_ticked_callback = NULL;
  box_private->user_unticked_callback = NULL;
  box->private_data = box_private;

  // setting checkbox size
  box->base->w = 20;
  box->base->h = 20;

  return ok(result_checkbox_ptr, box);
}

result_void checkbox_set_ticked_callback(checkbox* box,
                                         void (*callback)(checkbox*))
{
  if(!box)
  {
    return error(
      result_void,
      "Cannot set ticked callback to a NULL pointing checkbox widget!");
  }

  if(!callback)
  {
    return error(
      result_void,
      "Cannot set NULL pointing ticked callback to checkbox widget!");
  }

  box->private_data->user_ticked_callback = callback;

  return ok_void();
}

result_void checkbox_set_unticked_callback(checkbox* box,
                                           void (*callback)(checkbox*))
{
  if(!box)
  {
    return error(
      result_void,
      "Cannot set unticked callback to a NULL pointing checkbox widget!");
  }

  if(!callback)
  {
    return error(
      result_void,
      "Cannot set NULL pointing unticked callback to checkbox widget!");
  }

  box->private_data->user_unticked_callback = callback;

  return ok_void();
}

static result_bool default_internal_render_callback(const base_widget* widget)
{
  if(!widget->parent)
  {
    return error(result_bool,
                 "Checkbox widget must be placed inside a parent widget!");
  }

  if(!widget->parent->internal_get_background_callback)
  {
    return error(result_bool,
                 "Checkbox widget's parent didn't implement internal get "
                 "background callback!");
  }

  checkbox* box = (checkbox*)widget->derived;
  rect bounding_rect = widget->internal_get_bounding_rect_callback(widget);

  result_void _ = command_buffer_add_render_rect_command(
    widget->context->cmd_buffer,
    bounding_rect,
    widget->parent->internal_get_background_callback(widget->parent));
  if(!_.ok)
  {
    return error(result_bool, _.error);
  }

  _ = command_buffer_add_render_rect_outline_command(
    widget->context->cmd_buffer, bounding_rect, box->private_data->foreground);
  if(!_.ok)
  {
    return error(result_bool, _.error);
  }

  if(box->private_data->state == TICKED)
  {
    uint8 delta = bounding_rect.w - bounding_rect.w * 0.8f;
    bounding_rect.x += delta;
    bounding_rect.y += delta;
    bounding_rect.w -= 2 * delta;
    bounding_rect.h -= 2 * delta;
    result_void _ =
      command_buffer_add_render_rect_command(widget->context->cmd_buffer,
                                             bounding_rect,
                                             box->private_data->foreground);
    if(!_.ok)
    {
      return error(result_bool, _.error);
    }
  }

  return ok(result_bool, true);
}

static void default_internal_derived_free_callback(base_widget* widget)
{
  if(!widget)
  {
    return;
  }

  checkbox* box = (checkbox*)widget->derived;

  // freeing checkbox private struct
  free(box->private_data);

  // freeing checkbox object
  // freeing base_widget is taken care by internal_free_callback
  free(box);
}

static bool default_mouse_button_down_callback(base_widget* widget,
                                               mouse_button_event event)
{
  checkbox* box = (checkbox*)widget->derived;
  if(box->private_data->state == UNTICKED)
  {
    box->private_data->state = TICKED;
  }
  else
  {
    box->private_data->state = UNTICKED;
  }

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  return true;
}
