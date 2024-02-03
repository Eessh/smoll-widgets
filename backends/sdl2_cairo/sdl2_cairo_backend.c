#include "sdl2_cairo_backend.h"
#include <stdlib.h>
#include "../../include/macros.h"

static SDL_Window* window = NULL;
static cairo_t* cairo = NULL;

// cursors
typedef SDL_Cursor* SDL_CursorPtr;
SDL_CursorPtr arrow = NULL, ibeam = NULL, move = NULL, crosshair = NULL,
              resize_left_right = NULL, resize_top_left__bottom_right = NULL,
              resize_top_right__bottom_left = NULL, resize_top_bottom = NULL,
              hand = NULL, processing = NULL, loading = NULL, prohibited = NULL;

result_void init_sdl2();
result_void init_cairo();

void deinit_sdl2();
void deinit_cairo();

result_void sdl2_cairo_backend_load_font(const char* font, uint8 font_size);
result_text_dimensions sdl2_cairo_backend_get_text_dimensions(
  const char* text, const char* font_name, uint8 font_size);
result_void sdl2_cairo_backend_process_command(const command* cmd);
result_void
sdl2_cairo_backend_process_command_buffer(const command_buffer* cmd_buffer);

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
  backend->supports_curve_rendering = true;

  backend->load_font = sdl2_cairo_backend_load_font;
  backend->get_text_dimensions = sdl2_cairo_backend_get_text_dimensions;
  backend->process_command = sdl2_cairo_backend_process_command;
  backend->process_command_buffer = sdl2_cairo_backend_process_command_buffer;

  init_sdl2();
  init_cairo();

  // loading cursors
  arrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
  ibeam = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
  move = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
  crosshair = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
  resize_left_right = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
  resize_top_bottom = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
  resize_top_left__bottom_right =
    SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
  resize_top_right__bottom_left =
    SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
  hand = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
  processing = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAITARROW);
  loading = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
  prohibited = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);

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

  // freeing cursors
  SDL_FreeCursor(arrow);
  SDL_FreeCursor(ibeam);
  SDL_FreeCursor(move);
  SDL_FreeCursor(crosshair);
  SDL_FreeCursor(resize_left_right);
  SDL_FreeCursor(resize_top_bottom);
  SDL_FreeCursor(resize_top_left__bottom_right);
  SDL_FreeCursor(resize_top_right__bottom_left);
  SDL_FreeCursor(hand);
  SDL_FreeCursor(processing);
  SDL_FreeCursor(loading);
  SDL_FreeCursor(prohibited);

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

  // sdl2_cairo_backend_load_font(font_name, font_size);

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

  switch(cmd->type)
  {
  case RENDER_RECT: {
    const rect bounding_rect = cmd->data.render_rect.bounding_rect;
    const color rect_color = cmd->data.render_rect.rect_color;
    cairo_rectangle(cairo,
                    bounding_rect.x,
                    bounding_rect.y,
                    bounding_rect.w,
                    bounding_rect.h);
    cairo_set_source_rgba(cairo,
                          (float32)(rect_color.r) / 255.0f,
                          (float32)(rect_color.g) / 255.0f,
                          (float32)(rect_color.b) / 255.0f,
                          (float32)(rect_color.a) / 255.0f);
    cairo_fill(cairo);
    break;
  }
  case RENDER_ROUNDED_RECT: {
    const rect bounding_rect = cmd->data.render_rounded_rect.bounding_rect;
    const uint8 border_radius = cmd->data.render_rounded_rect.border_radius;
    const color rect_color = cmd->data.render_rounded_rect.rect_color;
    cairo_set_source_rgba(cairo,
                          (float32)(rect_color.r) / 255.0f,
                          (float32)(rect_color.g) / 255.0f,
                          (float32)(rect_color.b) / 255.0f,
                          (float32)(rect_color.a) / 255.0f);
    cairo_new_sub_path(cairo);
    cairo_arc(cairo,
              bounding_rect.x + border_radius,
              bounding_rect.y + border_radius,
              border_radius,
              M_PI,
              3 * M_PI / 2);
    cairo_arc(cairo,
              bounding_rect.x + bounding_rect.w - border_radius,
              bounding_rect.y + border_radius,
              border_radius,
              3 * M_PI / 2,
              2 * M_PI);
    cairo_arc(cairo,
              bounding_rect.x + bounding_rect.w - border_radius,
              bounding_rect.y + bounding_rect.h - border_radius,
              border_radius,
              0,
              M_PI / 2);
    cairo_arc(cairo,
              bounding_rect.x + border_radius,
              bounding_rect.y + bounding_rect.h - border_radius,
              border_radius,
              M_PI / 2,
              M_PI);
    cairo_close_path(cairo);
    cairo_fill(cairo);
    break;
  }
  case RENDER_RECT_OUTLINED: {
    const rect bounding_rect = cmd->data.render_rect.bounding_rect;
    const color rect_color = cmd->data.render_rect.rect_color;
    cairo_rectangle(cairo,
                    bounding_rect.x,
                    bounding_rect.y,
                    bounding_rect.w,
                    bounding_rect.h);
    cairo_set_source_rgba(cairo,
                          (float32)(rect_color.r) / 255.0f,
                          (float32)(rect_color.g) / 255.0f,
                          (float32)(rect_color.b) / 255.0f,
                          (float32)(rect_color.a) / 255.0f);
    cairo_stroke(cairo);
    break;
  }
  case RENDER_TEXT: {
    const char* text = cmd->data.render_text.text;
    const color text_color = cmd->data.render_text.text_color;
    const point text_coordinates = cmd->data.render_text.text_coordinates;
    cairo_set_source_rgba(cairo,
                          (float32)(text_color.r) / 255.0f,
                          (float32)(text_color.g) / 255.0f,
                          (float32)(text_color.b) / 255.0f,
                          (float32)(text_color.a) / 255.0f);
    cairo_font_extents_t font_extents;
    cairo_font_extents(cairo, &font_extents);
    cairo_move_to(cairo,
                  text_coordinates.x,
                  text_coordinates.y + font_extents.height -
                    font_extents.descent);
    cairo_show_text(cairo, text);
    break;
  }
  case PUSH_CLIP_RECT: {
    rect clip_rect = cmd->data.clip_rect;
    cairo_rectangle(cairo, clip_rect.x, clip_rect.y, clip_rect.w, clip_rect.h);
    cairo_clip(cairo);
    break;
  }
  case POP_CLIP_RECT: {
    cairo_reset_clip(cairo);
    break;
  }
  case SET_CURSOR_ARROW: {
    SDL_SetCursor(arrow);
    break;
  }
  case SET_CURSOR_IBEAM: {
    SDL_SetCursor(ibeam);
    break;
  }
  case SET_CURSOR_MOVE: {
    SDL_SetCursor(move);
    break;
  }
  case SET_CURSOR_CROSSHAIR: {
    SDL_SetCursor(crosshair);
    break;
  }
  case SET_CURSOR_HAND: {
    SDL_SetCursor(hand);
    break;
  }
  case SET_CURSOR_LOADING: {
    SDL_SetCursor(loading);
    break;
  }
  case SET_CURSOR_PROCESSING: {
    SDL_SetCursor(processing);
    break;
  }
  case SET_CURSOR_PROHIBITED: {
    SDL_SetCursor(prohibited);
    break;
  }
  case SET_CURSOR_RESIZE_LEFT_RIGHT: {
    SDL_SetCursor(resize_left_right);
    break;
  }
  case SET_CURSOR_RESIZE_TOP_BOTTOM: {
    SDL_SetCursor(resize_top_bottom);
    break;
  }
  case SET_CURSOR_RESIZE_TOP_LEFT__BOTTOM_RIGHT: {
    SDL_SetCursor(resize_top_left__bottom_right);
    break;
  }
  case SET_CURSOR_RESIZE_TOP_RIGHT__BOTTOM_LEFT: {
    SDL_SetCursor(resize_top_right__bottom_left);
    break;
  }
  case CLEAR_WINDOW: {
    cairo_set_source_rgb(cairo, 0.0f, 0.0f, 0.0f);
    cairo_fill(cairo);
    break;
  }
  default:
    break;
  }

  return ok_void();
}

