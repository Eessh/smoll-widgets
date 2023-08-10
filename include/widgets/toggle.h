#ifndef SMOLL_WIDGETS__TOGGLE_H
#define SMOLL_WIDGETS__TOGGLE_H

#include "../base_widget.h"

/// @brief Toggle widget private data.
typedef struct toggle_private toggle_private;

/// @brief Toggle Widget.
///        Use `toggle_new()` to create this widget.
///        Children should not be attached to this widget.
///        This should not be root widget.
typedef struct toggle
{
  /// @brief The fractional amount handle width should be of toggle
  ///        widget width.
  ///        Default value: 0.5.
  ///        Valid Range: 0.1 - 0.9
  float32 handle_width_fraction;

  /// @brief X-axis padding between toggle widget and it's handle.
  ///        This will be the value for both padding-left
  ///        and padding-right.
  uint16 padding_x;

  /// @brief Y-axis padding between toggle widget and it's handle.
  ///        This will be the value for both padding-top
  ///        and padding-bottom.
  uint16 padding_y;

  /// @brief Color of handle when toggle widget is in ON state.
  color on_handle_color;

  /// @brief Background color of toggle widget when it is in ON state.
  color on_background;

  /// @brief Color of handle when toggle widget is in OFF state.
  color off_handle_color;

  /// @brief Background color of toggle widget when it is in OFF state.
  color off_background;

  /// @brief Pointer to base widget of toggle.
  base_widget* base;

  /// @brief Pointer to private data of toggle.
  toggle_private* private_data;
} toggle;

/// @brief Toggle pointer result.
typedef struct result_toggle_ptr
{
  bool ok;
  union
  {
    toggle* value;
    const char* error;
  };
} result_toggle_ptr;

/// @brief Creates new toggle widget.
///        Once attached to UI tree, on destroying the smoll_context all
///        widgets get automatically freed.
/// @param parent_base pointer to base widget of parent.
/// @return Toggle pointer result.
result_toggle_ptr toggle_new(base_widget* parent_base);

/// @brief Sets callback function for toggle ON event.
/// @param t pointer to toggle.
/// @param callback the function to call when toggle turns ON.
/// @return Void result.
result_void toggle_set_on_callback(toggle* t, void (*callback)(toggle*));

/// @brief Sets callback function for toggle OFF event.
/// @param t pointer to toggle.
/// @param callback the function to call when toggle turns OFF.
/// @return Void result.
result_void toggle_set_off_callback(toggle* t, void (*callback)(toggle*));

#endif