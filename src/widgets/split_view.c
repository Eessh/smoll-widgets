#include "../../include/widgets/split_view.h"
#include <stdio.h>
#include <stdlib.h>
#include "../../include/macros.h"
#include "../../include/widgets/box.h"

typedef enum handle_state
{
  HANDLE_NORMAL,
  HANDLE_HOVERED,
  HANDLE_CLICKED
} handle_state;

typedef struct split_private
{
  handle_state state;
  point last_clicked;
  float32 ratio;
} split_private;

typedef struct split
{
  base_widget* base;

  split_private* private_data;
} split;

split* split_new();

static void split_internal_derived_free_callback(base_widget* widget);

static result_sizing_delta
default_internal_fit_layout_callback(base_widget* widget,
                                     bool call_on_children);

static result_void
default_pre_internal_relayout_hook(const base_widget* widget);

static result_bool default_internal_render_callback(const base_widget* widget);

static result_bool split_internal_render_callback(const base_widget* widget);

static bool split_mouse_button_down_callback(base_widget* widget,
                                             mouse_button_event event);

static bool split_mouse_button_up_callback(base_widget* widget,
                                           mouse_button_event event);

static bool split_mouse_enter_callback(base_widget* widget,
                                       mouse_motion_event event);

static bool split_mouse_leave_callback(base_widget* widget,
                                       mouse_motion_event event);

static bool split_mouse_move_callback(base_widget* widget,
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

  v->base = _.value;
  v->base->derived = v;
  v->base->parent = parent_base;
  v->type = type;
  v->base->flexbox_data.container.direction =
    type == SPLIT_VERTICAL ? FLEX_DIRECTION_ROW : FLEX_DIRECTION_COLUMN;

  if(parent_base)
  {
    base_widget_add_child(parent_base, v->base);
  }

  // creating first child's container
  result_box_ptr __ = box_new(v->base, FLEX_DIRECTION_COLUMN);
  if(!__.ok)
  {
    free(v);
    base_widget_free(_.value);
    return error(
      result_split_view_ptr,
      "Unable to allocate memory for first-child's container of split view!");
  }
  box* first_container = __.value;
  first_container->base->flexbox_data.container.is_fluid = false;
  first_container->base->flexbox_data.container.direction =
    FLEX_DIRECTION_COLUMN;
  first_container->base->flexbox_data.container.justify_content =
    JUSTIFY_CONTENT_START;
  first_container->base->flexbox_data.container.align_items = ALIGN_ITEMS_START;
  first_container->background = (color){255, 0, 0, 255};

  // creating splitter
  struct split* splitter = split_new();
  if(!splitter)
  {
    free(v);
    base_widget_free(_.value);
    return error(result_split_view_ptr,
                 "Unable to allocate memory for splitter of split view!");
  }
  base_widget_add_child(v->base, splitter->base);

  // creating second child's contaniner
  __ = box_new(v->base, FLEX_DIRECTION_COLUMN);
  if(!__.ok)
  {
    free(v);
    base_widget_free(_.value);
    return error(
      result_split_view_ptr,
      "Unable to allocate memory for second-child's container of split view!");
  }
  box* second_container = __.value;
  second_container->base->flexbox_data.container.is_fluid = false;
  second_container->background = (color){0, 0, 255, 255};

  v->base->internal_fit_layout_callback = default_internal_fit_layout_callback;
  v->base->pre_internal_relayout_hook = default_pre_internal_relayout_hook;
  v->base->internal_render_callback = default_internal_render_callback;
  splitter->base->internal_derived_free_callback =
    split_internal_derived_free_callback;

  splitter->base->mouse_button_down_callback = split_mouse_button_down_callback;
  splitter->base->mouse_button_up_callback = split_mouse_button_up_callback;
  splitter->base->mouse_enter_callback = split_mouse_enter_callback;
  splitter->base->mouse_leave_callback = split_mouse_leave_callback;
  splitter->base->mouse_move_callback = split_mouse_move_callback;

  v->handle_size = 3;
  v->handle_color = (color){255, 0, 0, 255};
  v->handle_hover_color = (color){0, 255, 0, 255};
  v->handle_click_color = (color){0, 0, 255, 255};

  first_container->base->internal_render_callback =
    default_internal_render_callback;
  second_container->base->internal_render_callback =
    default_internal_render_callback;

  first_container->background = (color){0, 0, 0, 255};
  second_container->background = (color){16, 16, 16, 255};

  splitter->base->flexbox_data.item.cross_axis_sizing =
    CROSS_AXIS_SIZING_EXPAND;
  splitter->base->internal_render_callback = split_internal_render_callback;
  if(type == SPLIT_HORIZONTAL)
  {
    splitter->base->w = v->base->w;
    splitter->base->h = v->handle_size;
  }
  else
  {
    splitter->base->w = v->handle_size;
    splitter->base->h = v->base->h;
  }

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

  base_widget_child_node* first_container_node = view->base->children_head;
  base_widget_child_node* second_container_node =
    first_container_node->next->next;

  base_widget_add_child(first_container_node->child, first_child);
  base_widget_add_child(second_container_node->child, second_child);

  return ok_void();
}