result_void
sdl2_cairo_backend_process_command_buffer(const command_buffer* cmd_buffer)
{
  if(!cmd_buffer)
  {
    return error(result_void,
                 "Cannot process command buffer pointing to NULL!");
  }

  int16 buffer_length = command_buffer_length(cmd_buffer);
  if(buffer_length < 1)
  {
    return ok_void();
  }

  SDL_Rect rects_to_update[buffer_length];
  int last_rect_index = 0;

  result_command_buffer_const_iterator_ptr _ =
    command_buffer_const_iterator_new(cmd_buffer);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  command_buffer_const_iterator* iterator = _.value;

  for(int16 i = 0; i < buffer_length; i++)
  {
    if(!iterator->good)
    {
      break;
    }

    result_const_command_ptr __ = iterator->next_cmd(iterator);
    if(!__.ok)
    {
      return error(result_void, __.error);
    }

    const command* cmd = __.value;
    switch(cmd->type)
    {
    case RENDER_RECT: {
      rects_to_update[i] =
        rect_to_sdl_rect(cmd->data.render_rect.bounding_rect);
      last_rect_index = i;
      break;
    }
    case RENDER_ROUNDED_RECT: {
      rects_to_update[i] =
        rect_to_sdl_rect(cmd->data.render_rounded_rect.bounding_rect);
      last_rect_index = i;
      break;
    }
    case RENDER_RECT_OUTLINED: {
      rects_to_update[i] =
        rect_to_sdl_rect(cmd->data.render_rect.bounding_rect);
      last_rect_index = i;
      break;
    }
    case CLEAR_WINDOW: {
      int window_width = 0, window_height = 0;
      SDL_GetWindowSize(window, &window_width, &window_height);
      rects_to_update[i] =
        (SDL_Rect){.x = 0, .y = 0, .w = window_width, .h = window_height};
      last_rect_index = i;
      break;
    }
    default: {
      break;
    };
    }

    result_void ___ = sdl2_cairo_backend_process_command(cmd);
    if(!___.ok)
    {
      return ___;
    }
  }

  if(SDL_UpdateWindowSurfaceRects(
       window, rects_to_update, last_rect_index + 1) != 0)
  {
    return error(result_void, "Unable to update window surface rects!");
  }

  return ok_void();
}

result_void init_sdl2()
{
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
  {
    return error(result_void, "Error while initializing SDL2!");
  }

  /// bypassing compositors in X11
  SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
  SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
  SDL_SetHint("SDL_MOUSE_DOUBLE_CLICK_RADIUS", "4");

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

viewport_resize_event translate_sdl2_window_resize_event(SDL_WindowEvent event)
{
  // destroying previous cairo object as SDL_Window's surface is recreated
  cairo_destroy(cairo);
  init_cairo();

  return (viewport_resize_event){.w = (uint16)event.data1,
                                 .h = (uint16)event.data2};
}

viewport_resize_event translate_sdl2_window_maximized_or_restored_event()
{
  // destroying previous cairo object as SDL_Window's surface is recreated
  cairo_destroy(cairo);
  init_cairo();

  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  //  SDL_GetWindowSizeInPixels(window, &w, &h);
  return (viewport_resize_event){.w = (uint16)w, .h = (uint16)h};
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
