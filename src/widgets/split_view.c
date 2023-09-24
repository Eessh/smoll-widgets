#include "../../include/widgets/split_view.h"
#include <stdlib.h>
#include "../../include/macros.h"

typedef enum handle_state
{
  HANDLE_NORMAL,
  HANDLE_HOVERED,
  HANDLE_CLICKED
} handle_state;

struct split_view_private
{
  handle_state state;
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

static bool default_mouse_leave_callback(base_widget* widget,
                                         mouse_motion_event event);

result_split_view_ptr split_view_new(base_widget* parent_base, split_type type)
{
  split_view* v = (split_view*)calloc(1, sizeof(split_view));
  if(!v)
  {
    return error(result_split_view_ptr,
                 "Unable to allocate memory for split view widget!");
  }

  result_base_widget_ptr _ = base_widget_new(FLEX_CONTAINER);
  if(!_.ok)
  {
    free(v);
    return error(result_split_view_ptr, _.error);
  }

  split_view_private* v_private =
    (split_view_private*)calloc(1, sizeof(split_view_private));
  if(!v_private)
  {
    free(v);
    base_widget_free(_.value);
    return error(result_split_view_ptr,
                 "Unable to allocate memory for private fields of split view!");
  }

  v->base = _.value;
  v->base->derived = v;
  v->base->parent = parent_base;

  v->private_data = v_private;

  if(parent_base)
  {
    base_widget_add_child(parent_base, v->base);
  }

  v->base->internal_fit_layout_callback = default_internal_fit_layout_callback;
  v->base->internal_render_callback = default_internal_render_callback;
  v->base->internal_derived_free_callback =
    default_internal_derived_free_callback;

  v->base->mouse_button_down_callback = default_mouse_button_down_callback;
  v->base->mouse_button_up_callback = default_mouse_button_up_callback;
  v->base->mouse_enter_callback = default_mouse_enter_callback;
  v->base->mouse_leave_callback = default_mouse_leave_callback;

  v->base->flexbox_data.container.direction =
    type == SPLIT_HORIZONTAL ? FLEX_DIRECTION_ROW : FLEX_DIRECTION_COLUMN;

  v->handle_size = 2;
  v->handle_color = (color){255, 0, 0, 255};
  v->handle_hover_color = (color){0, 255, 0, 255};
  v->handle_click_color = (color){0, 0, 255, 255};
  v_private->state = HANDLE_NORMAL;

  return ok(result_split_view_ptr, v);
}

result_void split_view_connect_children(split_view* view,
                                        base_widget* first_child,
                                        base_widget* second_child)
{
  if(!view)
  {
    return error(result_void,
                 "Cannot connect children to NULL pointed split-view!");
  }

  if(!first_child || !second_child)
  {
    return error(result_void, "first_child or second_child is NULL!");
  }

  base_widget_add_child(view->base, first_child);

  //  result_base_widget_ptr _ = base_widget_new(FLEX_ITEM);
  //  if(!_.ok)
  //  {
  //    return error(result_void, _.error);
  //  }
  //  base_widget* handle = _.value;
  //  handle->w =
  //    view->type == SPLIT_HORIZONTAL ? view->handle_size : view->base->w;
  //  handle->h =
  //    view->type == SPLIT_HORIZONTAL ? view->base->h : view->handle_size;
  //  base_widget_add_child(view->base, handle);

  base_widget_add_child(view->base, second_child);

  return ok_void();
}

static void default_internal_derived_free_callback(base_widget* widget)
{
  split_view* v = (split_view*)widget->derived;

  free(v->private_data);
  free(v);
}

static result_sizing_delta
default_internal_fit_layout_callback(base_widget* widget, bool call_on_children)
{
  if(call_on_children)
  {
    base_widget_child_node* node = widget->children_head;
    while(node)
    {
      if(node->child->internal_fit_layout_callback)
      {
        node->child->internal_fit_layout_callback(node->child, true);
      }
      node = node->next;
    }
  }

  base_widget* first_child = NULL;
  base_widget* second_child = NULL;

  if(widget->children_head)
  {
    first_child = widget->children_head->child;
    second_child =
      widget->children_head->next ? widget->children_head->next->child : NULL;
  }

  split_view* v = (split_view*)widget->derived;

  uint16 new_w = v->type == SPLIT_HORIZONTAL
                   ? (first_child ? first_child->w : 0) + v->handle_size +
                       (second_child ? second_child->w : 0)
                   : max(first_child ? first_child->w : 0,
                         second_child ? second_child->w : 0);
  uint16 new_h = v->type == SPLIT_HORIZONTAL
                   ? max(first_child ? first_child->h : 0,
                         second_child ? second_child->h : 0)
                   : (first_child ? first_child->h : 0) + v->handle_size +
                       (second_child ? second_child->h : 0);
  sizing_delta deltas = {.x = (int16)new_w - (int16)widget->w,
                         .y = (int16)new_h - (int16)widget->h};

  widget->w = new_w;
  widget->h = new_h;

  return ok(result_sizing_delta, deltas);
}

static result_bool default_internal_render_callback(const base_widget* widget)
{
  split_view* v = (split_view*)widget->derived;

  base_widget* first_child = NULL;
  base_widget* second_child = NULL;

  if(widget->children_head)
  {
    first_child = widget->children_head->child;
    second_child =
      widget->children_head->next ? widget->children_head->next->child : NULL;
  }

  result_void _ = command_buffer_add_render_rect_command(
    widget->context->cmd_buffer,
    v->type == SPLIT_HORIZONTAL
      ? (rect){first_child ? first_child->w + 1 : widget->x,
               widget->y,
               v->handle_size,
               widget->h}
      : (rect){widget->x,
               first_child ? first_child->h + 1 : widget->y,
               widget->w,
               v->handle_size},
    v->private_data->state == HANDLE_NORMAL
      ? v->handle_color
      : (v->private_data->state == HANDLE_HOVERED ? v->handle_hover_color
                                                  : v->handle_click_color));
  if(!_.ok)
  {
    return error(result_bool, _.error);
  }

  if(first_child)
  {
    first_child->internal_render_callback(first_child);
  }
  if(second_child)
  {
    second_child->internal_render_callback(second_child);
  }

  return ok(result_bool, true);
}

static bool default_mouse_button_down_callback(base_widget* widget,
                                               mouse_button_event event)
{
  split_view* v = (split_view*)widget->derived;

  v->private_data->state = HANDLE_CLICKED;

  return true;
}

static bool default_mouse_button_up_callback(base_widget* widget,
                                             mouse_button_event event)
{
  split_view* v = (split_view*)widget->derived;

  v->private_data->state = HANDLE_NORMAL;

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
  split_view* v = (split_view*)widget->derived;

  v->private_data->state = HANDLE_HOVERED;

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  return true;
}

static bool default_mouse_leave_callback(base_widget* widget,
                                         mouse_motion_event event)
{
  split_view* v = (split_view*)widget->derived;

  v->private_data->state = HANDLE_NORMAL;

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  return true;
}
