#include "win32_cairo_backend.h"
#include <stdio.h>
#include <stdlib.h>
#include "../../include/macros.h"
#include "windowsx.h"

static cairo_t* cairo = NULL;

result_void init_cairo(HDC hdc);
void deinit_cairo();

result_void win32_cairo_backend_load_font(const char* font_name,
                                          uint8 font_size);
result_text_dimensions win32_cairo_backend_get_text_dimensions(
  const char* text, const char* font_name, uint8 font_size);
result_void win32_cairo_backend_process_command(const command* cmd);

result_render_backend_ptr win32_cairo_backend_create(HDC hdc)
{
  render_backend* backend = (render_backend*)calloc(1, sizeof(render_backend));
  if(!backend)
  {
    error(result_render_backend_ptr,
          "Unable to allocate memory for Win32+Cairo render backend!");
  }

  backend->name = "Win32 + Cairo";
  backend->backend_version = (version){1, 0, 0};

  backend->load_font = win32_cairo_backend_load_font;
  backend->get_text_dimensions = win32_cairo_backend_get_text_dimensions;
  backend->process_command = win32_cairo_backend_process_command;

  init_cairo(hdc);

  return ok(result_render_backend_ptr, backend);
}

cairo_t* win32_cairo_get_cairo_instance()
{
  return cairo;
}

result_void win32_cairo_backend_destroy(render_backend* backend)
{
  if(!backend)
  {
    return error(result_void, "Attempt to free a NULL pointed render backend!");
  }

  deinit_cairo();

  free(backend->name);
  free(backend);

  return ok_void();
}

mouse_motion_event translate_win32_mouse_move_event(HWND hwnd,
                                                    UINT msg,
                                                    WPARAM wParam,
                                                    LPARAM lParam)
{
  POINT global_pos;
  GetCursorPos(&global_pos);

  return (mouse_motion_event){.x = (uint16)GET_X_LPARAM(lParam),
                              .y = (uint16)GET_Y_LPARAM(lParam),
                              .global_x = (uint16)global_pos.x,
                              .global_y = (uint16)global_pos.y};
}

mouse_button_event translate_win32_mouse_button_event(HWND hwnd,
                                                      UINT msg,
                                                      WPARAM wParam,
                                                      LPARAM lParam)
{
  switch(msg)
  {
  case WM_LBUTTONUP:
    return (mouse_button_event){.x = GET_X_LPARAM(lParam),
                                .y = GET_Y_LPARAM(lParam),
                                .button_state = MOUSE_BUTTON_UP,
                                .button = MOUSE_BUTTON_LEFT};
  case WM_RBUTTONUP:
    return (mouse_button_event){.x = GET_X_LPARAM(lParam),
                                .y = GET_Y_LPARAM(lParam),
                                .button_state = MOUSE_BUTTON_UP,
                                .button = MOUSE_BUTTON_RIGHT};
  case WM_MBUTTONUP:
    return (mouse_button_event){.x = GET_X_LPARAM(lParam),
                                .y = GET_Y_LPARAM(lParam),
                                .button_state = MOUSE_BUTTON_UP,
                                .button = MOUSE_BUTTON_MIDDLE};
  case WM_LBUTTONDOWN:
    return (mouse_button_event){.x = GET_X_LPARAM(lParam),
                                .y = GET_Y_LPARAM(lParam),
                                .button_state = MOUSE_BUTTON_DOWN,
                                .button = MOUSE_BUTTON_LEFT};
  case WM_LBUTTONDBLCLK:
    return (mouse_button_event){.x = GET_X_LPARAM(lParam),
                                .y = GET_Y_LPARAM(lParam),
                                .button_state = MOUSE_DOUBLE_CLICK,
                                .button = MOUSE_BUTTON_LEFT};
  case WM_RBUTTONDOWN:
    return (mouse_button_event){.x = GET_X_LPARAM(lParam),
                                .y = GET_Y_LPARAM(lParam),
                                .button_state = MOUSE_BUTTON_DOWN,
                                .button = MOUSE_BUTTON_RIGHT};
  case WM_RBUTTONDBLCLK:
    return (mouse_button_event){.x = GET_X_LPARAM(lParam),
                                .y = GET_Y_LPARAM(lParam),
                                .button_state = MOUSE_DOUBLE_CLICK,
                                .button = MOUSE_BUTTON_RIGHT};
  case WM_MBUTTONDOWN:
    return (mouse_button_event){.x = GET_X_LPARAM(lParam),
                                .y = GET_Y_LPARAM(lParam),
                                .button_state = MOUSE_BUTTON_DOWN,
                                .button = MOUSE_BUTTON_MIDDLE};
  case WM_MBUTTONDBLCLK:
    return (mouse_button_event){.x = GET_X_LPARAM(lParam),
                                .y = GET_Y_LPARAM(lParam),
                                .button_state = MOUSE_DOUBLE_CLICK,
                                .button = MOUSE_BUTTON_MIDDLE};
  default:
    break;
  }

#ifdef DEBUG
  printf(
    "Cannot translate event: %ld with translate_win32_mouse_button_event()\n",
    msg);
  exit(1);
#endif
}

mouse_scroll_event translate_win32_mouse_wheel_event(HWND hwnd,
                                                     UINT msg,
                                                     WPARAM wParam,
                                                     LPARAM lParam)
{
  int8 scroll_units = GET_WHEEL_DELTA_WPARAM(wParam);
  float32 delta = (float32)(scroll_units) / WHEEL_DELTA;

  if(msg == WM_MOUSEWHEEL)
  {
    return (mouse_scroll_event){.delta_x = 0.0f, .delta_y = delta};
  }

  return (mouse_scroll_event){.delta_x = delta, .delta_y = 0.0f};
}

result_void win32_cairo_backend_process_command(const command* cmd)
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

result_void init_cairo(HDC hdc)
{
  cairo_surface_t* surface = cairo_win32_surface_create(hdc);
  if(!surface)
  {
    return error(result_void,
                 "Error while creating cairo surface for Win32 window!");
  }

  cairo = cairo_create(surface);
  if(!cairo)
  {
    return error(result_void, "Error while creating cairo instance!");
  }

  cairo_surface_destroy(surface);

  return ok_void();
}

void deinit_cairo()
{
  cairo_destroy(cairo);
}

result_void win32_cairo_backend_load_font(const char* font_name,
                                          uint8 font_size)
{
  if(!font_name)
  {
    return error(result_void, "Cannot load font pointing to NULL!");
  }

  cairo_select_font_face(cairo, font_name, 0, 0);
  cairo_set_font_size(cairo, font_size);

  return ok_void();
}

result_text_dimensions win32_cairo_backend_get_text_dimensions(
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

  win32_cairo_backend_load_font(font_name, font_size);

  cairo_font_extents_t font_extents;
  cairo_font_extents(cairo, &font_extents);
  cairo_text_extents_t text_extents;
  cairo_text_extents(cairo, text, &text_extents);

  text_dimensions dimensions = {.w = text_extents.width,
                                .h = font_extents.height};

  return ok(result_text_dimensions, dimensions);
}
