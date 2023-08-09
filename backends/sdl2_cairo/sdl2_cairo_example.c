#include <stdio.h>
#include "../../include/smoll_context.h"
#include "../../include/widgets/box.h"
#include "../../include/widgets/button.h"
#include "sdl2_cairo_backend.h"

// Callbacks function prototypes
void mouse_button_down_callback(button* btn, mouse_button_event event);
void mouse_button_up_callback(button* btn, mouse_button_event event);
void mouse_enter_callback(button* btn, mouse_motion_event event);
void mouse_leave_callback(button* btn, mouse_motion_event event);

int main()
{
  smoll_context* sctx = NULL;
  render_backend* backend = NULL;

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

  // Creating SDL2 + Cairo backend
  {
    result_render_backend_ptr _ = sdl2_cairo_backend_create();
    if(!_.ok)
    {
      printf("Error: %s", _.error);
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
      sdl2_cairo_backend_destroy(backend);
      exit(1);
    }
  }

  // Setting default font (or) fallback font for smoll context
  smoll_context_set_default_font(sctx, "Consolas", 18);

  // Creating box widget
  box* bx = NULL;
  {
    result_box_ptr _ = box_new(NULL);
    if(!_.ok)
    {
      printf("Error while creating box: %s", _.error);
    }
    bx = _.value;
    bx->base->x = 0;
    bx->base->y = 0;
    bx->base->w = 1080;
    bx->base->h = 720;
    bx->background = (color){255, 255, 255, 255};
  }

  // Setting button as root widget of smoll context
  smoll_context_set_root_widget(sctx, bx->base);

  // Creating button widget
  button* btn = NULL;
  {
    result_button_ptr _ = button_new(bx->base, "Hola!");
    if(!_.ok)
    {
      printf("Error while creating button: %s", _.error);
    }
    btn = _.value;
    btn->base->x = 100;
    btn->base->y = 100;
    btn->base->w = 400;
    btn->base->h = 400;
    btn->padding_x = 20;
    btn->padding_y = 10;
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
    result_button_ptr _ = button_new(bx->base, "Hello there!");
    if(!_.ok)
    {
      printf("Error while creating button: %s", _.error);
    }
    btn1 = _.value;
    btn1->base->x = 300;
    btn1->base->y = 300;
    btn1->base->w = 400;
    btn1->base->h = 400;
    btn1->padding_x = 20;
    btn1->padding_y = 10;
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

  // Calling initial layouting, rendering functions
  smoll_context_initial_fit_layout(sctx);
  smoll_context_initial_render(sctx);

  // Event Loop
  while(1)
  {
    SDL_Event event;
    if(SDL_WaitEvent(&event))
    {
      if(event.type == SDL_QUIT)
      {
        goto cleanup;
      }

      // Translating & processing events
      else if(event.type == SDL_MOUSEMOTION)
      {
        smoll_context_process_mouse_motion_event(
          sctx, translate_sdl2_mouse_motion_event(event.motion));
      }
      else if(event.type == SDL_MOUSEBUTTONDOWN)
      {
        smoll_context_process_mouse_button_event(
          sctx, translate_sdl2_mouse_button_event(event.button, true));
      }
      else if(event.type == SDL_MOUSEBUTTONUP)
      {
        smoll_context_process_mouse_button_event(
          sctx, translate_sdl2_mouse_button_event(event.button, false));
      }
      else if(event.type == SDL_MOUSEWHEEL)
      {
        smoll_context_process_mouse_scroll_event(
          sctx, translate_sdl2_mouse_wheel_event(event.wheel));
      }
    }

    // Rendering incremental updates
    smoll_context_render(sctx);

    // Updating window surface
    SDL_UpdateWindowSurface(sdl2_cairo_backend_get_window());
  }

cleanup:
  // Destroying smoll context
  // this also frees UI tree
  smoll_context_destroy(sctx);

  // Destroying SDL2 + Cairo backend
  sdl2_cairo_backend_destroy(backend);

  return 0;
}

// Callbacks implementations
void mouse_button_down_callback(button* btn, mouse_button_event event)
{
  printf("Mouse button down!\n");
}

void mouse_button_up_callback(button* btn, mouse_button_event event)
{
  printf("Mouse button up!\n");
}

void mouse_enter_callback(button* btn, mouse_motion_event event)
{
  printf("Mouse entered into button!\n");
}

void mouse_leave_callback(button* btn, mouse_motion_event event)
{
  printf("Mouse left button!\n");
}
