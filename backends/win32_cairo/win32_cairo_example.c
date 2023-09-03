#include <stdio.h>
#include "../../include/smoll_context.h"
#include "../../include/widgets/box.h"
#include "../../include/widgets/button.h"
#include "../../include/widgets/checkbox.h"
#include "../../include/widgets/flex_view.h"
#include "../../include/widgets/progress_bar.h"
#include "../../include/widgets/toggle.h"
#include "win32_cairo_backend.h"

// Static because these are needed by
// WinMain to intialize these
// WndProc to process events
static smoll_context* sctx = NULL;
static render_backend* backend = NULL;

static progress_bar* bar = NULL;

// Callbacks function prototypes
void mouse_button_down_callback(button* btn, mouse_button_event event);
void mouse_button_up_callback(button* btn, mouse_button_event event);
void mouse_enter_callback(button* btn, mouse_motion_event event);
void mouse_leave_callback(button* btn, mouse_motion_event event);
void on_callback(toggle* t);
void off_callback(toggle* t);

// Event handler
LRESULT CALLBACK WndProc(HWND window,
                         UINT message,
                         WPARAM wParam,
                         LPARAM lParam);

// Entry point
INT WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   PSTR lpCmdLine,
                   INT iCmdShow)
{
  // Creating window class
  WNDCLASS window_class;
  window_class.style = CS_HREDRAW | CS_VREDRAW;
  window_class.lpfnWndProc = WndProc;
  window_class.cbClsExtra = 0;
  window_class.cbWndExtra = 0;
  window_class.hInstance = hInstance;
  window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  // cursor is handled by backend
  // default cursor is IDC_ARROW
  window_class.hCursor = NULL;
  window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  window_class.lpszMenuName = NULL;
  window_class.lpszClassName = TEXT("Smoll Widgets - Win32 + Cairo Backend");

  // Registering window class
  RegisterClass(&window_class);

  // Adjusting window size and position
  RECT rect = {0, 0, 1080, 720};
  AdjustWindowRect(
    &rect, WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME), FALSE);

  // Creating window and updating it
  HWND window =
    CreateWindow(window_class.lpszClassName,
                 window_class.lpszClassName,
                 WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME),
                 CW_USEDEFAULT,
                 CW_USEDEFAULT,
                 rect.right,
                 rect.bottom,
                 NULL,
                 NULL,
                 hInstance,
                 NULL);
  ShowWindow(window, iCmdShow);
  UpdateWindow(window);

  // Creating smoll context
  {
    result_smoll_context_ptr _ = smoll_context_create();
    if(!_.ok)
    {
      printf("Error: %s", _.error);
      exit(1);
    }
    sctx = _.value;
  }

  // Getting device context and creating Win32 + Cairo backend
  HDC hdc;
  {
    hdc = GetDC(window);
    result_render_backend_ptr _ = win32_cairo_backend_create(hdc);
    if(!_.ok)
    {
      printf("Error: %s", _.error);
      ReleaseDC(window, hdc);
      smoll_context_destroy(sctx);
      exit(1);
    }
    backend = _.value;
  }

  // Registering backend
  {
    result_void _ = smoll_context_register_backend(sctx, backend);
    if(!_.ok)
    {
      printf("Error: %s", _.error);
      smoll_context_destroy(sctx);
      win32_cairo_backend_destroy(backend);
      ReleaseDC(window, hdc);
      exit(1);
    }
  }

  // Setting default font (or) fallback font for smoll context
  smoll_context_set_default_font(sctx, "Consolas", 14);

  // Creating box widget
  box* bx = NULL;
  {
    result_box_ptr _ = box_new(NULL, FLEX_DIRECTION_ROW);
    if(!_.ok)
    {
      printf("Error while creating box: %s", _.error);
    }
    bx = _.value;
    bx->base->w = 1080;
    bx->base->h = 720;
    bx->background = (color){255, 255, 255, 255};
    bx->base->flexbox_data.container.is_fluid = false;
  }

  // Setting button as root widget of smoll context
  smoll_context_set_root_widget(sctx, bx->base);

  // Creating flex-row view
  flex_view* row_view = NULL;
  {
    result_flex_view_ptr _ = flex_view_new(bx->base, FLEX_DIRECTION_ROW);
    if(!_.ok)
    {
      printf("Error while creating flex-row view: %s", _.error);
    }
    row_view = _.value;
    row_view->base->flexbox_data.container.align_items = FLEX_ALIGN_CENTER;
    row_view->base->flexbox_data.container.cross_axis_sizing =
      CROSS_AXIS_SIZING_EXPAND;
    row_view->base->flexbox_data.container.gap = 10;
    row_view->background = (color){128, 128, 128, 255};
  }

  // Creating button widget
  button* btn = NULL;
  {
    result_button_ptr _ = button_new(row_view->base, "Hola!");
    if(!_.ok)
    {
      printf("Error while creating button: %s", _.error);
    }
    btn = _.value;
    btn->padding_x = 6;
    btn->padding_y = 4;
    btn->foreground = (color){255, 255, 255, 255};
    btn->background = (color){16, 16, 16, 255};
    btn->hover_foreground = (color){0, 255, 0, 255};
    btn->hover_background = (color){64, 64, 64, 255};
    btn->click_foreground = (color){255, 0, 0, 255};
    btn->click_background = (color){128, 128, 128, 255};

    // Attaching event callbacks
    button_set_mouse_down_callback(btn, mouse_button_down_callback);
    button_set_mouse_up_callback(btn, mouse_button_up_callback);
    button_set_mouse_enter_callback(btn, mouse_enter_callback);
    button_set_mouse_leave_callback(btn, mouse_leave_callback);
  }

  // Creating another button widget
  button* btn1 = NULL;
  {
    result_button_ptr _ = button_new(row_view->base, "Hello there!");
    if(!_.ok)
    {
      printf("Error while creating button: %s", _.error);
    }
    btn1 = _.value;
    btn1->padding_x = 6;
    btn1->padding_y = 4;
    btn1->foreground = (color){255, 255, 255, 255};
    btn1->background = (color){16, 16, 16, 255};
    btn1->hover_foreground = (color){0, 255, 0, 255};
    btn1->hover_background = (color){64, 64, 64, 255};
    btn1->click_foreground = (color){255, 0, 0, 255};
    btn1->click_background = (color){128, 128, 128, 255};

    // Attaching event callbacks
    button_set_mouse_down_callback(btn1, mouse_button_down_callback);
    button_set_mouse_up_callback(btn1, mouse_button_up_callback);
    button_set_mouse_enter_callback(btn1, mouse_enter_callback);
    button_set_mouse_leave_callback(btn1, mouse_leave_callback);
  }

  // Creating toggle widget
  toggle* t = NULL;
  {
    result_toggle_ptr _ = toggle_new(row_view->base);
    if(!_.ok)
    {
      printf("Error while creating toggle: %s", _.error);
    }
    t = _.value;
    t->base->w = 50;
    t->base->h = 20;
    t->handle_width_fraction = 0.4;
    t->padding_x = 2;
    t->padding_y = 2;

    // Attaching event callbacks
    toggle_set_on_callback(t, on_callback);
    toggle_set_off_callback(t, off_callback);
  }

  // Creating progress bar
  {
    result_progress_bar_ptr _ = progress_bar_new(
      row_view->base, 20, (color){0, 255, 0, 255}, (color){64, 64, 64, 255});
    if(!_.ok)
    {
      printf("Error while creating progress bar: %s\n", _.error);
    }
    bar = _.value;
    bar->base->w = 200;
    bar->base->h = 20;
  }

  // Creating checkbox
  checkbox* cbox = NULL;
  {
    result_checkbox_ptr _ =
      checkbox_new(row_view->base, (color){255, 255, 255, 255});
    if(!_.ok)
    {
      printf("Error while creating checkbox: %s\n", _.error);
    }
    cbox = _.value;
  }

  // Calling initial layouting, rendering functions
  // smoll_context_initial_fit_layout(sctx);
  smoll_context_initialize_layout(sctx);
  smoll_context_initial_render(sctx);

  // Event loop
  MSG message;
  while(GetMessage(&message, NULL, 0, 0))
  {
    TranslateMessage(&message);
    DispatchMessage(&message);
  }

  // Destroying smoll context
  // this also frees UI tree
  smoll_context_destroy(sctx);

  // Destroying Win32 + Cairo backend
  win32_cairo_backend_destroy(backend);

  // Releasing device context
  ReleaseDC(window, hdc);

  return message.wParam;
}