split* split_new()
{
  split* s = (split*)calloc(1, sizeof(split));
  if(!s)
  {
    return NULL;
  }

  result_base_widget_ptr _ = base_widget_new(FLEX_ITEM);
  if(!_.ok)
  {
    free(s);
    return NULL;
  }
  s->base = _.value;
  s->base->derived = s;

  s->private_data = (split_private*)calloc(1, sizeof(split_private));
  if(!s->private_data)
  {
    free(s);
    base_widget_free(s->base);
    return NULL;
  }
  s->private_data->state = HANDLE_NORMAL;
  s->private_data->last_clicked = (point){0, 0};
  s->private_data->ratio = 0.5f;

  return s;
}

static void split_internal_derived_free_callback(base_widget* widget)
{
  split* s = (split*)widget->derived;

  free(s->private_data);
  free(s);
}

static result_bool split_internal_render_callback(const base_widget* widget)
{
  split* s = (split*)widget->derived;
  split_view* parent = (split_view*)widget->parent->derived;
  color bg = s->private_data->state == HANDLE_NORMAL ? parent->handle_color
             : s->private_data->state == HANDLE_HOVERED
               ? parent->handle_hover_color
               : parent->handle_click_color;

  info("SplitView(): split.internal-render-callback(), (x, y, w, h): (%d, %d, "
       "%d, %d), color: (%d, %d, %d, %d)",
       widget->x,
       widget->y,
       widget->w,
       widget->h,
       bg.r,
       bg.g,
       bg.b,
       bg.a);

  result_void _ = command_buffer_add_render_rect_command(
    widget->context->cmd_buffer,
    (rect){widget->x, widget->y, widget->w, widget->h},
    bg);
  if(!_.ok)
  {
    return error(result_bool, _.error);
  }

  return ok(result_bool, true);
}

