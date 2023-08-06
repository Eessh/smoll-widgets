#include <stdlib.h>
#include "../include/base_widget.h"
#include "../include/command_buffer.h"
#include "../include/macros.h"

result_internal_context_ptr internal_context_create()
{
  internal_context* context =
    (internal_context*)calloc(1, sizeof(internal_context));
  if(!context)
  {
    return error(result_internal_context_ptr,
                 "Unable to allocate memory for internal context!");
  }

  context->mouse_x = 0;
  context->mouse_y = 0;

  context->global_mouse_x = 0;
  context->global_mouse_y = 0;

  context->viewport_w = 0;
  context->viewport_h = 0;

  context->root = NULL;
  context->overlay_widget = NULL;
  context->active_scrollbar = NULL;
  context->keyboard_focused_widget = NULL;
  context->mouse_focused_widget = NULL;

  context->font = NULL;
  context->font_size = 0;

  // creating command buffer
  result_command_buffer_ptr _ = command_buffer_new();
  if(!_.ok)
  {
    return error(result_internal_context_ptr, _.error);
  }
  context->cmd_buffer = _.value;

  context->backend = NULL;

  return ok(result_internal_context_ptr, context);
}

result_void recursively_free_ui_tree(base_widget* root)
{
  if(!root)
  {
    return error(result_void, "Attempt to free a NULL pointed base widget!");
  }

  if(root->children_head)
  {
    base_widget_child_node* prev_node = root->children_head;
    base_widget_child_node* node = prev_node->next;
    while(node)
    {
      result_void _ = recursively_free_ui_tree(prev_node->child);
      if(!_.ok)
      {
        return _;
      }
      _ = base_widget_child_node_free(prev_node);
      if(!_.ok)
      {
        return _;
      }
      prev_node = node;
      node = node->next;
    }
    result_void _ = recursively_free_ui_tree(prev_node->child);
    if(!_.ok)
    {
      return _;
    }
    _ = base_widget_child_node_free(prev_node);
    if(!_.ok)
    {
      return _;
    }
  }

  result_void _ = base_widget_free(root);
  if(!_.ok)
  {
    return _;
  }

  return ok_void();
}

result_void internal_context_destroy(internal_context* context)
{
  if(!context)
  {
    return error(result_void,
                 "Attempt to free a NULL pointed internal context!");
  }

  // ignoring errors while freeing UI tree
  // result_void _ = recursively_free_ui_tree(context->root);
  context->root->internal_free_callback(context->root);

  free(context->font);

  // ignoring errors while freeing comand buffer
  result_void _ = command_buffer_free(context->cmd_buffer);

  free(context);

  return ok_void();
}

result_bool widget_encloses_point(base_widget* widget, uint16 x, uint16 y)
{
  if(!widget)
  {
    return (result_bool){.ok = false,
                         .error = "Cannot check if widget encloses point, for "
                                  "a NULL pointing widget!"};
  }

  rect bounding_rect = widget->internal_get_bounding_rect_callback(widget);

  return (result_bool){
    .ok = true,
    .value = (bounding_rect.x <= x && x <= bounding_rect.x + bounding_rect.w &&
              bounding_rect.y <= y && y <= bounding_rect.y + bounding_rect.h)};
}

result_base_widget_ptr
recursive_deepest_widget_with_point(base_widget* widget, uint16 x, uint16 y)
{
  if(!widget)
  {
    return error(result_base_widget_ptr,
                 "Cannot get deepest child with given point, in a NULL "
                 "pointing base widget!");
  }

  result_bool _ = widget_encloses_point(widget, x, y);
  if(!_.ok)
  {
    return error(result_base_widget_ptr, _.error);
  }

  // this case doesn't occur as we are checking the root,
  // then call this recursive function.
  // if(!_.value)
  // {
  //   // widget doesn't contain point
  //   return error(result_base_widget_ptr, "Widget doesn't contain point, cannot get deepest child!");
  // }

  if(!widget->children_head)
  {
    // as there are no children, we return this widget as the deepest
    return ok(result_base_widget_ptr, widget);
  }

  base_widget_child_node* node = widget->children_head;
  while(node)
  {
    result_bool _ = widget_encloses_point(node->child, x, y);
    if(!_.ok)
    {
      return error(result_base_widget_ptr, _.error);
    }
    if(_.value)
    {
      result_base_widget_ptr __ =
        recursive_deepest_widget_with_point(node->child, x, y);
      if(!__.ok)
      {
        return __;
      }
      return ok(result_base_widget_ptr, __.value);
    }
    node = node->next;
  }

  // none of the children contains the point
  // so returning this widget as the deepest widget
  return ok(result_base_widget_ptr, widget);
}

result_base_widget_ptr internal_context_get_deepest_widget_with_point(
  const internal_context* context, uint16 x, uint16 y)
{
  if(!context)
  {
    return error(result_base_widget_ptr,
                 "Cannot get deepest widget which encloses the point from NULL "
                 "pointed internal context!");
  }

  if(!context->root)
  {
    return error(result_base_widget_ptr,
                 "No root widget is set to context, hence cannot get deepest "
                 "widget which encloses the given point!");
  }

  result_bool _ = widget_encloses_point(context->root, x, y);
  if(!_.ok)
  {
    return error(result_base_widget_ptr, _.error);
  }

  if(!_.value)
  {
    // root widget doesn't contain point
    return error(result_base_widget_ptr,
                 "Root widget doesn't contain the point, hence cannot get "
                 "deepest widget which encloses the point!");
  }

  // widget contains the point
  if(!context->root->children_head)
  {
    // widget has no children, so root is the deepest widget.
    return ok(result_base_widget_ptr, context->root);
  }

  return recursive_deepest_widget_with_point(context->root, x, y);
}