// Callbacks implementations
void mouse_button_down_callback(button* btn, mouse_button_event event)
{
  printf("Mouse button down!\n");
  button_set_text(btn, "Clicked!");
}

void mouse_button_up_callback(button* btn, mouse_button_event event)
{
  printf("Mouse button up!\n");
  button_set_text(btn, "Hola!");
}

void mouse_enter_callback(button* btn, mouse_motion_event event)
{
  printf("Mouse entered into button!\n");
}

void mouse_leave_callback(button* btn, mouse_motion_event event)
{
  printf("Mouse left button!\n");
}

void on_callback(toggle* t)
{
  printf("Setting progress bar percent to: 80\n");
  progress_bar_set_percent(bar, 80);
}

void off_callback(toggle* t)
{
  printf("Setting progress bar percent to: 60\n");
  progress_bar_set_percent(bar, 60);
}

// Event handler implementation
LRESULT CALLBACK WndProc(HWND window,
                         UINT message,
                         WPARAM wParam,
                         LPARAM lParam)
{
  switch(message)
  {
  case WM_DESTROY: {
    PostQuitMessage(0);
    return 0;
  }
  case WM_MOUSEMOVE: {
    smoll_context_process_mouse_motion_event(
      sctx, translate_win32_mouse_move_event(window, message, wParam, lParam));
    smoll_context_render(sctx);
    UpdateWindow(window);
    return 0;
  }
  case WM_LBUTTONUP:
  case WM_RBUTTONUP:
  case WM_MBUTTONUP:
  case WM_LBUTTONDOWN:
  case WM_LBUTTONDBLCLK:
  case WM_RBUTTONDOWN:
  case WM_RBUTTONDBLCLK:
  case WM_MBUTTONDOWN:
  case WM_MBUTTONDBLCLK: {
    smoll_context_process_mouse_button_event(
      sctx,
      translate_win32_mouse_button_event(window, message, wParam, lParam));
    smoll_context_render(sctx);
    UpdateWindow(window);
    return 0;
  }
  case WM_MOUSEWHEEL:
  case WM_MOUSEHWHEEL: {
    smoll_context_process_mouse_scroll_event(
      sctx, translate_win32_mouse_wheel_event(window, message, wParam, lParam));
    smoll_context_render(sctx);
    UpdateWindow(window);
    return 0;
  }
  default:
    break;
  }

  return DefWindowProc(window, message, wParam, lParam);
}
