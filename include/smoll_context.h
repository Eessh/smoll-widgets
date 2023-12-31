#ifndef SMOLL_WIDGETS__SMOLL_CONTEXT_H
#define SMOLL_WIDGETS__SMOLL_CONTEXT_H

#include "base_widget.h"
#include "events.h"

/// @brief Smoll Context.
///        Holds all the data of UI, processes events.
///        Use `smoll_context_new()` to create this context.
///        Do not allocate this on stack, as there will be high possibility
///        of memory leak by not freeing UI tree when the context
///        goes out of scope.
typedef struct smoll_context smoll_context;

/// @brief Smoll context pointer result.
typedef struct result_smoll_context_ptr
{
  bool ok;
  union
  {
    smoll_context* value;
    const char* error;
  };
} result_smoll_context_ptr;

/// @brief Creates smoll context.
/// @return Smoll context pointer result.
result_smoll_context_ptr smoll_context_create(uint16 viewport_width,
                                              uint16 viewport_height);

/// @brief Frees resources used by smoll context.
/// @param context pointer to smoll context.
/// @return Void result.
result_void smoll_context_destroy(smoll_context* context);

/// @brief Sets default (or) fallback font for smoll context.
///        Make sure the backend is attached to smoll context before calling
///        this function.
/// @param context pointer to smoll context.
/// @param font name of font.
/// @return Void result.
result_void smoll_context_set_default_font(smoll_context* context,
                                           const char* font,
                                           uint8 font_size);

/// @brief Sets root widget for smoll context.
///        Root widget must be set first. Assigning of widgets follows a order,
///        it starts from top of UI tree, assign root widget to context, then
///        assign children to this root widget, then assign children to root
///        widget's children and so on ...
/// @param context pointer to smoll context.
/// @param widget pointer to base widget of root widget.
/// @return Void result.
result_void smoll_context_set_root_widget(smoll_context* context,
                                          base_widget* root_widget_base);

result_void
smoll_context_process_viewport_resize_event(smoll_context* context,
                                            viewport_resize_event event);

/// @brief Processes the mouse motion event on the given smoll context.
///        The event should be externally constructed by the backend provider.
///        Smoll context, cannot construct events itself, it just processes
///        the events it got from the event loop of backend.
/// @param context pointer to smoll context.
/// @param event mouse motion event struct.
/// @return Void result.
result_void smoll_context_process_mouse_motion_event(smoll_context* context,
                                                     mouse_motion_event event);

/// @brief Processes the mouse button event on the given smoll context.
///        The event should be externally constructed by the backend provider.
///        Smoll context, cannot construct events itself, it just processes
///        the events it got from the event loop of backend.
/// @param context pointer to smoll context.
/// @param event mouse motion event struct.
/// @return Void result.
result_void smoll_context_process_mouse_button_event(smoll_context* context,
                                                     mouse_button_event event);

/// @brief Processes the mouse scroll event on the given smoll context.
///        The event should be externally constructed by the backend provider.
///        Smoll context, cannot construct events itself, it just processes
///        the events it got from the event loop of backend.
/// @param context pointer to smoll context.
/// @param event mouse motion event struct.
/// @return Void result.
result_void smoll_context_process_mouse_scroll_event(smoll_context* context,
                                                     mouse_scroll_event event);

/// @brief Registers render backend to this context.
/// @param context pointer to smoll context.
/// @param backend pointer to render backend.
/// @return Void result.
result_void smoll_context_register_backend(smoll_context* context,
                                           render_backend* backend);

/// @brief First render of UI Tree.
///        Call this function only after setting root widget of smoll context.
///        This should be called after:
/// @code smoll_context_initial_fit_layout() @endcode
/// @param context pointer to smoll context.
/// @return Void result.
result_void smoll_context_initial_render(smoll_context* context);

result_void smoll_context_initialize_layout(smoll_context* context);

/// @brief Renders UI using commands from command buffer.
/// @param context pointer to context.
/// @return Void result.
result_void smoll_context_render(smoll_context* context);

result_void
smoll_context_render_send_cmd_buffer_to_backend(smoll_context* context);

#endif