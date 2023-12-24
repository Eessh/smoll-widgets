#include <stdio.h>
#include "../../include/smoll_context.h"
#include "../../include/widgets/box.h"
#include "../../include/widgets/button.h"
#include "../../include/widgets/checkbox.h"
#include "../../include/widgets/flex_view.h"
#include "../../include/widgets/list_view.h"
#include "../../include/widgets/progress_bar.h"
// #include "../../include/widgets/split.h"
#include "../../include/widgets/split_view.h"
#include "../../include/widgets/toggle.h"
#include "sdl2_cairo_backend.h"

progress_bar* bar = NULL;
checkbox* cbox = NULL;

// Callbacks function prototypes
void mouse_button_down_callback(button* btn, mouse_button_event event);
void mouse_button_up_callback(button* btn, mouse_button_event event);
void mouse_enter_callback(button* btn, mouse_motion_event event);
void mouse_leave_callback(button* btn, mouse_motion_event event);
void on_callback(toggle* t);
void off_callback(toggle* t);

int main()
{
  smoll_context* sctx = NULL;
  render_backend* backend = NULL;

  // Creating smoll context
  {
    result_smoll_context_ptr _ = smoll_context_create(1080, 720);
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
  smoll_context_set_default_font(sctx, "Consolas", 14);

  // Creating root box widget
  box* bx = NULL;
  {
    result_box_ptr _ =
      box_new_with_debug_name(NULL, FLEX_DIRECTION_ROW, "root");
    if(!_.ok)
    {
      printf("Error while creating box: %s", _.error);
    }
    bx = _.value;
    bx->background = (color){255, 255, 255, 255};
    bx->base->flexbox_data.container.is_fluid = false;
  }

  // Setting box widget as root widget of smoll context
  // root widget has to be fixed size
  smoll_context_set_root_widget(sctx, bx->base);

  // creating split view
  split_view* split = NULL;
  {
    result_split_view_ptr _ = split_view_new(bx->base, SPLIT_VERTICAL);
    if(!_.ok)
    {
      printf("Error while creating split-view: %s", _.error);
    }
    split = _.value;
    split->base->flexbox_data.container.flex_grow = 1;
    split->base->flexbox_data.container.cross_axis_sizing =
      CROSS_AXIS_SIZING_EXPAND;
  }

  // Creating flex-row view
  // flex_view* row_view = NULL;
  // {
  //   result_flex_view_ptr _ =
  //     flex_view_new_with_debug_name(NULL, FLEX_DIRECTION_ROW, "left-pane");
  //   if(!_.ok)
  //   {
  //     printf("Error while creating flex-row view: %s", _.error);
  //   }
  //   row_view = _.value;
  //   row_view->base->flexbox_data.container.flex_grow = 1;
  //   row_view->base->flexbox_data.container.align_items = ALIGN_ITEMS_CENTER;
  //   row_view->base->flexbox_data.container.justify_content =
  //     JUSTIFY_CONTENT_START;
  //   row_view->base->flexbox_data.container.cross_axis_sizing =
  //     CROSS_AXIS_SIZING_EXPAND;
  //   row_view->base->flexbox_data.container.gap = 10;
  //   row_view->background = (color){128, 128, 128, 255};
  // }

  list_view* row_view = NULL;
  {
    result_list_view_ptr _ =
      list_view_new_with_debug_name(NULL, "left-list-view");
    if(!_.ok)
    {
      printf("Error while creating list-view: %s", _.error);
    }
    row_view = _.value;
    row_view->base->flexbox_data.container.cross_axis_sizing =
      CROSS_AXIS_SIZING_EXPAND;
    row_view->base->flexbox_data.container.flex_grow = 1;
    row_view->base->flexbox_data.container.gap = 10;
  }

  // Creating another flex view
  flex_view* col_view = NULL;
  {
    result_flex_view_ptr _ =
      flex_view_new_with_debug_name(NULL, FLEX_DIRECTION_COLUMN, "right-pane");
    if(!_.ok)
    {
      printf("Error while creating flex-column view: %s", _.error);
    }
    col_view = _.value;
    col_view->base->flexbox_data.container.flex_grow = 1;
    col_view->base->flexbox_data.container.align_items = ALIGN_ITEMS_START;
    col_view->base->flexbox_data.container.justify_content =
      JUSTIFY_CONTENT_START;
    col_view->base->flexbox_data.container.cross_axis_sizing =
      CROSS_AXIS_SIZING_EXPAND;
    col_view->background = (color){33, 66, 99, 255};
  }

  // attaching flex row view to split view
  split_view_connect_children(split, row_view->base, col_view->base);

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

  {
    result_button_ptr _ = button_new(row_view->base, "Cola!");
    if(!_.ok)
    {
      printf("Error while creating button: %s", _.error);
    }
    button* b = _.value;
    b->foreground = (color){255, 255, 255, 255};
    b->background = (color){16, 16, 16, 255};

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){16, 16, 16, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){26, 26, 26, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){36, 36, 36, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){46, 46, 46, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){56, 56, 56, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){66, 66, 66, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){76, 76, 76, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){86, 86, 86, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){96, 96, 96, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){106, 106, 106, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){116, 116, 116, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){126, 126, 126, 126};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){136, 136, 136, 136};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){146, 146, 146, 146};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){156, 156, 156, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){166, 166, 166, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){176, 176, 176, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){186, 186, 186, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){196, 196, 196, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){206, 206, 206, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){216, 216, 216, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){216, 216, 216, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){216, 216, 216, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;

    _ = button_new(row_view->base, "Cola!");
    _.value->foreground = (color){255, 255, 255, 255};
    _.value->background = (color){216, 216, 216, 255};
    _.value->border_radius = 5;
    _.value->padding_x = 6;
    _.value->padding_y = 4;
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

  // Creating another button widget
  button* btn2 = NULL;
  {
    result_button_ptr _ = button_new(col_view->base, "Hello there!");
    if(!_.ok)
    {
      printf("Error while creating button: %s", _.error);
    }
    btn2 = _.value;
    btn2->padding_x = 6;
    btn2->padding_y = 4;
    btn2->foreground = (color){255, 255, 255, 255};
    btn2->background = (color){16, 16, 16, 255};
    btn2->hover_foreground = (color){0, 255, 0, 255};
    btn2->hover_background = (color){64, 64, 64, 255};
    btn2->click_foreground = (color){255, 0, 0, 255};
    btn2->click_background = (color){128, 128, 128, 255};
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
    //    t->base->flexbox_data.item.cross_axis_sizing = CROSS_AXIS_SIZING_EXPAND;
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
    bar->base->debug_name = "progress-bar";
  }

  // Creating checkbox
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
  smoll_context_initialize_layout(sctx);
  smoll_context_initial_render(sctx);

  /// initial window surface update
  SDL_UpdateWindowSurface(sdl2_cairo_backend_get_window());

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
      else if(event.type == SDL_WINDOWEVENT)
      {
        if(event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
          smoll_context_process_viewport_resize_event(
            sctx, translate_sdl2_window_resize_event(event.window));
        }
        else if(event.window.event == SDL_WINDOWEVENT_RESTORED ||
                event.window.event == SDL_WINDOWEVENT_MAXIMIZED)
        {
          smoll_context_process_viewport_resize_event(
            sctx, translate_sdl2_window_maximized_or_restored_event());
        }
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

    // Rendering incremental updates
    // smoll_context_render(sctx);

    // Rendering all updates at once
    smoll_context_render_send_cmd_buffer_to_backend(sctx);

    // Updating window surface
    // SDL_UpdateWindowSurface(sdl2_cairo_backend_get_window());
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
  widget_set_visibility(cbox->base, true);
}

void off_callback(toggle* t)
{
  printf("Setting progress bar percent to: 60\n");
  progress_bar_set_percent(bar, 60);
  widget_set_visibility(cbox->base, false);
}
