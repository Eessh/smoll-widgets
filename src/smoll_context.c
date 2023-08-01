#include "../include/smoll_context.h"
#include "../include/backend.h"
#include "../include/base_widget.h"
#include "../include/internal_context.h"
#include "../include/macros.h"

/// @brief Smoll Context.
///        Acts as a wrapper for Internal Context.
///        Hides Internal Context from users, while all widgets can access
///        Internal Context publicly.
struct smoll_context
{
  /// @brief Internal Context, the actual context which holds all data of UI.
  internal_context internal_ctx;
};

result_void smoll_context_set_root_widget(smoll_context* context,
                                          base_widget* root_widget_base)
{
  if(!context)
  {
    return error(result_void,
                 "Cannot set root widget to NULL pointed context!");
  }

  if(!root_widget_base)
  {
    return error(result_void,
                 "Cannot set NULL pointed root widget to context!");
  }

  // setting context's root widget.
  context->internal_ctx.root = root_widget_base;

  // setting root widget's context.
  root_widget_base->context = &context->internal_ctx;

  return ok_void();
}

result_void smoll_context_process_mouse_motion_event(smoll_context* context,
                                                     mouse_motion_event event)
{
  if(!context)
  {
    return error(result_void,
                 "Cannot process mouse motion event on NULL pointed context!");
  }

  result_base_widget_ptr _ =
    internal_context_get_deepest_widget_with_point_and_event_type(
      &context->internal_ctx, event.x, event.y, MOUSE_MOTION);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  // constructing event object.
  internal_mouse_motion_event internal_event = {
    .event = event, .propagation = true, .state = AT_TARGET, .target = _.value};

  return internal_context_process_mouse_motion_event(&context->internal_ctx,
                                                     &internal_event);
}

result_void smoll_context_process_mouse_button_event(smoll_context* context,
                                                     mouse_button_event event)
{
  if(!context)
  {
    return error(result_void,
                 "Cannot process mouse button event on NULL pointed context!");
  }

  result_base_widget_ptr _ =
    internal_context_get_deepest_widget_with_point_and_event_type(
      &context->internal_ctx, event.x, event.y, MOUSE_BUTTON);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  // constructing event object.
  internal_mouse_button_event internal_event = {
    .event = event, .propagation = true, .state = AT_TARGET, .target = _.value};

  return internal_context_process_mouse_button_event(&context->internal_ctx,
                                                     &internal_event);
}

result_void smoll_context_process_mouse_scroll_event(smoll_context* context,
                                                     mouse_scroll_event event)
{
  if(!context)
  {
    return error(result_void,
                 "Cannot process mouse scroll event on NULL pointed context!");
  }

  result_base_widget_ptr _ =
    internal_context_get_deepest_widget_with_point_and_event_type(
      &context->internal_ctx,
      context->internal_ctx.mouse_x,
      context->internal_ctx.mouse_y,
      MOUSE_MOTION);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  // constructing event object.
  internal_mouse_scroll_event internal_event = {.event = event,
                                                .target = _.value};

  return internal_context_process_mouse_scroll_event(&context->internal_ctx,
                                                     &internal_event);
}

result_void smoll_context_register_backend(smoll_context* context,
                                           render_backend* backend)
{
  if(!context)
  {
    return error(result_void,
                 "Cannot register backend to NULL pointing context!");
  }

  if(!backend)
  {
    return error(result_void,
                 "Cannot register NULL pointed backend to context!");
  }

  context->internal_ctx.backend = backend;

  return ok_void();
}

result_void base_widget_recursive_free(base_widget* widget)
{
  if(!widget)
  {
    return error(result_void, "Attempt to free a NULL pointed base widget!");
  }

  if(widget->children_head)
  {
    base_widget_child_node* prev_node = widget->children_head;
    base_widget_child_node* node = prev_node->next;
    while(node)
    {
      result_void _ = base_widget_recursive_free(prev_node->child);
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
    result_void _ = base_widget_recursive_free(prev_node->child);
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

  result_void _ = base_widget_free(widget);
  if(!_.ok)
  {
    return _;
  }

  return ok_void();
}