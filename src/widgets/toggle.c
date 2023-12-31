#include "../../include/widgets/toggle.h"
#include <stdlib.h>
#include "../../include/macros.h"

typedef enum toggle_state
{
  TOGGLE_OFF,
  TOGGLE_ON
} toggle_state;

struct toggle_private
{
  toggle_state state;

  void (*user_on_callback)(toggle*);

  void (*user_off_callback)(toggle*);
};

static result_bool default_internal_render_callback(const base_widget* widget);

static void default_internal_derived_free_callback(base_widget* widget);

static bool default_mouse_button_down_callback(base_widget* widget,
                                               mouse_button_event event);

result_toggle_ptr toggle_new(base_widget* parent_base)
{
  toggle* t = (toggle*)calloc(1, sizeof(toggle));
  if(!t)
  {
    return error(result_toggle_ptr,
                 "Unable to allocat memory for toggle widget!");
  }

  result_base_widget_ptr _ = base_widget_new(FLEX_ITEM);
  if(!_.ok)
  {
    free(t);
    return error(result_toggle_ptr, _.error);
  }

  toggle_private* t_private =
    (toggle_private*)calloc(1, sizeof(toggle_private));
  if(!t_private)
  {
    free(t);
    base_widget_free(_.value);
    return error(result_toggle_ptr,
                 "Unable to allocate memory for toggle widget private data!");
  }

  t->base = _.value;
  t->base->derived = t;
  t->base->parent = parent_base;

  t->private_data = t_private;

  // attaching this toggle to parent's children list
  // and assigning context
  if(parent_base)
  {
    base_widget_add_child(parent_base, t->base);
    t->base->context = parent_base->context;
  }

  t->base->internal_render_callback = default_internal_render_callback;
  t->base->internal_derived_free_callback =
    default_internal_derived_free_callback;

  t->base->mouse_button_down_callback = default_mouse_button_down_callback;

  t->handle_width_fraction = 0.5f;

  t->padding_x = 1;
  t->padding_y = 1;

  t->on_handle_color = (color){0, 255, 0, 255};
  t->on_background = (color){216, 216, 216, 255};

  t->off_handle_color = (color){255, 255, 255, 255};
  t->off_background = (color){0, 0, 0, 255};

  t->private_data->state = TOGGLE_OFF;

  t->private_data->user_on_callback = NULL;
  t->private_data->user_off_callback = NULL;

  // setting need resizing flag to false
  t->base->need_resizing = false;

  return ok(result_toggle_ptr, t);
}

result_toggle_ptr toggle_new_with_debug_name(base_widget* parent_base,
                                             const char* debug_name)
{
  result_toggle_ptr _ = toggle_new(parent_base);
  if(!_.ok)
  {
    return _;
  }

  _.value->base->debug_name = debug_name;

  trace("Toggle: created with debug-name: %s", debug_name);

  return _;
}

result_void toggle_set_on_callback(toggle* t, void (*callback)(toggle*))
{
  if(!t)
  {
    return error(result_void,
                 "Cannot set on callback for a NULL pointing toggle widget!");
  }

  if(!callback)
  {
    return error(result_void,
                 "Cannot set NULL pointing callback function for on event of "
                 "toggle widget!");
  }

  t->private_data->user_on_callback = callback;

  return ok_void();
}

result_void toggle_set_off_callback(toggle* t, void (*callback)(toggle*))
{
  if(!t)
  {
    return error(result_void,
                 "Cannot set off callback for a NULL pointing toggle widget!");
  }

  if(!callback)
  {
    return error(result_void,
                 "Cannot set NULL pointing callback function for off event of "
                 "toggle widget!");
  }

  t->private_data->user_off_callback = callback;

  return ok_void();
}

static result_bool default_internal_render_callback(const base_widget* widget)
{
  toggle* t = (toggle*)widget->derived;

  color handle_color = t->private_data->state == TOGGLE_ON
                         ? t->on_handle_color
                         : t->off_handle_color;
  color background =
    t->private_data->state == TOGGLE_ON ? t->on_background : t->off_background;

  trace("Toggle(%s): internal-render(), (x, y, w, h): (%d, %d, %d, %d), "
        "background: (%d, %d, %d, %d), handle-color: (%d, %d, %d, %d)",
        widget->debug_name,
        widget->x,
        widget->y,
        widget->w,
        widget->h,
        background.r,
        background.g,
        background.b,
        background.a,
        handle_color.r,
        handle_color.g,
        handle_color.b,
        handle_color.a);

  rect bounding_rect = common_internal_get_bounding_rect(widget);

  {
    result_void _ = command_buffer_add_render_rect_command(
      widget->context->cmd_buffer, bounding_rect, background);
    if(!_.ok)
    {
      return error(result_bool, _.error);
    }
  }

  bounding_rect.x += (int16)(t->padding_x);
  bounding_rect.y += (int16)(t->padding_y);
  bounding_rect.w -= 2 * t->padding_x;
  uint16 handle_width = bounding_rect.w * t->handle_width_fraction;
  if(t->private_data->state == TOGGLE_ON)
  {
    // move handle by remaining space
    bounding_rect.x += bounding_rect.w - handle_width;
  }
  bounding_rect.w = handle_width;
  bounding_rect.h -= 2 * t->padding_y;

  {
    result_void _ = command_buffer_add_render_rect_command(
      widget->context->cmd_buffer, bounding_rect, handle_color);
    if(!_.ok)
    {
      return error(result_bool, _.error);
    }
  }

  return ok(result_bool, true);
}

static void default_internal_derived_free_callback(base_widget* widget)
{
  trace("Toggle(%s): internal-derived-free()", widget->debug_name);

  toggle* t = (toggle*)widget->derived;

  // freeing toggle private struct
  free(t->private_data);

  // freeing toggle object
  // freeing base_widget is taken care by internal_free_callback
  free(t);
}

static bool default_mouse_button_down_callback(base_widget* widget,
                                               mouse_button_event event)
{
  toggle* t = (toggle*)widget->derived;

  if(t->private_data->state == TOGGLE_OFF)
  {
    t->private_data->state = TOGGLE_ON;

    // calling user callbacks (if present)
    if(t->private_data->user_on_callback)
    {
      t->private_data->user_on_callback(t);
    }
  }
  else
  {
    t->private_data->state = TOGGLE_OFF;

    // calling user callbacks (if present)
    if(t->private_data->user_off_callback)
    {
      t->private_data->user_off_callback(t);
    }
  }

  // updating UI
  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  return true;
}
