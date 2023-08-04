#include "../../include/smoll_context.h"
#include "sdl2_cairo_backend.h"

int main()
{
  smoll_context* sctx = smoll_context_create().value;
  render_backend* backend = sdl2_cairo_backend_create().value;
  smoll_context_register_backend(sctx, backend);

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
        smoll_context_process_mouse_wheel_event(
          sctx, translate_sdl2_mouse_wheel_event(event.wheel));
      }
    }
  }

cleanup:
  smoll_context_destroy(sctx);
  sdl2_cairo_backend_destroy(backend);

  return 0;
}