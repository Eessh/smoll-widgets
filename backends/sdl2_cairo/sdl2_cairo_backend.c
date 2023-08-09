#include "sdl2_cairo_backend.h"
#include <stdlib.h>
#include "../../include/macros.h"

static SDL_Window* window = NULL;
static cairo_t* cairo = NULL;

result_void init_sdl2();
result_void init_cairo();

void deinit_sdl2();
void deinit_cairo();

result_void sdl2_cairo_backend_load_font(const char* font, uint8 font_size);
result_text_dimensions sdl2_cairo_backend_get_text_dimensions(
  const char* text, const char* font_name, uint8 font_size);
result_void sdl2_cairo_backend_process_command(const command* cmd);

result_render_backend_ptr sdl2_cairo_backend_create()
{
  render_backend* backend = (render_backend*)calloc(1, sizeof(render_backend));
  if(!backend)
  {
    error(result_render_backend_ptr,
          "Unable to allocate memory for SDL2+Cairo render backend!");
  }

  backend->name = "SDL2 + Cairo";
  backend->backend_version = (version){1, 0, 0};

  backend->load_font = sdl2_cairo_backend_load_font;
  backend->get_text_dimensions = sdl2_cairo_backend_get_text_dimensions;
  backend->process_command = sdl2_cairo_backend_process_command;

  init_sdl2();
  init_cairo();

  return ok(result_render_backend_ptr, backend);
}

SDL_Window* sdl2_cairo_backend_get_window()
{
  return window;
}

cairo_t* sdl2_cairo_backend_get_cairo_instance()
{
  return cairo;
}

result_void sdl2_cairo_backend_destroy(render_backend* backend)
{
  if(!backend)
  {
    error(result_void, "Attempt to free a NULL pointed render backend!");
  }

  deinit_cairo();
  deinit_sdl2();

  free(backend);

  return ok_void();
}

result_void sdl2_cairo_backend_load_font(const char* font, uint8 font_size)
{
  if(!font)
  {
    return error(result_void, "Cannot load font pointing to NULL!");
  }

  cairo_select_font_face(cairo, font, 0, 0);
  cairo_set_font_size(cairo, font_size);

  return ok_void();
}

result_text_dimensions sdl2_cairo_backend_get_text_dimensions(
  const char* text, const char* font_name, uint8 font_size)
{
  if(!text)
  {
    return error(result_text_dimensions,
                 "Cannot get dimensions of text pointing to NULL!");
  }

  if(!font_name)
  {
    return error(result_text_dimensions,
                 "Cannot get dimensions of text, with font pointing to NULL");
  }

  sdl2_cairo_backend_load_font(font_name, font_size);

  cairo_font_extents_t font_extents;
  cairo_font_extents(cairo, &font_extents);
  cairo_text_extents_t text_extents;
  cairo_text_extents(cairo, text, &text_extents);

  text_dimensions dimensions = {.w = text_extents.width,
                                .h = font_extents.height};

  return ok(result_text_dimensions, dimensions);
}

result_void sdl2_cairo_backend_process_command(const command* cmd)
{
  if(!cmd)
  {
    return error(result_void, "Cannot process command pointing to NULL!");
  }

  if(cmd->type == RENDER_RECT)
  {
    cairo_rectangle(cairo,
                    cmd->bounding_rect.x,
                    cmd->bounding_rect.y,
                    cmd->bounding_rect.w,
                    cmd->bounding_rect.h);
    cairo_set_source_rgba(cairo,
                          (float32)(cmd->rect_color.r) / 255.0f,
                          (float32)(cmd->rect_color.g) / 255.0f,
                          (float32)(cmd->rect_color.b) / 255.0f,
                          (float32)(cmd->rect_color.a) / 255.0f);
    cairo_fill(cairo);
  }
  else
  {
    cairo_set_source_rgba(cairo,
                          (float32)(cmd->text_color.r) / 255.0f,
                          (float32)(cmd->text_color.g) / 255.0f,
                          (float32)(cmd->text_color.b) / 255.0f,
                          (float32)(cmd->text_color.a) / 255.0f);
    cairo_font_extents_t font_extents;
    cairo_font_extents(cairo, &font_extents);
    cairo_move_to(cairo,
                  cmd->bounding_rect.x,
                  cmd->bounding_rect.y + font_extents.height -
                    font_extents.descent);
    cairo_show_text(cairo, cmd->text);
  }
}

result_void init_sdl2()
{
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
  {
    return error(result_void, "Error while initializing SDL2!");
  }

  window = SDL_CreateWindow("SDL2 + Cairo Backend",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            1080,
                            720,
                            SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE |
                              SDL_WINDOW_SHOWN);
  if(!window)
  {
    return error(result_void, "Error while creating SDL2 window!");
  }

  return ok_void();
}

result_void init_cairo()
{
  SDL_Surface* window_surface = SDL_GetWindowSurface(window);
  if(!window_surface)
  {
    return error(result_void, "Error while getting SDL2 window surface!");
  }

  cairo_surface_t* cairo_surface =
    cairo_image_surface_create_for_data((unsigned char*)window_surface->pixels,
                                        CAIRO_FORMAT_RGB24,
                                        window_surface->w,
                                        window_surface->h,
                                        window_surface->pitch);
  if(!cairo_surface)
  {
    return error(result_void,
                 "Error while creating cairo surface for SDL2 window!");
  }

  cairo_surface_set_device_scale(cairo_surface, 1.0, 1.0);

  cairo = cairo_create(cairo_surface);
  if(!cairo)
  {
    return error(result_void, "Error while creating cairo instance!");
  }

  cairo_surface_destroy(cairo_surface);

  return ok_void();
}

void deinit_sdl2()
{
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void deinit_cairo()
{
  cairo_destroy(cairo);
}

mouse_motion_event translate_sdl2_mouse_motion_event(SDL_MouseMotionEvent event)
{
  int32 global_x, global_y;
  SDL_GetGlobalMouseState(&global_x, &global_y);

  return (mouse_motion_event){.x = event.x,
                              .y = event.y,
                              .global_x = (uint16)global_x,
                              .global_y = (uint16)global_y};
}

mouse_button_event translate_sdl2_mouse_button_event(SDL_MouseButtonEvent event,
                                                     bool button_down)
{
  mouse_button_type button_type = MOUSE_BUTTON_LEFT;

  if(event.button == SDL_BUTTON_RIGHT)
  {
    button_type = MOUSE_BUTTON_RIGHT;
  }
  else if(event.button == SDL_BUTTON_MIDDLE)
  {
    button_type = MOUSE_BUTTON_MIDDLE;
  }

  mouse_button_state state = button_down ? MOUSE_BUTTON_DOWN : MOUSE_BUTTON_UP;

  // if SDL reported multiple clicks, clicks occurred at same place
  // otherwise it would just report single click events and some other
  // events such as mouse motion or mouse wheel
  if(event.clicks == 2 && !button_down)
  {
    state = MOUSE_DOUBLE_CLICK;
  }
  else if(event.clicks == 3 && !button_down)
  {
    state = MOUSE_TRIPLE_CLICK;
  }

  return (mouse_button_event){
    .x = event.x, .y = event.y, .button_state = state, .button = button_type};
}

mouse_scroll_event translate_sdl2_mouse_wheel_event(SDL_MouseWheelEvent event)
{
  return (mouse_scroll_event){.delta_x = event.preciseX,
                              .delta_y = event.preciseY};
}