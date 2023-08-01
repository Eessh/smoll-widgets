#include "../include/smoll_context.h"
#include <stdlib.h>
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
  internal_context* internal_ctx;
};

result_smoll_context_ptr smoll_context_create()
{
  smoll_context* context = (smoll_context*)calloc(1, sizeof(smoll_context));
  if(!context)
  {
    return error(result_smoll_context_ptr,
                 "Unable to allocate memory for smoll context!");
  }

  result_internal_context_ptr _ = internal_context_create();
  if(!_.ok)
  {
    return error(result_smoll_context_ptr, _.error);
  }

  context->internal_ctx = _.value;

  return ok(result_smoll_context_ptr, context);
}

result_void smoll_context_destroy(smoll_context* context)
{
  if(!context)
  {
    return error(result_void, "Attempt to free a NULL pointed smoll context!");
  }

  // ignoring if any errors occurred while destroying internal context
  result_void _ = internal_context_destroy(context->internal_ctx);

  free(context);

  return ok_void();
}

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
  context->internal_ctx->root = root_widget_base;

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

  // if internal context has active scrollbar widget
  // forward all mouse motion & mouse button events to that widget
  if(context->internal_ctx->active_scrollbar)
  {
    // constructing event object.
    internal_mouse_motion_event internal_event = {
      .event = event,
      .propagation = true,
      .state = AT_TARGET,
      .target = context->internal_ctx->active_scrollbar};

    return internal_context_process_mouse_motion_event(&context->internal_ctx,
                                                       &internal_event);
  }

  // if internal context has over widget
  // forward all events to that widget
  if(context->internal_ctx->overlay_widget)
  {
    // constructing event object.
    internal_mouse_motion_event internal_event = {
      .event = event,
      .propagation = true,
      .state = AT_TARGET,
      .target = context->internal_ctx->overlay_widget};

    return internal_context_process_mouse_motion_event(&context->internal_ctx,
                                                       &internal_event);
  }

  result_base_widget_ptr _ =
    internal_context_get_deepest_widget_with_point_and_event_type(
      &context->internal_ctx, event.x, event.y, MOUSE_MOTION_INTERNAL_EVENT);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  if(!_.value)
  {
    // no widget satisfies requirements
    return ok_void();
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

  // if internal context has active scrollbar widget
  // forward all mouse motion & mouse button events to that widget
  if(context->internal_ctx->active_scrollbar)
  {
    // constructing event object.
    internal_mouse_button_event internal_event = {
      .event = event,
      .propagation = true,
      .state = AT_TARGET,
      .target = context->internal_ctx->active_scrollbar};

    return internal_context_process_mouse_button_event(&context->internal_ctx,
                                                       &internal_event);
  }

  // if internal context has over widget
  // forward all events to that widget
  if(context->internal_ctx->overlay_widget)
  {
    // constructing event object.
    internal_mouse_button_event internal_event = {
      .event = event,
      .propagation = true,
      .state = AT_TARGET,
      .target = context->internal_ctx->overlay_widget};

    return internal_context_process_mouse_button_event(&context->internal_ctx,
                                                       &internal_event);
  }

  result_base_widget_ptr _ =
    internal_context_get_deepest_widget_with_point_and_event_type(
      &context->internal_ctx, event.x, event.y, MOUSE_BUTTON_INTERNAL_EVENT);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  if(!_.value)
  {
    // no widget satisfies requirements
    return ok_void();
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

  // if internal context has over widget
  // forward all events to that widget
  if(context->internal_ctx->overlay_widget)
  {
    // constructing event object.
    internal_mouse_scroll_event internal_event = {
      .event = event, .target = context->internal_ctx->overlay_widget};

    return internal_context_process_mouse_scroll_event(&context->internal_ctx,
                                                       &internal_event);
  }

  result_base_widget_ptr _ =
    internal_context_get_deepest_widget_with_point_and_event_type(
      &context->internal_ctx,
      context->internal_ctx->mouse_x,
      context->internal_ctx->mouse_y,
      MOUSE_SCROLL_INTERNAL_EVENT);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  if(!_.value)
  {
    // no widget satisfies requirements
    return ok_void();
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

  context->internal_ctx->backend = backend;

  return ok_void();
}
