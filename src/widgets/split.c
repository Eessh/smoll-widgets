#include "../../include/widgets/split.h"
#include <stdio.h>
#include <stdlib.h>
#include "../../include/macros.h"

typedef enum split_state
{
  SPLIT_NORMAL,
  SPLIT_HOVERED,
  SPLIT_CLICKED,
} split_state;

struct split_private
{
  split_state state;

  base_widget* first_widget;
  base_widget* second_widget;

  uint32 clicked_x, clicked_y;
};

static void default_internal_derived_free_callback(base_widget* widget);

static result_sizing_delta
default_internal_fit_layout_callback(base_widget* widget,
                                     bool call_on_children);

static result_bool default_internal_render_callback(const base_widget* widget);

static bool default_mouse_button_down_callback(base_widget* widget,
                                               mouse_button_event event);

static bool default_mouse_button_up_callback(base_widget* widget,
                                             mouse_button_event event);

static bool default_mouse_enter_callback(base_widget* widget,
                                         mouse_motion_event event);

static bool default_mouse_move_callback(base_widget* widget,
                                        mouse_motion_event event);

static bool default_mouse_leave_callback(base_widget* widget,
                                         mouse_motion_event event);

result_split_ptr split_new(base_widget* parent_base,
                           split_direction direction,
                           base_widget* first_widget,
                           base_widget* second_widget)
{
  if(!parent_base)
  {
    return error(result_split_ptr, "Split widget needs parent base!");
  }

  if(!first_widget)
  {
    return error(result_split_ptr, "First widget is pointing to NULL!");
  }

  if(!second_widget)
  {
    return error(result_split_ptr, "Second widget is pointing to NULL!");
  }

  split* s = (split*)calloc(1, sizeof(split));
  if(!s)
  {
    return error(result_split_ptr,
                 "Unable to allocate memory for split widget!");
  }

  result_base_widget_ptr _ = base_widget_new(FLEX_ITEM);
  if(!_.ok)
  {
    free(s);
    return error(result_split_ptr, _.error);
  }

  split_private* s_private = (split_private*)calloc(1, sizeof(split_private));
  if(!s_private)
  {
    free(s);
    base_widget_free(_.value);
    return error(
      result_split_ptr,
      "Unable to allocate memory for split widget's private fields!");
  }

  s->base = _.value;
  s->base->derived = s;
  s->base->parent = parent_base;

  base_widget_add_child_after(parent_base, s->base, first_widget);

  s->base->internal_fit_layout_callback = default_internal_fit_layout_callback;
  s->base->internal_render_callback = default_internal_render_callback;
  s->base->internal_derived_free_callback =
    default_internal_derived_free_callback;

  s->base->mouse_button_down_callback = default_mouse_button_down_callback;
  s->base->mouse_button_up_callback = default_mouse_button_up_callback;
  s->base->mouse_enter_callback = default_mouse_enter_callback;
  s->base->mouse_move_callback = default_mouse_move_callback;
  s->base->mouse_leave_callback = default_mouse_leave_callback;

  s->direction = direction;
  s->split_color = (color){255, 0, 0, 255};
  s->split_hover_color = (color){0, 255, 0, 255};
  s->split_click_color = (color){0, 0, 255, 255};
  s->private_data = s_private;
  s_private->first_widget = first_widget;
  s_private->second_widget = second_widget;
  s_private->state = SPLIT_NORMAL;

  // making split widget to expand along cross-axis of parent
  s->base->flexbox_data.item.cross_axis_sizing = CROSS_AXIS_SIZING_EXPAND;

  if(direction == SPLIT_DIRECTION_VERTICAL)
  {
    s->base->w = 10;
    s->base->h = parent_base->h;
  }
  else
  {
    s->base->w = parent_base->w;
    s->base->h = 10;
  }

  return ok(result_split_ptr, s);
}

result_void split_set_size(split* split_widget, uint8 size)
{
  if(!split_widget)
  {
    return error(result_void,
                 "Cannot set size of split widget, which is pointing to NULL!");
  }

  if(split_widget->direction == SPLIT_DIRECTION_VERTICAL)
  {
    split_widget->base->w = size;
  }
  else
  {
    split_widget->base->h = size;
  }

  // triggering re-layout on parent widget
  result_bool _ = common_internal_adjust_layout(
    split_widget->base->parent);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  return ok_void();
}

