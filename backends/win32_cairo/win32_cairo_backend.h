#ifndef SMOLL_WIDGETS_WIN32_CAIRO_BACKEND_H
#define SMOLL_WIDGETS_WIN32_CAIRO_BACKEND_H

#include "../../include/backend.h"
#include "../../include/events.h"
#include "cairo-windows-1.17.2/include/cairo-win32.h"
#include "cairo-windows-1.17.2/include/cairo.h"

result_render_backend_ptr win32_cairo_backend_create(HDC hdc);

cairo_t* win32_cairo_get_cairo_instance();

result_void win32_cairo_backend_destroy(render_backend* backend);

mouse_motion_event translate_win32_mouse_move_event(HWND hwnd,
                                                    UINT msg,
                                                    WPARAM wParam,
                                                    LPARAM lParam);

mouse_button_event translate_win32_mouse_button_event(HWND hwnd,
                                                      UINT msg,
                                                      WPARAM wParam,
                                                      LPARAM lParam);

mouse_scroll_event translate_win32_mouse_wheel_event(HWND hwnd,
                                                     UINT msg,
                                                     WPARAM wParam,
                                                     LPARAM lParam);

#endif //SMOLL_WIDGETS_WIN32_CAIRO_BACKEND_H
