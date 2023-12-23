#include "win32_cairo_backend.h"
#include <stdio.h>
#include <stdlib.h>
#include <winuser.h>
#include "../../include/macros.h"
#include "windowsx.h"

#define M_PI 3.14159265358979323846264338327950288

static cairo_t* cairo = NULL;

// cursors
HCURSOR arrow = NULL, ibeam = NULL, move = NULL, crosshair = NULL,
        resize_left_right = NULL, resize_top_left__bottom_right = NULL,
        resize_top_right__bottom_left = NULL, resize_top_bottom = NULL,
        hand = NULL, processing = NULL, loading = NULL, prohibited = NULL;

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
  backend->supports_curve_rendering = true;

  backend->load_font = win32_cairo_backend_load_font;
  backend->get_text_dimensions = win32_cairo_backend_get_text_dimensions;
  backend->process_command = win32_cairo_backend_process_command;

  init_cairo(hdc);

  // loading cursors
  arrow = LoadCursor(NULL, IDC_ARROW);
  ibeam = LoadCursor(NULL, IDC_IBEAM);
  move = LoadCursor(NULL, IDC_SIZEALL);
  crosshair = LoadCursor(NULL, IDC_CROSS);
  resize_left_right = LoadCursor(NULL, IDC_SIZEWE);
  resize_top_left__bottom_right = LoadCursor(NULL, IDC_SIZENWSE);
  resize_top_right__bottom_left = LoadCursor(NULL, IDC_SIZENESW);
  resize_top_bottom = LoadCursor(NULL, IDC_SIZENS);
  hand = LoadCursor(NULL, IDC_HAND);
  processing = LoadCursor(NULL, IDC_APPSTARTING);
  loading = LoadCursor(NULL, IDC_WAIT);
  prohibited = LoadCursor(NULL, IDC_NO);

  // setting cursor
  SetCursor(arrow);

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

  // freeing cursors
  DestroyCursor(arrow);
  DestroyCursor(ibeam);
  DestroyCursor(move);
  DestroyCursor(crosshair);
  DestroyCursor(resize_left_right);
  DestroyCursor(resize_top_left__bottom_right);
  DestroyCursor(resize_top_right__bottom_left);
  DestroyCursor(resize_top_bottom);
  DestroyCursor(hand);
  DestroyCursor(processing);
  DestroyCursor(loading);
  DestroyCursor(prohibited);

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
  }
  else if (cmd->type == RENDER_ROUNDED_RECT) {
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
  }
  else if(cmd->type == RENDER_RECT_OUTLINED)
  {
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
  }
  else if(cmd->type == RENDER_TEXT)
  {
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
  }
  else if(cmd->type == PUSH_CLIP_RECT)
  {
    printf("BACKEND: Command: push clip rect\n");
    rect clip_rect = cmd->data.clip_rect;
    cairo_rectangle(cairo, clip_rect.x, clip_rect.y, clip_rect.w, clip_rect.h);
    cairo_clip(cairo);
  }
  else if(cmd->type == POP_CLIP_RECT)
  {
    printf("BACKEND: Command: pop clip rect\n");
    cairo_reset_clip(cairo);
  }
  else if(cmd->type == SET_CURSOR_ARROW)
  {
    SetCursor(arrow);
  }
  else if(cmd->type == SET_CURSOR_IBEAM)
  {
    SetCursor(ibeam);
  }
  else if(cmd->type == SET_CURSOR_MOVE)
  {
    SetCursor(move);
  }
  else if(cmd->type == SET_CURSOR_CROSSHAIR)
  {
    SetCursor(crosshair);
  }
  else if(cmd->type == SET_CURSOR_HAND)
  {
    SetCursor(hand);
  }
  else if(cmd->type == SET_CURSOR_LOADING)
  {
    SetCursor(loading);
  }
  else if(cmd->type == SET_CURSOR_PROCESSING)
  {
    SetCursor(processing);
  }
  else if(cmd->type == SET_CURSOR_PROHIBITED)
  {
    SetCursor(prohibited);
  }
  else if(cmd->type == SET_CURSOR_RESIZE_LEFT_RIGHT)
  {
    SetCursor(resize_left_right);
  }
  else if(cmd->type == SET_CURSOR_RESIZE_TOP_BOTTOM)
  {
    SetCursor(resize_top_bottom);
  }
  else if(cmd->type == SET_CURSOR_RESIZE_TOP_LEFT__BOTTOM_RIGHT)
  {
    SetCursor(resize_top_left__bottom_right);
  }
  else if(cmd->type == SET_CURSOR_RESIZE_TOP_RIGHT__BOTTOM_LEFT)
  {
    SetCursor(resize_top_right__bottom_left);
  }

  return ok_void();
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

  printf("BACKEND: font face: %s, font size: %d\n", font_name, font_size);

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

  // result_void _ = win32_cairo_backend_load_font(font_name, font_size);
  // if(!_.ok)
  // {
  //   return error(result_text_dimensions, _.error);
  // }

  cairo_font_extents_t font_extents;
  cairo_font_extents(cairo, &font_extents);
  cairo_text_extents_t text_extents;
  cairo_text_extents(cairo, text, &text_extents);

  text_dimensions dimensions = {.w = text_extents.width,
                                .h = font_extents.height};

  return ok(result_text_dimensions, dimensions);
}
