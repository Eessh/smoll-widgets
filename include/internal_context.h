#ifndef SMOLL_WIDGETS__INTERNAL_CONTEXT_H
#define SMOLL_WIDGETS__INTERNAL_CONTEXT_H

#include "backend.h"
#include "base_widget.h"
#include "command_buffer.h"
#include "internal_events.h"
#include "types.h"

/// @brief Internal context.
///        This is public to all widgets.
///        Use `internal_context_new()` to create this context.
///        Do not allocate this on stack, as there will be high possibility
///        of memory leak by not freeing UI tree when the context
///        goes out of scope.
typedef struct internal_context
{
  /// @brief Local mouse x-coordinate.
  uint16 mouse_x;

  /// @brief Local mouse y-coordinate.
  uint16 mouse_y;

  /// @brief Global mouse x-coordinate.
  uint16 global_mouse_x;

  /// @brief Global mouse y-coodinate.
  uint16 global_mouse_y;

  /// @brief Viewport width.
  uint16 viewport_w;

  /// @brief Viewport height.
  uint16 viewport_h;

  /// @brief Root widget.
  base_widget* root;

  /// @brief Overlay (or) popup widget.
  ///        If this exists, all events will be forwarded to this widget.
  ///        No events will be given to the root widget.
  base_widget* overlay_widget;

  /// @brief Active scrollbar widget.
  ///        If this exists, all mouse motion events will be forwarded
  ///        to this widget.
  base_widget* active_scrollbar;

  /// @brief Widget with keyboard focus.
  ///        If this exists, all keyboard events will be forwarded
  ///        to this widget.
  base_widget* keyboard_focused_widget;

  /// @brief Widget with mouse focus.
  ///        This is used to send mouse leave events, when mouse enters
  ///        a new widget.
  base_widget* mouse_focused_widget;

  /// @brief Default font for context.
  ///        This should be loaded by backend.
  char* font;

  /// @brief Default font size.
  uint8 font_size;

  /// @brief Command buffer.
  command_buffer* cmd_buffer;

  /// @brief Backend.
  render_backend* backend;
} internal_context;

/// @brief Internal context pointer result.
typedef struct result_internal_context_ptr
{
  bool ok;
  union
  {
    internal_context* value;
    const char* error;
  };
} result_internal_context_ptr;

/// @brief Creates a new internal context.
/// @return Internal context pointer result.
result_internal_context_ptr internal_context_create();

/// @brief Frees resources used by internal context.
/// @param context pointer to internal context.
/// @return Void result.
result_void internal_context_destroy(internal_context* context);

/// @brief Gets the deepest widget which encloses the point.
/// @param context const pointer to internal context.
/// @param x point x-coordinate.
/// @param y point y-coordinate.
/// @return Base widget pointer result.
result_base_widget_ptr internal_context_get_deepest_widget_with_point(
  const internal_context* context, uint16 x, uint16 y);

/// @brief Gets the deepest widget which encloses the point, and the widget
///        has subscribed to the event type.
/// @param context const pointer to internal context.
/// @param x point x-coordinate.
/// @param y point y-coordinate.
/// @param type event type.
/// @return Base widget pointer result.
result_base_widget_ptr
internal_context_get_deepest_widget_with_point_and_event_type(
  const internal_context* context,
  uint16 x,
  uint16 y,
  internal_event_type type);

/// @brief Processes internal mouse motion event.
/// @param context pointer to internal context.
/// @param internal_event poiinter to internal mouse motion event.
/// @return void result.
result_void internal_context_process_mouse_motion_event(
  internal_context* context, internal_mouse_motion_event* internal_event);

/// @brief Processes internal mouse button event.
/// @param context pointer to internal context.
/// @param internal_event poiinter to internal mouse button event.
/// @return Void result.
result_void internal_context_process_mouse_button_event(
  internal_context* context, internal_mouse_button_event* internal_event);

/// @brief Processes internal mouse scroll event.
/// @param context pointer to internal context.
/// @param internal_event poiinter to internal mouse scroll event.
/// @return Void result.
result_void internal_context_process_mouse_scroll_event(
  internal_context* context, internal_mouse_scroll_event* internal_event);

#endif