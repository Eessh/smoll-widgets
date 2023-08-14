#include "../../include/widgets/progress_bar.h"
#include <stdlib.h>
#include "../../include/macros.h"

struct progress_bar_private
{
  uint8 percent;

  color foreground;
  color background;
};

static result_bool default_internal_render_callback(const base_widget* widget);

result_progress_bar_ptr progress_bar_new(base_widget* parent_base,
                                         uint8 percent,
                                         color foreground,
                                         color background)
{
  progress_bar* bar = (progress_bar*)calloc(1, sizeof(progress_bar));
  if(!bar)
  {
    return error(result_progress_bar_ptr,
                 "Unable to allocate memory for progress_bar widget!");
  }

  result_base_widget_ptr _ = base_widget_new();
  if(!_.ok)
  {
    free(bar);
    return error(result_progress_bar_ptr, _.error);
  }

  progress_bar_private* bar_private =
    (progress_bar_private*)calloc(1, sizeof(progress_bar_private));
  if(!bar_private)
  {
    free(bar);
    base_widget_free(_.value);
    return error(result_progress_bar_ptr,
                 "Unable to allocate memory for progress_bar private fields!");
  }

  bar->base = _.value;
  bar->base->derived = bar;
  bar->base->parent = parent_base;

  if(parent_base)
  {
    base_widget_add_child(parent_base, bar->base);
    bar->base->context = parent_base->context;
  }

  bar->base->internal_render_callback = default_internal_render_callback;

  bar_private->percent = percent;
  bar_private->foreground = foreground;
  bar_private->background = background;
  bar->private_data = bar_private;

  return ok(result_progress_bar_ptr, bar);
}

result_uint8 progress_bar_get_percent(const progress_bar* bar)
{
  if(!bar)
  {
    return error(result_uint8,
                 "Cannot get percent of a NULL pointed progress_bar!");
  }

  return ok(result_uint8, bar->private_data->percent);
}

result_color progress_bar_get_foreground(const progress_bar* bar)
{
  if(!bar)
  {
    return error(result_color,
                 "Cannot get foreground of a NULL pointed progress_bar!");
  }

  return ok(result_color, bar->private_data->foreground);
}

result_color progress_bar_get_background(const progress_bar* bar)
{
  if(!bar)
  {
    return error(result_color,
                 "Cannot get background of a NULL pointed progress_bar!");
  }

  return ok(result_color, bar->private_data->background);
}

result_void progress_bar_set_percent(progress_bar* bar, uint8 percent)
{
  if(!bar)
  {
    return error(result_void,
                 "Cannot set percent of a NULL pointed progress_bar!");
  }

  if(bar->private_data->percent == percent)
  {
    return ok_void();
  }

  bar->private_data->percent = percent;
  bar->base->internal_render_callback(bar->base);

  return ok_void();
}

result_void progress_bar_set_foreground(progress_bar* bar, color foreground)
{
  if(!bar)
  {
    return error(result_void,
                 "Cannot set foreground of a NULL pointed progress_bar!");
  }

  bar->private_data->foreground = foreground;
  rect bounding_rect =
    bar->base->internal_get_bounding_rect_callback(bar->base);
  bounding_rect.w *= (float32)(bar->private_data->percent / 100.0f);

  result_void _ = command_buffer_add_render_rect_command(
    bar->base->context->cmd_buffer, bounding_rect, foreground);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  return ok_void();
}

result_void progress_bar_set_background(progress_bar* bar, color background)
{
  if(!bar)
  {
    return error(result_void,
                 "Cannot set background of a NULL pointed progress_bar!");
  }

  bar->private_data->background = background;
  bar->base->internal_render_callback(bar->base);

  return ok_void();
}

static result_bool default_internal_render_callback(const base_widget* widget)
{
  progress_bar* bar = (progress_bar*)widget->derived;
  rect bounding_rect =
    bar->base->internal_get_bounding_rect_callback(bar->base);

  result_void _ =
    command_buffer_add_render_rect_command(bar->base->context->cmd_buffer,
                                           bounding_rect,
                                           bar->private_data->background);
  if(!_.ok)
  {
    return error(result_bool, _.error);
  }

  bounding_rect.w *= (float32)(bar->private_data->percent / 100.0f);

  _ = command_buffer_add_render_rect_command(bar->base->context->cmd_buffer,
                                             bounding_rect,
                                             bar->private_data->foreground);
  if(!_.ok)
  {
    return error(result_bool, _.error);
  }

  return ok(result_bool, true);
}