static result_void default_pre_internal_relayout_hook(const base_widget* widget)
{
  info("Called internal re-layout hook");
  base_widget_child_node* first_container = widget->children_head;
  base_widget_child_node* splitter_node = first_container->next;
  base_widget_child_node* second_container = splitter_node->next;

  split_view* s = (split_view*)widget->derived;
  float32 ratio =
    ((split*)(splitter_node->child->derived))->private_data->ratio;
  if(s->type == SPLIT_VERTICAL)
  {
    first_container->child->w = (widget->w - splitter_node->child->w) * ratio;
    second_container->child->w =
      (widget->w - splitter_node->child->w) * ((float32)(1 - ratio));
    first_container->child->h = widget->h;
    second_container->child->h = widget->h;
    splitter_node->child->h = widget->h;
  }
  else
  {
    first_container->child->w = widget->w;
    second_container->child->w = widget->w;
    splitter_node->child->w = widget->w;
    first_container->child->h = (widget->h - splitter_node->child->h) * ratio;
    second_container->child->h =
      (widget->h - splitter_node->child->h) * ((float32)(1 - ratio));
  }

  return ok_void();
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

static result_sizing_delta
container_internal_fit_layout_callback(base_widget* widget,
                                       bool call_on_children)
{
  sizing_delta deltas = {.x = 0, .y = 0};

  return ok(result_sizing_delta, deltas);
}

static result_bool default_internal_render_callback(const base_widget* widget)
{
  // pushing clip-rect
  result_void _ = command_buffer_add_push_clip_rect_command(
    widget->context->cmd_buffer, common_internal_get_bounding_rect(widget));
  if(!_.ok)
  {
    return error(result_bool, _.error);
  }

  base_widget_child_node* temp = widget->children_head;
  while(temp)
  {
    if(temp->child->internal_render_callback)
    {
      temp->child->internal_render_callback(temp->child);
    }
    temp = temp->next;
  }

  // popping clip-rect
  _ = command_buffer_add_pop_clip_rect_command(widget->context->cmd_buffer);
  if(!_.ok)
  {
    return error(result_bool, _.error);
  }

  return ok(result_bool, true);
}

static bool split_mouse_button_down_callback(base_widget* widget,
                                             mouse_button_event event)
{
  // setting context's active draggable widget
  widget->context->active_draggable_widget = widget;

  split* v = (split*)widget->derived;

  v->private_data->state = HANDLE_CLICKED;
  v->private_data->last_clicked = (point){event.x, event.y};

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  return true;
}

static bool split_mouse_button_up_callback(base_widget* widget,
                                           mouse_button_event event)
{
  // removing context's active draggable widget
  widget->context->active_draggable_widget = NULL;

  split* v = (split*)widget->derived;

  v->private_data->state = HANDLE_NORMAL;

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  return true;
}

static bool split_mouse_enter_callback(base_widget* widget,
                                       mouse_motion_event event)
{
  split* v = (split*)widget->derived;

  v->private_data->state = HANDLE_HOVERED;

  if(((split_view*)(widget->parent->derived))->type == SPLIT_VERTICAL)
  {
    result_void _ = command_buffer_add_set_cursor_command(
      widget->context->cmd_buffer, SET_CURSOR_RESIZE_LEFT_RIGHT);
    if(!_.ok)
    {
      return false;
    }

    widget->w += 3;
  }
  else
  {
    result_void _ = command_buffer_add_set_cursor_command(
      widget->context->cmd_buffer, SET_CURSOR_RESIZE_TOP_BOTTOM);
    if(!_.ok)
    {
      return false;
    }

    widget->h += 3;
  }

  common_internal_relayout(widget->parent);
  widget->parent->internal_render_callback(widget->parent);

  return true;
}

static bool split_mouse_leave_callback(base_widget* widget,
                                       mouse_motion_event event)
{
  split* v = (split*)widget->derived;

  v->private_data->state = HANDLE_NORMAL;

  if(((split_view*)(widget->parent->derived))->type == SPLIT_VERTICAL)
  {
    widget->w -= 3;
  }
  else
  {
    widget->h -= 3;
  }

  common_internal_relayout(widget->parent);
  widget->parent->internal_render_callback(widget->parent);

  command_buffer_add_set_cursor_command(widget->context->cmd_buffer,
                                        SET_CURSOR_ARROW);

  return true;
}

static bool split_mouse_move_callback(base_widget* widget,
                                      mouse_motion_event event)
{
  split* v = (split*)widget->derived;

  if(v->private_data->state != HANDLE_CLICKED)
  {
    return false;
  }

  base_widget* parent = widget->parent;
  base_widget* first_child = parent->children_head->child;
  base_widget* second_child = parent->children_head->next->next->child;
  split_type type = ((split_view*)(parent->derived))->type;
  if(type == SPLIT_VERTICAL)
  {
    int16 delta = v->private_data->last_clicked.x - event.x;
    first_child->w -= delta;
    second_child->w += delta;
    v->private_data->ratio = (float32)first_child->w / second_child->w;
  }
  else
  {
    int16 delta = v->private_data->last_clicked.y - event.y;
    first_child->h -= delta;
    second_child->h += delta;
  }

  v->private_data->last_clicked = (point){event.x, event.y};

  common_internal_relayout(parent);

  parent->internal_render_callback(parent);

  return true;
}
