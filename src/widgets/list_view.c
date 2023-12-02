#include "../../include/widgets/list_view.h"
#include <math.h>
#include <stdlib.h>
#include "../../include/macros.h"

struct list_view_private
{
  float32 scroll_offset;
  float32 scroll_acceleration;
};

static void default_internal_derived_free_callback(base_widget* widget);

static color
default_internal_get_background_callback(const base_widget* widget);

static result_sizing_delta
default_internal_fit_layout_callback(base_widget* widget,
                                     bool call_on_children);

static result_void
defualt_internal_post_relayout_hook(const base_widget* widget);

static result_bool default_internal_render_callback(const base_widget* widget);

static bool default_mouse_scroll_callback(base_widget* widget,
                                          mouse_scroll_event event);

result_list_view_ptr list_view_new(base_widget* parent_base)
{
  list_view* view = (list_view*)calloc(1, sizeof(list_view));
  if(!view)
  {
    return error(result_list_view_ptr,
                 "Unable to allocate memory for list_view");
  }

  result_base_widget_ptr _ = base_widget_new(FLEX_CONTAINER);
  if(!_.ok)
  {
    free(view);
    return error(result_list_view_ptr, _.error);
  }

  list_view_private* view_private =
    (list_view_private*)calloc(1, sizeof(list_view_private));
  if(!view_private)
  {
    base_widget_free(_.value);
    free(view);
    return error(result_list_view_ptr,
                 "Unable to allocate memory for private fields of list-view!");
  }

  view->base = _.value;
  view->base->derived = view;
  view->base->parent = parent_base;

  view->private_data = view_private;

  if(parent_base)
  {
    // also assigns context
    base_widget_add_child(parent_base, view->base);
  }

  view->base->internal_derived_free_callback =
    default_internal_derived_free_callback;
  view->base->internal_get_background_callback =
    default_internal_get_background_callback;
  view->base->internal_fit_layout_callback =
    default_internal_fit_layout_callback;
  view->base->post_internal_relayout_hook = defualt_internal_post_relayout_hook;
  view->base->internal_render_callback = default_internal_render_callback;

  view->base->mouse_scroll_callback = default_mouse_scroll_callback;

  view->base->flexbox_data.container.direction = FLEX_DIRECTION_COLUMN;
  view->base->flexbox_data.container.align_items = ALIGN_ITEMS_START;

  view->background = (color){0, 0, 0, 255};
  view->private_data->scroll_offset = 0.0f;
  view->private_data->scroll_acceleration = 100;

  return ok(result_list_view_ptr, view);
}

result_list_view_ptr list_view_new_with_debug_name(base_widget* parent_base,
                                                   const char* debug_name)
{
  result_list_view_ptr _ = list_view_new(parent_base);
  if(!_.ok)
  {
    return _;
  }

  _.value->base->debug_name = debug_name;

  trace("List-View: created with debug-name: %s", debug_name);

  return _;
}

static void default_internal_derived_free_callback(base_widget* widget)
{
  trace("List-View(%s): internal-derived-free()", widget->debug_name);

  list_view* view = (list_view*)widget->derived;

  // freeing private data
  free(view->private_data);

  free(view);
}

static color default_internal_get_background_callback(const base_widget* widget)
{
  list_view* view = (list_view*)widget->derived;
  color bg = view->background;

  trace(
    "Flex-View(%s): internal-get-background(), background: (%d, %d, %d, %d)",
    view->base->debug_name,
    bg.r,
    bg.b,
    bg.g,
    bg.a);

  return bg;
}

static result_sizing_delta
default_internal_fit_layout_callback(base_widget* widget, bool call_on_children)
{
  trace("Flex-View(%s): internal-fit-layout()", widget->debug_name);

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

static result_void
defualt_internal_post_relayout_hook(const base_widget* widget)
{
  trace("List-View(%s): internal-post-relayout-hook()", widget->debug_name);

  list_view* view = (list_view*)widget->derived;
  float32 scroll_offset = view->private_data->scroll_offset;

  if(fabsf(scroll_offset) < 1.0f)
  {
    trace("List-View(%s): scroll-offset < 1.0f, avoiding scrolling.",
          widget->debug_name);
    view->private_data->scroll_offset = 0.0f;
    return ok_void();
  }

  base_widget_child_node* node = widget->children_head;
  while(node)
  {
    debug(
      "List-View(%s): child y-coord: %d", widget->debug_name, node->child->y);
    node->child->y += (int16)scroll_offset;
    debug("List-View(%s): child y-coord after scroll-offset: %d",
          widget->debug_name,
          node->child->y);
    node = node->next;
  }

  return ok_void();
}

static result_bool default_internal_render_callback(const base_widget* widget)
{
  list_view* view = (list_view*)widget->derived;
  color bg = view->background;
  trace("List-View(%s): internal-render-callback(), (x, y, w, h): (%d, %d, %d, "
        "%d), background: (%d, %d, %d, %d)",
        view->base->debug_name,
        widget->x,
        widget->y,
        widget->w,
        widget->h,
        bg.r,
        bg.b,
        bg.g,
        bg.a);

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
    int16 ycoord = node->child->y, height = node->child->h;
    if(ycoord + height < widget->y || ycoord > widget->y + (int16)widget->h)
    {
      node = node->next;
      continue;
    }

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

static bool default_mouse_scroll_callback(base_widget* widget,
                                          mouse_scroll_event event)
{
  trace("List-View(%s): mouse-scroll event, delta-y: %f.",
        widget->debug_name,
        event.delta_y);

  uint32 required_view_height = 0;
  base_widget_child_node* node = widget->children_head;
  while(node)
  {
    required_view_height += node->child->h + widget->flexbox_data.container.gap;
    node = node->next;
  }
  required_view_height -= widget->flexbox_data.container.gap;

  if(required_view_height < widget->h)
  {
    return false;
  }

  list_view* view = (list_view*)widget->derived;

  float32 delta_y = event.delta_y * view->private_data->scroll_acceleration;
  view->private_data->scroll_offset += (int16)delta_y;

  if(view->private_data->scroll_offset > 0.0f)
  {
    view->private_data->scroll_offset = 0.0f;
    node = widget->children_head;
    if(node && node->child->y == widget->y)
    {
      return false;
    }
    int16 y = widget->y;
    while(node)
    {
      node->child->y = y;
      y += node->child->h + widget->flexbox_data.container.gap;
      node = node->next;
    }
    widget->internal_render_callback(widget);
    return false;
  }

  node = widget->children_head;
  while(node)
  {
    node->child->y += (int16)delta_y;
    node = node->next;
  }

  result_bool _ = widget->internal_render_callback(widget);
  if(!_.ok)
  {
    return false;
  }

  return true;
}
