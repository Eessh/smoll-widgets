#include "../../include/backend.h"
#include "../../include/events.h"
#define SDL_MAIN_HANDLED
#include "SDL2-2.26.5/x86_64-w64-mingw32/include/SDL2/SDL.h"
#include "cairo-windows-1.17.2/include/cairo.h"

/// @brief Creates SDL2+Cairo render backend.
/// @return Render backend pointer result.
result_render_backend_ptr sdl2_cairo_backend_create();

/// @brief Gives pointer to SDL2 window.
/// @return Pointer to SDL2 window.
SDL_Window* sdl2_cairo_backend_get_window();

/// @brief Gives pointer to cairo instance.
/// @return Pointer to cairo_t.
cairo_t* sdl2_cairo_backend_get_cairo_instance();

/// @brief Frees resources used by SDL2+Cairo render backend.
/// @param backend pointer to render backend.
/// @return Void result.
result_void sdl2_cairo_backend_destroy(render_backend* backend);

/// @brief Translates SDL2 window resize event to smoll context event.
///        This internally reloads the cairo object for SDL2 window surface.
/// @param event SDL2 Window event.
/// @return Smoll context's viewport resize event.
viewport_resize_event translate_sdl2_window_resize_event(SDL_WindowEvent event);

/// @brief Translates SDL2 window maximized or restored
///        events to smoll context event.
///        This internally reloads the cairo object for SDL2 window surface.
/// @return Smoll context's viewport resize event.
viewport_resize_event translate_sdl2_window_maximized_or_restored_event();

/// @brief Translates SDL2 mouse motion event to smoll context event.
/// @param event SDL2 Mouse Motion event.
/// @return Smoll context's mouse motion event.
mouse_motion_event
translate_sdl2_mouse_motion_event(SDL_MouseMotionEvent event);

/// @brief Translates SDL2 mouse button event to smoll context event.
/// @param event SDL2 Mouse Button event.
/// @param button_down If this is a button down or button up event.
/// @return Smoll context's mouse button event.
mouse_button_event translate_sdl2_mouse_button_event(SDL_MouseButtonEvent event,
                                                     bool button_down);

/// @brief Translates SDL2 mouse wheel event to smoll context event.
/// @param event SDL2 Mouse Wheel event.
/// @return Smoll context's mouse scroll event.
mouse_scroll_event translate_sdl2_mouse_wheel_event(SDL_MouseWheelEvent event);