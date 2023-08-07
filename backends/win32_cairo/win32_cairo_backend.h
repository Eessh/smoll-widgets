#ifndef SMOLL_WIDGETS_WIN32_CAIRO_BACKEND_H
#define SMOLL_WIDGETS_WIN32_CAIRO_BACKEND_H

#include "../../include/backend.h"
#include "../../include/events.h"
#include "cairo-windows-1.17.2/include/cairo-win32.h"
#include "cairo-windows-1.17.2/include/cairo.h"

/// @brief Create Win32+Cairo render backend.
/// @param hdc handle to device context.
/// @return Render backend point result.
result_render_backend_ptr win32_cairo_backend_create(HDC hdc);

/// @brief Gives pointer to cairo instance.
/// @return Pointer to cairo_t.
cairo_t* win32_cairo_get_cairo_instance();

/// @brief Frees resources used by Win32+Cairo render backend.
/// @param backend pointer to render backend.
/// @return Void result.
result_void win32_cairo_backend_destroy(render_backend* backend);

/// @brief Translates Win32 mouse move event to smoll context's
///        mouse motion event.
/// @param hwnd handle to window.
/// @param msg event message.
/// @param wParam word param.
/// @param lParam long param.
/// @return Smoll context's mouse motion event.
mouse_motion_event translate_win32_mouse_move_event(HWND hwnd,
                                                    UINT msg,
                                                    WPARAM wParam,
                                                    LPARAM lParam);

/// @brief Translates Win32 mouse button event to smoll context's
///        mouse button event.
/// @param hwnd handle to window.
/// @param msg event message.
/// @param wParam word param.
/// @param lParam long param.
/// @return Smoll context's mouse button event.
mouse_button_event translate_win32_mouse_button_event(HWND hwnd,
                                                      UINT msg,
                                                      WPARAM wParam,
                                                      LPARAM lParam);

/// @brief Translates Win32 mouse wheel event to smoll context's
///        mouse scroll event.
/// @param hwnd handle to window.
/// @param msg event message.
/// @param wParam word param.
/// @param lParam long param.
/// @return Smoll context's mouse scroll event.
mouse_scroll_event translate_win32_mouse_wheel_event(HWND hwnd,
                                                     UINT msg,
                                                     WPARAM wParam,
                                                     LPARAM lParam);

#endif //SMOLL_WIDGETS_WIN32_CAIRO_BACKEND_H
