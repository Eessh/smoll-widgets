#include "../../include/backend.h"
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
cairo_t* sdl2_cairo_get_cairo_instance();

/// @brief Frees resources used by SDL2+Cairo render backend.
/// @param backend pointer to render backend.
/// @return Void result.
result_void sdl2_cairo_backend_destroy(render_backend* backend);