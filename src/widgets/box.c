#include "../../include/widgets/box.h"
#include <stdbool.h>
#include <stdlib.h>
#include "../../include/macros.h"

static color
default_internal_get_background_callback(const base_widget* widget);

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

result_box_ptr box_new(base_widget* parent_base, flex_direction direction)
{
  box* b = (box*)calloc(1, sizeof(box));
  if(!b)
  {
    return error(result_box_ptr, "Unable to allocate memory for box widget!");
  }

  result_base_widget_ptr _ = base_widget_new(FLEX_CONTAINER);
  if(!_.ok)
  {
    free(b);
    return error(result_box_ptr, _.error);
  }

  b->base = _.value;
  b->base->derived = b;
  b->base->parent = parent_base;

  // attaching this box to parent's children list
  // also handles assigning context
  if(parent_base)
  {
    base_widget_add_child(parent_base, b->base);
  }

  b->base->internal_get_background_callback =
    default_internal_get_background_callback;
  b->base->internal_fit_layout_callback = default_internal_fit_layout_callback;
  b->base->internal_render_callback = default_internal_render_callback;

  b->base->flexbox_data.container.direction = direction;

  b->debug_name = "Box";
  b->padding_x = 0;
  b->padding_y = 0;
  b->background = (color){255, 255, 255, 255};

  return ok(result_box_ptr, b);
}

result_box_ptr box_new_with_debug_name(base_widget* parent_base,
                                       flex_direction direction,
                                       const char* debug_name)
{
  result_box_ptr _ = box_new(parent_base, direction);
  if(!_.ok)
  {
    return _;
  }

  _.value->debug_name = debug_name;

  trace("Box: created with debug-name: %s", debug_name);

  return _;
}

static color default_internal_get_background_callback(const base_widget* widget)
{
  box* b = (box*)widget->derived;
  color bg = b->background;

  trace("Box(%s): internal-get-background(), background-color: %d, %d, %d, %d",
        b->debug_name,
        bg.r,
        bg.b,
        bg.g,
        bg.a);

  return bg;
}

static result_sizing_delta
default_internal_fit_layout_callback(base_widget* widget, bool call_on_children)
{
  box* b = (box*)widget->derived;
  trace("Box(%s): internal-fit-layout(), sizing-deltas: 0, 0", b->debug_name);

  // Box is just a static container, does nothing
  // box widget doesn't resize according to children
  // just stays the same size, until unless changed width or height
  // so on invoking fit layout callback, it just calls fit layout
  // on all of its children

  if(call_on_children)
  {
    base_widget_child_node* node = widget->children_head;
    while(node)
    {
      node->child->internal_fit_layout_callback(node->child, true);
      node = node->next;
    }
  }

  sizing_delta deltas = {.x = 0, .y = 0};

  return ok(result_sizing_delta, deltas);
}

static result_bool default_internal_render_callback(const base_widget* widget)
{
  box* b = (box*)widget->derived;
  color bg = b->background;

  trace(
    "Box(%s): internal-render(), (x, y, w, h): (%d, %d, %d, %d), background: "
    "(%d, %d, %d, %d)",
    b->debug_name,
    widget->x,
    widget->y,
    widget->w,
    widget->h,
    bg.r,
    bg.b,
    bg.g,
    bg.a);

  // rect bounding_rect = widget->internal_get_bounding_rect_callback(widget);
  rect bounding_rect = common_internal_get_bounding_rect(widget);

  result_void _ = command_buffer_add_render_rect_command(
    widget->context->cmd_buffer, bounding_rect, bg);
  if(!_.ok)
  {
    return error(result_bool, _.error);
  }

  // pushing clip-rect
  _ = command_buffer_add_push_clip_rect_command(widget->context->cmd_buffer,
                                                bounding_rect);
  if(!_.ok)
  {
    return error(result_bool, _.error);
  }

  // rendering children
  base_widget_child_node* node = widget->children_head;
  while(node)
  {
    node->child->internal_render_callback(node->child);
    node = node->next;
  }

  // popping clip-rect
  _ = command_buffer_add_pop_clip_rect_command(widget->context->cmd_buffer);
  if(!_.ok)
  {
    return error(result_bool, _.error);
  }

  return ok(result_bool, true);
}
