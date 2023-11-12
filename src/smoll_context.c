#include "../include/smoll_context.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/backend.h"
#include "../include/base_widget.h"
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

result_smoll_context_ptr smoll_context_create(uint16 viewport_width,
                                              uint16 viewport_height)
{
  smoll_context* context = (smoll_context*)calloc(1, sizeof(smoll_context));
  if(!context)
  {
    return error(result_smoll_context_ptr,
                 "Unable to allocate memory for smoll context!");
  }

  result_internal_context_ptr _ =
    internal_context_create(viewport_width, viewport_height);
  if(!_.ok)
  {
    free(context);
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

result_void smoll_context_set_default_font(smoll_context* context,
                                           const char* font,
                                           uint8 font_size)
{
  if(!context)
  {
    return error(result_void,
                 "Cannot set default font for a NULL pointing smoll context!");
  }

  if(!font)
  {
    return error(result_void,
                 "Cannot set NULL pointing font to smoll context!");
  }

  char* font_copy = strdup(font);
  if(!font_copy)
  {
    return error(result_void, "Unable to make a copy of font!");
  }

  context->internal_ctx->font = font_copy;
  context->internal_ctx->font_size = font_size;

  if(context->internal_ctx->backend)
  {
    context->internal_ctx->backend->load_font(font, font_size);
  }

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
  root_widget_base->context = context->internal_ctx;

  root_widget_base->w = context->internal_ctx->viewport_w;
  root_widget_base->h = context->internal_ctx->viewport_h;

  return ok_void();
}

result_void
smoll_context_process_viewport_resize_event(smoll_context* context,
                                            viewport_resize_event event)
{
  if(!context)
  {
    return error(
      result_void,
      "Cannot process viewport resize event on NULL pointed context!");
  }

  context->internal_ctx->viewport_w = event.w;
  context->internal_ctx->viewport_h = event.h;

  command_buffer_add_clear_window_command(context->internal_ctx->cmd_buffer);

  base_widget* root = context->internal_ctx->root;
  root->w = event.w;
  root->h = event.h;

  // root->internal_calculate_size(root);
  common_internal_calculate_size(root);
  root->internal_relayout(root);
  root->internal_render_callback(root);

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
  if(context->internal_ctx->active_draggable_widget)
  {
    // constructing event object.
    internal_mouse_motion_event internal_event = {
      .event = event,
      .propagation = true,
      .state = AT_TARGET,
      .target = context->internal_ctx->active_draggable_widget};

    return internal_context_process_mouse_motion_event(context->internal_ctx,
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

    return internal_context_process_mouse_motion_event(context->internal_ctx,
                                                       &internal_event);
  }

  result_base_widget_ptr _ =
    internal_context_get_deepest_widget_with_point_and_event_type(
      context->internal_ctx, event.x, event.y, MOUSE_MOTION_INTERNAL_EVENT);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  if(!_.value)
  {
    // root widget itself doesn't contain the point
    // calling mouse leave callback if internal context has mouse focused widget
    if(context->internal_ctx->mouse_focused_widget &&
       context->internal_ctx->mouse_focused_widget->mouse_leave_callback)
    {
      context->internal_ctx->mouse_focused_widget->mouse_leave_callback(
        context->internal_ctx->mouse_focused_widget, event);
    }
    // setting internal context's mouse focused widget to NULL
    // as mouse is outside the viewport
    context->internal_ctx->mouse_focused_widget = NULL;
    return ok_void();
  }

  // constructing event object.
  internal_mouse_motion_event internal_event = {
    .event = event, .propagation = true, .state = AT_TARGET, .target = _.value};

  return internal_context_process_mouse_motion_event(context->internal_ctx,
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
  if(context->internal_ctx->active_draggable_widget)
  {
    // constructing event object.
    internal_mouse_button_event internal_event = {
      .event = event,
      .propagation = true,
      .state = AT_TARGET,
      .target = context->internal_ctx->active_draggable_widget};

    return internal_context_process_mouse_button_event(context->internal_ctx,
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

    return internal_context_process_mouse_button_event(context->internal_ctx,
                                                       &internal_event);
  }

  result_base_widget_ptr _ =
    internal_context_get_deepest_widget_with_point_and_event_type(
      context->internal_ctx, event.x, event.y, MOUSE_BUTTON_INTERNAL_EVENT);
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

  return internal_context_process_mouse_button_event(context->internal_ctx,
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

    return internal_context_process_mouse_scroll_event(context->internal_ctx,
                                                       &internal_event);
  }

  result_base_widget_ptr _ =
    internal_context_get_deepest_widget_with_point_and_event_type(
      context->internal_ctx,
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

  return internal_context_process_mouse_scroll_event(context->internal_ctx,
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

result_void smoll_context_initial_render(smoll_context* context)
{
  if(!context)
  {
    return error(
      result_void,
      "Cannot perform initial UI render of context pointing to NULL!");
  }

  if(context->internal_ctx->root->internal_render_callback)
  {
    context->internal_ctx->root->internal_render_callback(
      context->internal_ctx->root);
  }

  return smoll_context_render(context);
}

result_void smoll_context_initial_fit_layout(smoll_context* context)
{
  if(!context)
  {
    return error(
      result_void,
      "Cannot perform initial UI layouting of context pointing to NULL!");
  }

  base_widget* root = context->internal_ctx->root;

  if(root->internal_fit_layout_callback)
  {
    root->internal_fit_layout_callback(root, true);
  }

  if(root->internal_assign_positions)
  {
    root->internal_assign_positions(root);
  }

  return ok_void();
}

result_void smoll_context_initialize_layout(smoll_context* context)
{
  if(!context)
  {
    return error(result_void,
                 "Cannot initialize sizing of context pointing to NULL!");
  }

  base_widget* root = context->internal_ctx->root;

  // result_void _ = root->internal_calculate_size(root);
  result_void _ = common_internal_calculate_size(root);
  if(!_.ok)
  {
    return _;
  }

  return root->internal_relayout(root);
}

result_void smoll_context_render(smoll_context* context)
{
  if(!context)
  {
    return error(result_void, "Cannot render UI of context pointing to NULL!");
  }

  if(!context->internal_ctx->backend)
  {
    return error(result_void,
                 "No backend is registered to process the command!");
  }

  if(!context->internal_ctx->backend->process_command)
  {
    return error(
      result_void,
      "Registered backend doesn't contain process command callback function!");
  }

  while(command_buffer_length(context->internal_ctx->cmd_buffer) > 0)
  {
    command* cmd =
      command_buffer_get_next_command(context->internal_ctx->cmd_buffer).value;

    // forwarding command to backend for processing
    context->internal_ctx->backend->process_command(cmd);

    // freeing command after processing
    command_free(cmd);
  }

  return ok_void();
}
