#ifndef SMOLL_WIDGETS__BUTTON_H
#define SMOLL_WIDGETS__BUTTON_H

#include "../base_widget.h"

/// @brief Button widget private data.
typedef struct button_private button_private;

/// @brief Button Widget.
///        Use `button_new()` to create this widget.
///        Children should not be attached to this widget.
///        This should not be root widget.
typedef struct button
{
  /// @brief X-axis padding. This will be the value for both padding-left
  ///        and padding-right.
  uint16 padding_x;

  /// @brief Y-axis padding. This will be the value for both padding-top
  ///        and padding-bottom.
  uint16 padding_y;

  /// @brief Foreground color of button, applies to text of button.
  color foreground;

  /// @brief Background color of button.
  color background;

  /// @brief Hover foreground color of button, applies to text of button.
  ///        Button automatically uses in hover foreground color when hovered.
  color hover_foreground;

  /// @brief Hover background color of button.
  ///        Button automatically uses in hover background color when hovered.
  color hover_background;

  /// @brief Click foreground color of button, applies to text of button.
  ///        Button automatically uses in click foreground color when hovered.
  color click_foreground;

  /// @brief Click background color of button.
  ///        Button automatically uses in click background color when hovered.
  color click_background;

  /// @brief Pointer to base widget of button.
  base_widget* base;

  /// @brief Pointer to private data of button.
  button_private* private_data;
} button;

/// @brief Button pointer result.
typedef struct result_button_ptr
{
  bool ok;
  union
  {
    button* value;
    const char* error;
  };
} result_button_ptr;

/// @brief Creates new button widget.
///        Once attached to UI tree, on destroying the smoll_context all
///        widgets get automatically freed.
/// @param parent_base pointer to base widget of parent.
/// @param text constant pointer to text.
/// @return Button pointer result.
result_button_ptr button_new(base_widget* parent_base, const char* text);

/// @brief Gets text of button.
/// @param btn constant pointer to button.
/// @return Const char pointer result.
result_const_char_ptr button_get_text(const button* btn);

/// @brief Sets text of button.
/// @param btn pointer to button.
/// @param text constant pointer to text.
/// @return Void result.
result_void button_set_text(button* btn, const char* text);

/// @brief Sets callback for mouse down event.
///        Setting button colors to click colors are automatically handled.
///        Use this function to do some external calculations.
/// @param btn pointer to button.
/// @param callback the function to call when mouse down event occurs.
result_void
button_set_mouse_down_callback(button* btn,
                               void (*callback)(button*, mouse_button_event));

/// @brief Sets callback for mouse up event.
///        Setting button colors to click colors are automatically handled.
///        Use this function to do some external calculations.
/// @param btn pointer to button.
/// @param callback the function to call when mouse up event occurs.
result_void button_set_mouse_up_callback(button* btn,
                                         void (*callback)(button*,
                                                          mouse_button_event));

/// @brief Sets callback for mouse enter event.
///        Setting button colors to hover colors are automatically handled.
///        Use this function to do some external calculations.
/// @param btn pointer to button.
/// @param callback the function to call when mouse enter event occurs.
result_void
button_set_mouse_enter_callback(button* btn,
                                void (*callback)(button*, mouse_motion_event));

/// @brief Sets callback for mouse leave event.
///        Setting button colors to hover colors are automatically handled.
///        Use this function to do some external calculations.
/// @param btn pointer to button.
/// @param callback the function to call when mouse leave event occurs.
result_void
button_set_mouse_leave_callback(button* btn,
                                void (*callback)(button*, mouse_motion_event));

#endif