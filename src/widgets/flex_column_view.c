#include "../../include/widgets/flex_column_view.h"
#include <stdlib.h>
#include "../../include/macros.h"

static color
default_internal_get_background_callback(const base_widget* widget);

static result_sizing_delta
default_internal_fit_layout_callback(base_widget* widget,
                                     bool call_on_children);

// static result_bool default_internal_adjust_layout_callback(base_widget* widget);

static result_bool
default_internal_assign_positions_callback(base_widget* widget);

static result_bool default_internal_render_callback(const base_widget* widget);

result_flex_column_view flex_column_view_new(base_widget* parent_base)
{
  flex_column_view* v = (flex_column_view*)calloc(1, sizeof(flex_column_view));
  if(!v)
  {
    return error(result_flex_column_view,
                 "Unable to allocate memory for flex-column view widget!");
  }

  result_base_widget_ptr _ = base_widget_new(FLEX_CONTAINER);
  if(!_.ok)
  {
    free(v);
    return error(result_flex_column_view, _.error);
  }

  v->base = _.value;
  v->base->derived = v;
  v->base->parent = parent_base;

  if(parent_base)
  {
    base_widget_add_child(parent_base, v->base);
    v->base->context = parent_base->context;
  }

  v->base->internal_get_background_callback =
    default_internal_get_background_callback;
  v->base->internal_fit_layout_callback = default_internal_fit_layout_callback;
  // v->base->internal_adjust_layout_callback =
  //   default_internal_adjust_layout_callback;
  v->base->internal_assign_positions =
    default_internal_assign_positions_callback;
  v->base->internal_render_callback = default_internal_render_callback;

  v->padding_x = 0;
  v->padding_y = 0;
  v->gap = 0;

  v->background = (color){0, 0, 0, 255};

  return ok(result_flex_column_view, v);
}

static color default_internal_get_background_callback(const base_widget* widget)
{
  flex_column_view* v = (flex_column_view*)widget->derived;
  return v->background;
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

  flex_column_view* v = (flex_column_view*)widget->derived;
  uint16 total_height = 2 * v->padding_y, max_width = 0;

  base_widget_child_node* node = widget->children_head;
  while(node)
  {
    total_height += node->child->h + v->gap;
    max_width = max(max_width, node->child->w);
    node = node->next;
  }
  total_height -= v->gap;

  widget->w = max_width + 2 * v->padding_x;
  widget->h = total_height;

  sizing_delta deltas = {.x = max_width + 2 * v->padding_x - widget->w,
                         .y = total_height - widget->h};

  return ok(result_sizing_delta, deltas);
}

// static result_bool default_internal_adjust_layout_callback(base_widget* widget)
// {
//   result_bool _ = widget->internal_fit_layout_callback(widget, false);
//   if(!_.ok)
//   {
//     return _;
//   }
//
//   if(_.value && widget->parent &&
//      widget->parent->internal_adjust_layout_callback)
//   {
//     return widget->parent->internal_adjust_layout_callback(widget->parent);
//   }
//
//   widget->internal_assign_positions(widget);
//   widget->internal_render_callback(widget);
//
//   return ok(result_bool, true);
// }

static result_bool
default_internal_assign_positions_callback(base_widget* widget)
{
  flex_column_view* v = (flex_column_view*)widget->derived;
  int16 y = widget->y + v->padding_y;
  base_widget_child_node* node = widget->children_head;
  while(node)
  {
    node->child->x = v->padding_x;
    node->child->y = y;
    if(node->child->internal_assign_positions)
    {
      node->child->internal_assign_positions(node->child);
    }
    y += node->child->y + v->gap;
    node = node->next;
  }

  return ok(result_bool, true);
}

static result_bool default_internal_render_callback(const base_widget* widget)
{
  flex_column_view* v = (flex_column_view*)widget->derived;
  result_void _ = command_buffer_add_render_rect_command(
    widget->context->cmd_buffer,
    widget->internal_get_bounding_rect_callback(widget),
    v->background);
  if(!_.ok)
  {
    return error(result_bool, _.error);
  }

  base_widget_child_node* node = widget->children_head;
  while(node)
  {
    node->child->internal_render_callback(node->child);
    node = node->next;
  }

  return ok(result_bool, true);
}
