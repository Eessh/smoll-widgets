#include "../../include/widgets/label.h"
#include <stdlib.h>
#include <string.h>
#include "../../include/macros.h"

struct label_private
{
  char* text;
  color text_color;
};

static void default_internal_derived_free_callback(base_widget* widget);

static result_sizing_delta
default_internal_fit_layout_callback(base_widget* widget,
                                     bool call_on_children);

static result_bool default_internal_render_callback(const base_widget* widget);

result_label_ptr label_new(base_widget* parent_base, const char* text)
{
  if(!text)
  {
    return error(result_label_ptr,
                 "Cannot create a label for NULL pointing text!");
  }

  label* l = (label*)calloc(1, sizeof(label));
  if(!l)
  {
    return error(result_label_ptr, "Unable to allocate memory for label!");
  }

  result_base_widget_ptr _ = base_widget_new(FLEX_ITEM);
  if(!_.ok)
  {
    free(l);
    return error(result_label_ptr, _.error);
  }

  label_private* private_data =
    (label_private*)calloc(1, sizeof(label_private));
  if(!private_data)
  {
    free(l);
    base_widget_free(_.value);
    return error(result_label_ptr,
                 "Unable to allocate memory for private fields of label!");
  }

  private_data->text = NULL;
  private_data->text_color = (color){255, 255, 255, 255};

  private_data->text = strdup(text);
  if(!private_data->text)
  {
    free(l);
    base_widget_free(_.value);
    free(private_data);
    return error(result_label_ptr,
                 "Unable to allocate memory for text of label!");
  }

  l->base = _.value;
  l->base->derived = l;
  l->base->parent = parent_base;

  l->private_data = private_data;

  if(parent_base)
  {
    base_widget_add_child(parent_base, l->base);
  }

  l->base->internal_derived_free_callback =
    default_internal_derived_free_callback;
  l->base->internal_fit_layout_callback = default_internal_fit_layout_callback;
  l->base->internal_render_callback = default_internal_render_callback;

  return ok(result_label_ptr, l);
}

result_label_ptr label_new_with_debug_name(base_widget* parent_base,
                                           const char* text,
                                           const char* debug_name)
{
  result_label_ptr _ = label_new(parent_base, text);
  if(!_.ok)
  {
    return _;
  }

  _.value->base->debug_name = strdup(debug_name);

  return _;
}

result_const_char_ptr label_get_text(const label* l)
{
  if(!l)
  {
    return error(result_const_char_ptr,
                 "Cannot get text of a label pointing to NULL!");
  }

  return ok(result_const_char_ptr, (const char*)l->private_data->text);
}

result_bool label_set_text(label* l, const char* text)
{
  if(!l)
  {
    return error(result_bool, "Cannot set text of a label pointing to NULL!");
  }

  if(!text)
  {
    return error(result_bool, "Cannot set NULL pointing text to label!");
  }

  char* duplicated_text = strdup(text);
  if(!duplicated_text)
  {
    return ok(result_bool, false);
  }

  free(l->private_data->text);
  l->private_data->text = duplicated_text;

  common_internal_adjust_layout(l->base);

  return ok(result_bool, true);
}

result_color label_get_color(const label* l)
{
  if(!l)
  {
    return error(result_color, "Cannot get color of a label pointing to NULL!");
  }

  return ok(result_color, l->private_data->text_color);
}

result_bool label_set_color(label* l, const color* c)
{
  if(!l)
  {
    return error(result_bool, "Cannot set color to NULL pointing label!");
  }

  if(!c)
  {
    return error(result_bool, "Cannot set NULL pointing color to label!");
  }

  l->private_data->text_color = *c;

  return ok(result_bool, true);
}

static void default_internal_derived_free_callback(base_widget* widget)
{
  trace("label(%s): internal-derived-free()", widget->debug_name);

  label* l = (label*)widget->derived;

  // freeing label text
  free(l->private_data->text);
  // freeing label private struct
  free(l->private_data);
  // freeing label object
  // freeing base_widget is taken care by internal_free_callback
  free(l);
}

static result_sizing_delta
default_internal_fit_layout_callback(base_widget* widget, bool call_on_children)
{
  trace("label(%s): internal-fit-layout()", widget->debug_name);

  label* l = (label*)widget->derived;

  result_text_dimensions ___ = widget->context->backend->get_text_dimensions(
    l->private_data->text, widget->context->font, widget->context->font_size);
  if(!___.ok)
  {
    return error(result_sizing_delta, ___.error);
  }
  text_dimensions dimensions = ___.value;

  debug("  > Text: \"%s\", dimensions: %d, %d",
        l->private_data->text,
        dimensions.w,
        dimensions.h);
  debug(
    "  > Widget dimensions: %d, %d | padding: %d, %d", widget->w, widget->h);

  sizing_delta deltas = {.x = (int16)dimensions.w - (int16)widget->w,
                         .y = (int16)dimensions.h - (int16)widget->h};

  info("  > sizing-deltas: (%d, %d)", deltas.x, deltas.y);

  // setting widget's sizing, so no need to set need resizing flag
  widget->w = dimensions.w;
  widget->h = dimensions.h;

  return ok(result_sizing_delta, deltas);
}

static result_bool default_internal_render_callback(const base_widget* widget)
{
  label* l = (label*)widget->derived;
  color foreground = l->private_data->text_color;

  info("Label(%s): internal-render(), text: \"%s\", (x, y, w, h): (%d, %d, "
       "%d, %d), color: (%d, %d, %d, %d)",
       widget->debug_name,
       l->private_data->text,
       widget->x,
       widget->y,
       widget->w,
       widget->h,
       foreground.r,
       foreground.g,
       foreground.b,
       foreground.a);

  rect bounding_rect = common_internal_get_bounding_rect(widget);

  result_void _ = command_buffer_add_render_rect_command(
    widget->context->cmd_buffer,
    bounding_rect,
    widget->parent->internal_get_background_callback(widget->parent));
  if(!_.ok)
  {
    return error(result_bool, _.error);
  }

  result_void ___ = command_buffer_add_render_text_command(
    widget->context->cmd_buffer,
    l->private_data->text,
    foreground,
    (point){.x = bounding_rect.x, .y = bounding_rect.y});
  if(!___.ok)
  {
    return error(result_bool, ___.error);
  }

  return ok(result_bool, true);
}
