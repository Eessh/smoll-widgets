#include "../../include/widgets/box.h"
#include <stdlib.h>
#include "../../include/macros.h"

static rect
default_internal_get_bounding_rect_callback(const base_widget* widget);

static result_bool default_internal_fit_layout_callback(base_widget* widget);

static result_bool default_internal_render_callback(const base_widget* widget);

result_box_ptr box_new(base_widget* parent_base)
{
  box* b = (box*)calloc(1, sizeof(box));
  if(!b)
  {
    return error(result_box_ptr, "Unable to allocate memory for box widget!");
  }

  result_base_widget_ptr _ = base_widget_new();
  if(!_.ok)
  {
    free(b);
    return error(result_box_ptr, _.error);
  }

  b->base = _.value;
  b->base->derived = b;
  b->base->parent = parent_base;

  // attaching this box to parent's children list
  if(parent_base)
  {
    base_widget_add_child(parent_base, b->base);
  }

  // assigning context
  if(parent_base)
  {
    b->base->context = parent_base->context;
  }

  b->base->internal_get_bounding_rect_callback =
    default_internal_get_bounding_rect_callback;
  b->base->internal_fit_layout_callback = default_internal_fit_layout_callback;
  b->base->internal_render_callback = default_internal_render_callback;

  b->padding_x = 0;
  b->padding_y = 0;
  b->background = (color){255, 255, 255, 255};

  if(parent_base)
  {
    // calling fit layout to initialize the bounding rect properly
    b->base->internal_fit_layout_callback(b->base);
  }

  return ok(result_box_ptr, b);
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
    return error(
      result_bool,
      "Cannot process internal fit layout callback on a NULL pointed widget!");
  }

  // Box is just a static container, does nothing
  // box widget doesn't resize according to children
  // just stays the same size, until unless changed width or height
  // so on invoking fit layout callback, it just calls fit layout
  // on all of its children

  base_widget_child_node* node = widget->children_head;
  while(node)
  {
    node->child->internal_fit_layout_callback(node->child);
    node = node->next;
  }

  return ok(result_bool, true);
}

static result_bool default_internal_render_callback(const base_widget* widget)
{
  if(!widget)
  {
    return error(
      result_bool,
      "Cannot process internal render callback on a NULL pointed widget!");
  }

  box* b = (box*)widget->derived;

  result_void _ = command_buffer_add_render_rect_command(
    widget->context->cmd_buffer,
    widget->internal_get_bounding_rect_callback(widget),
    b->background);
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