result_bool widget_has_valid_mouse_motion_callbacks(base_widget* widget)
{
  if(!widget)
  {
    return error(result_bool,
                 "Cannot check for valid mouse motion callbacks of a NULL "
                 "pointed widget!");
  }

  return ok(
    result_bool,
    (bool)(widget->mouse_enter_callback || widget->mouse_leave_callback));
}

result_bool widget_has_valid_mouse_button_callbacks(base_widget* widget)
{
  if(!widget)
  {
    return error(result_bool,
                 "Cannot check for valid mouse button callbacks of a NULL "
                 "pointed widget!");
  }

  return ok(result_bool,
            (bool)(widget->mouse_button_down_callback ||
                   widget->mouse_button_up_callback));
}

result_bool widget_has_valid_mouse_scroll_callback(base_widget* widget)
{
  if(!widget)
  {
    return error(
      result_bool,
      "Cannot check for valid mouse scroll callback of a NULL pointed widget!");
  }

  return ok(result_bool, (bool)(widget->mouse_scroll_callback));
}

result_bool
widget_has_valid_callback_for_even_type(base_widget* widget,
                                        internal_event_type event_type)
{
  if(!widget)
  {
    return error(result_bool,
                 "Cannot check for a valid callback for event type of a  NULL "
                 "pointed widget!");
  }

  switch(event_type)
  {
  case MOUSE_MOTION_INTERNAL_EVENT:
    return widget_has_valid_mouse_motion_callbacks(widget);
  case MOUSE_BUTTON_INTERNAL_EVENT:
    return widget_has_valid_mouse_button_callbacks(widget);
  case MOUSE_SCROLL_INTERNAL_EVENT:
    return widget_has_valid_mouse_scroll_callback(widget);
  default:
    break;
  }

  return error(result_bool, "Unknown event type!");
}

result_base_widget_ptr recursive_deepest_widget_with_point_and_event_callbacks(
  base_widget* widget, uint16 x, uint16 y, internal_event_type event_type)
{
  if(!widget)
  {
    return error(result_base_widget_ptr,
                 "Cannot get deepest child with given point, in a NULL "
                 "pointing base widget!");
  }

  result_bool _ = widget_encloses_point(widget, x, y);
  if(!_.ok)
  {
    return error(result_base_widget_ptr, _.error);
  }

  // this case doesn't occur as we are checking the root,
  // then call this recursive function.
  // if(!_.value)
  // {
  //   // widget doesn't contain point
  //   return error(result_base_widget_ptr, "Widget doesn't contain point, cannot get deepest child!");
  // }

  if(!widget->children_head &&
     widget_has_valid_callback_for_even_type(widget, event_type).value)
  {
    // as there are no children, we return this widget as the deepest
    return ok(result_base_widget_ptr, widget);
  }

  base_widget_child_node* node = widget->children_head;
  while(node)
  {
    result_bool _ = widget_encloses_point(node->child, x, y);
    if(!_.ok)
    {
      return error(result_base_widget_ptr, _.error);
    }
    if(_.value)
    {
      result_base_widget_ptr __ =
        recursive_deepest_widget_with_point_and_event_callbacks(
          node->child, x, y, event_type);
      if(!__.ok)
      {
        return __;
      }
      return ok(result_base_widget_ptr, __.value);
    }
    node = node->next;
  }

  // none of the children contains the point

  if(!widget_has_valid_callback_for_even_type(widget, event_type).value)
  {
    return ok(result_base_widget_ptr, NULL);
  }

  return ok(result_base_widget_ptr, widget);
}

result_base_widget_ptr
internal_context_get_deepest_widget_with_point_and_event_type(
  const internal_context* context,
  uint16 x,
  uint16 y,
  internal_event_type event_type)
{
  if(!context)
  {
    return error(
      result_base_widget_ptr,
      "Cannot get deepest widget which encloses the point and has registered "
      "callback for given event type from NULL pointed internal context!");
  }

  if(!context->root)
  {
    return error(result_base_widget_ptr,
                 "No root widget is set to context, hence cannot get deepest "
                 "widget which encloses the given point!");
  }

  result_bool _ = widget_encloses_point(context->root, x, y);
  if(!_.ok)
  {
    return error(result_base_widget_ptr, _.error);
  }

  if(!_.value)
  {
    // root widget doesn't contain point
    return ok(result_base_widget_ptr, NULL);
  }

  // root contains the point
  return recursive_deepest_widget_with_point_and_event_callbacks(
    context->root, x, y, event_type);
}

result_void
internal_context_process_mouse_motion_event(internal_context* context,
                                            internal_mouse_motion_event* event)
{
  if(!context)
  {
    return error(result_void,
                 "Cannot process internal mouse motion event on a NULL pointed "
                 "internal context!");
  }

  event->target->internal_mouse_motion_callback(event->target, event);

  return ok_void();
}

result_void
internal_context_process_mouse_button_event(internal_context* context,
                                            internal_mouse_button_event* event)
{
  if(!context)
  {
    return error(result_void,
                 "Cannot process internal mouse button event on a NULL pointed "
                 "internal context!");
  }

  event->target->internal_mouse_button_callback(event->target, event);

  return ok_void();
}

result_void
internal_context_process_mouse_scroll_event(internal_context* context,
                                            internal_mouse_scroll_event* event)
{
  if(!context)
  {
    return error(result_void,
                 "Cannot process internal mouse scroll event on a NULL pointed "
                 "internal context!");
  }

  event->target->internal_mouse_scroll_callback(event->target, event);

  return ok_void();
}
