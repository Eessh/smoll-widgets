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

  smoll_context_set_default_font(sctx, "Consolas");
  smoll_context_set_default_font_size(sctx, 18);

  button* btn = NULL;
  {
    result_button_ptr _ = button_new(NULL, "Hola!");
    if(!_.ok)
    {
      printf("Error while creating button: %s", _.error);
    }
    btn = _.value;
  }

  smoll_context_set_root_widget(sctx, btn->base);

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
  }

cleanup:
  button_free(btn);
  smoll_context_destroy(sctx);
  sdl2_cairo_backend_destroy(backend);

  return 0;
}