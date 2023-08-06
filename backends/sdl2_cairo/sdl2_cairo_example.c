#include <stdio.h>
#include "../../include/smoll_context.h"
#include "../../include/widgets/button.h"
#include "sdl2_cairo_backend.h"

int main()
{
  smoll_context* sctx = NULL;
  render_backend* backend = NULL;

  {
    result_smoll_context_ptr _ = smoll_context_create();
    if(!_.ok)
    {
      printf("Error: %s", _.error);
      exit(1);
    }
    sctx = _.value;
  }

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

  smoll_context_set_default_font(sctx, "Consolas", 18);

  button* btn = NULL;
  {
    result_button_ptr _ = button_new(NULL, "Hola!");
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
  }

  smoll_context_set_root_widget(sctx, btn->base);

  smoll_context_initial_fit_layout(sctx);
  smoll_context_initial_render(sctx);

  printf("Created context, render backend and attached backend.\n");

  while(1)
  {
    SDL_Event event;
    if(SDL_WaitEvent(&event))
    {
      if(event.type == SDL_QUIT)
      {
        goto cleanup;
      }
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

    smoll_context_render(sctx);
    SDL_UpdateWindowSurface(sdl2_cairo_backend_get_window());
  }

cleanup:
  // this is taken care by smoll_context_destroy
  //  button_free(btn);
  smoll_context_destroy(sctx);
  sdl2_cairo_backend_destroy(backend);

  return 0;
}