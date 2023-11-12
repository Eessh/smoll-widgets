#include "../../include/widgets/flex_view.h"
#include <stdlib.h>
#include "../../include/macros.h"

static color
default_internal_get_background_callback(const base_widget* widget);

static result_sizing_delta
default_internal_fit_layout_callback(base_widget* widget,
                                     bool call_on_children);

static result_bool default_internal_render_callback(const base_widget* widget);

result_flex_view_ptr flex_view_new(base_widget* parent_base,
                                   flex_direction direction)
{
  flex_view* view = (flex_view*)calloc(1, sizeof(flex_view));
  if(!view)
  {
    return error(result_flex_view_ptr,
                 "Unable to allocate memory for flex_view!");
  }

  result_base_widget_ptr _ = base_widget_new(FLEX_CONTAINER);
  if(!_.ok)
  {
    free(view);
    return error(result_flex_view_ptr, _.error);
  }

  view->base = _.value;
  view->base->derived = view;
  view->base->parent = parent_base;

  if(parent_base)
  {
    // also assigns context
    base_widget_add_child(parent_base, view->base);
  }

  view->base->internal_get_background_callback =
    default_internal_get_background_callback;
  view->base->internal_fit_layout_callback =
    default_internal_fit_layout_callback;
  view->base->internal_render_callback = default_internal_render_callback;

  view->base->flexbox_data.container.direction = direction;

  view->background = (color){0, 0, 0, 255};

  return ok(result_flex_view_ptr, view);
}

result_flex_view_ptr flex_view_new_with_debug_name(base_widget* parent_base,
                                                   flex_direction direction,
                                                   const char* debug_name)
{
  result_flex_view_ptr _ = flex_view_new(parent_base, direction);
  if(!_.ok)
  {
    return _;
  }

  // _.value->debug_name = strdup(debug_name);
  _.value->debug_name = debug_name;

  trace("Flex-View: created with debug-name: %s", debug_name);

  return _;
}

static color default_internal_get_background_callback(const base_widget* widget)
{
  flex_view* view = (flex_view*)widget->derived;
  color bg = view->background;

  info("Flex-View(%s): internal-get-background(), background: (%d, %d, %d, %d)",
       view->debug_name,
       bg.r,
       bg.b,
       bg.g,
       bg.a);

  return bg;
}

static result_sizing_delta
default_internal_fit_layout_callback(base_widget* widget, bool call_on_children)
{
  flex_view* view = (flex_view*)widget->derived;
  info("Flex-View(%s): internal-fit-layout()", view->debug_name);

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

  uint16 total_width = 0, max_height = 0;
  base_widget_child_node* node = widget->children_head;
  while(node)
  {
    total_width += node->child->w + widget->flexbox_data.container.gap;
    max_height = max(max_height, node->child->h);
    node = node->next;
  }
  total_width -= widget->flexbox_data.container.gap;

  sizing_delta deltas = {.x = total_width - widget->w,
                         .y = max_height - widget->h};

  widget->w = total_width;
  widget->h = max_height;

  info("  > sizing-deltas: (%d, %d)", deltas.x, deltas.y);

  return ok(result_sizing_delta, deltas);
}

static result_bool default_internal_render_callback(const base_widget* widget)
{
  flex_view* view = (flex_view*)widget->derived;
  color bg = view->background;
  info("Flex-View(%s): internal-render-callback(), (x, y, w, h): (%d, %d, %d, "
       "%d), background: (%d, %d, %d, %d)",
       view->debug_name,
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