static void default_internal_derived_free_callback(base_widget* widget)
{
  split* s = (split*)widget->derived;

  free(s->private_data);
  free(s);
}

static result_sizing_delta
default_internal_fit_layout_callback(base_widget* widget, bool call_on_children)
{
  // doesn't give a hekk

  sizing_delta deltas = {.x = 0, .y = 0};

  return ok(result_sizing_delta, deltas);
}

static result_bool default_internal_render_callback(const base_widget* widget)
{
  split* s = (split*)widget->derived;

  result_void _ = command_buffer_add_render_rect_command(
    widget->context->cmd_buffer,
    (rect){widget->x, widget->y, widget->w, widget->h},
    s->private_data->state == SPLIT_NORMAL
      ? s->split_color
      : (s->private_data->state == SPLIT_HOVERED ? s->split_hover_color
                                                 : s->split_click_color));
  if(!_.ok)
  {
    return error(result_bool, _.error);
  }

  return ok(result_bool, true);
}

static bool default_mouse_button_down_callback(base_widget* widget,
                                               mouse_button_event event)
{
  split* s = (split*)widget->derived;

  s->private_data->state = SPLIT_CLICKED;
  s->private_data->clicked_x = event.x;
  s->private_data->clicked_y = event.y;

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  return true;
}

static bool default_mouse_button_up_callback(base_widget* widget,
                                             mouse_button_event event)
{
  split* s = (split*)widget->derived;

  s->private_data->state = SPLIT_NORMAL;

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  return true;
}

static bool default_mouse_enter_callback(base_widget* widget,
                                         mouse_motion_event event)
{
  split* v = (split*)widget->derived;

  v->private_data->state = SPLIT_HOVERED;

  result_void _ = command_buffer_add_set_cursor_command(
    widget->context->cmd_buffer, SET_CURSOR_RESIZE_LEFT_RIGHT);
  if(!_.ok)
  {
    return false;
  }

  {
    result_bool _ = widget->internal_render_callback(widget);
    if(!_.ok)
    {
      return false;
    }
  }

  return true;
}

static bool default_mouse_move_callback(base_widget* widget,
                                        mouse_motion_event event)
{
  split* s = (split*)widget->derived;

  if(s->private_data->state != SPLIT_CLICKED)
  {
    // ignore
    return false;
  }

  // marking first, second widgets as non-fluid, if they are containers
  //  if(s->private_data->first_widget->type == FLEX_CONTAINER)
  //  {
  //    s->private_data->first_widget->flexbox_data.container.is_fluid = false;
  //  }
  //  if(s->private_data->second_widget->type == FLEX_CONTAINER)
  //  {
  //    s->private_data->second_widget->flexbox_data.container.is_fluid = false;
  //  }

  if(s->direction == SPLIT_DIRECTION_VERTICAL)
  {
    // measure delta along x-axis
    int32 delta = event.x - s->private_data->clicked_x;
    s->private_data->first_widget->w += delta;
    s->private_data->second_widget->w -= delta;
  }
  else
  {
    // measure delta along y-axis
    int32 delta = event.y - s->private_data->clicked_y;
    s->private_data->first_widget->h += delta;
    s->private_data->second_widget->h -= delta;
  }

  // triggering re-layout on first and second widgets
  common_internal_adjust_layout(
    s->private_data->first_widget);
  common_internal_adjust_layout(
    s->private_data->second_widget);

  // triggering re-layout on parent widget
  result_bool _ =
    common_internal_adjust_layout(widget->parent);
  if(!_.ok)
  {
    return false;
  }

  widget->parent->internal_render_callback(widget->parent);

  return true;
}

static bool default_mouse_leave_callback(base_widget* widget,
                                         mouse_motion_event event)
{
  split* v = (split*)widget->derived;

  v->private_data->state = SPLIT_NORMAL;

  result_void _ = command_buffer_add_set_cursor_command(
    widget->context->cmd_buffer, SET_CURSOR_ARROW);
  if(!_.ok)
  {
    return false;
  }

  {
    result_bool _ = widget->internal_render_callback(widget);
    if(!_.ok)
    {
      return false;
    }
  }

  return true;
}
