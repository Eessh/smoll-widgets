#include "../../include/widgets/scrollbar.h"
#include <stdlib.h>
#include "../../include/macros.h"

// forward declaration
typedef struct list_view list_view;

extern result_bool
list_view_set_scroll_offset_from_scrollbar(list_view* view,
                                           const float32* new_scroll_offset);

typedef enum scrollbar_state
{
  SCROLLBAR_NORMAL,
  SCROLLBAR_HOVERED,
  SCROLLBAR_CLICKED
} scrollbar_state;

struct scrollbar_private
{
  scrollbar_state state;

  scrollbar_target_descriptor target_descriptor;

  float32 scroll_offset;
};

static void default_internal_derived_free_callback(base_widget* widget);

static result_sizing_delta
default_internal_fit_layout_callback(base_widget* widget,
                                     bool call_on_children);

static result_bool default_internal_render_callback(const base_widget* widget);

static bool default_mouse_button_down_callback(base_widget* widget,
                                               mouse_button_event event);

static bool default_mouse_button_up_callback(base_widget* widget,
                                             mouse_button_event event);

static bool default_mouse_enter_callback(base_widget* widget,
                                         mouse_motion_event event);

static bool default_mouse_leave_callback(base_widget* widget,
                                         mouse_motion_event event);

result_scrollbar_ptr
scrollbar_new(base_widget* parent_base,
              const scrollbar_descriptor* const descriptor,
              const scrollbar_target_descriptor* const target_descriptor)
{
  if(!target_descriptor)
  {
    return error(
      result_scrollbar_ptr,
      "Cannot create scrollbar with target descriptor pointing to NULL!");
  }

  scrollbar* bar = (scrollbar*)calloc(1, sizeof(scrollbar));
  if(!bar)
  {
    return error(result_scrollbar_ptr,
                 "Unable to allocate memory for scrollbar widget!");
  }

  result_base_widget_ptr _ = base_widget_new(FLEX_ITEM);
  if(!_.ok)
  {
    free(bar);
    return error(result_scrollbar_ptr, _.error);
  }

  scrollbar_private* bar_private =
    (scrollbar_private*)calloc(1, sizeof(scrollbar_private));
  if(!bar_private)
  {
    base_widget_free(_.value);
    free(bar);
    return error(result_scrollbar_ptr,
                 "Unable to allocate memory for private fileds of scrollbar!");
  }

  bar->base = _.value;
  bar->base->derived = bar;
  bar->base->parent = parent_base;

  bar->private_data = bar_private;

  // attaching this scrollbar to parent's children list
  if(parent_base)
  {
    base_widget_add_child(parent_base, bar->base);
  }

  // assigning context
  if(parent_base)
  {
    bar->base->context = parent_base->context;
  }

  // assigning callbacks
  bar->base->internal_fit_layout_callback =
    default_internal_fit_layout_callback;
  bar->base->internal_render_callback = default_internal_render_callback;

  bar->base->internal_derived_free_callback =
    default_internal_derived_free_callback;

  bar->base->mouse_button_down_callback = default_mouse_button_down_callback;
  bar->base->mouse_button_up_callback = default_mouse_button_up_callback;
  bar->base->mouse_enter_callback = default_mouse_enter_callback;
  bar->base->mouse_leave_callback = default_mouse_leave_callback;

  bar->base->flexbox_data.item.flex_grow = 1;

  // assigning descriptors
  bar->descriptor =
    descriptor != NULL ? *descriptor : default_scrollbar_descriptor;
  bar->private_data->state = SCROLLBAR_NORMAL;
  bar->private_data->target_descriptor = *target_descriptor;
  bar->private_data->scroll_offset = 0;

  return ok(result_scrollbar_ptr, bar);
}

result_scrollbar_ptr scrollbar_new_with_debug_name(
  base_widget* parent_base,
  const scrollbar_descriptor* const descriptor,
  const scrollbar_target_descriptor* const target_descriptor,
  const char* debug_name)
{
  result_scrollbar_ptr _ =
    scrollbar_new(parent_base, descriptor, target_descriptor);
  if(!_.ok)
  {
    return _;
  }

  _.value->base->debug_name = debug_name;

  trace("Scrollbar: created with debug-name: %s", debug_name);

  return _;
}

result_float32 scrollbar_get_scroll_offset(const scrollbar* bar) {
  if(!bar)
  {
    return error(result_float32,
                 "Cannot get scroll offset of scrollbar pointing to NULL!");
  }

  return ok(result_float32, bar->private_data->scroll_offset);
}

result_bool scrollbar_set_scroll_offset(scrollbar* bar,
    const float32* new_scroll_offset)
{
  if(!bar)
  {
    return error(result_bool,
                 "Cannot set scroll offset of scrollbar pointing to NULL!");
  }

  if(!new_scroll_offset)
  {
    return error(result_bool,
                 "Cannot set NULL pointing scroll offset to given scrollbar!");
  }

  bar->private_data->scroll_offset = *new_scroll_offset;

  // setting target list view's scroll offset
  list_view* target_list_view = (list_view*)(bar->private_data->target_descriptor.base->derived);
  const float32 target_scroll_offset = 1.0f;
  list_view_set_scroll_offset_from_scrollbar(target_list_view, &target_scroll_offset);

  return bar->base->internal_render_callback(bar->base);
}

static void default_internal_derived_free_callback(base_widget* widget)
{
  trace("Scrollbar(%s): internal-derived-free()", widget->debug_name);

  scrollbar* bar = (scrollbar*)widget->derived;

  // freeing scrollbar's private data
  free(bar->private_data);

  // freeing scrollbar
  // freeing base_widget is taken care by internal_free_callback
  free(bar);
}

static result_sizing_delta
default_internal_fit_layout_callback(base_widget* widget, bool call_on_children)
{
  trace("Scrollbar(%s): internal-fit-layout()", widget->debug_name);

  // Scrollbar doesn't have children, its main-axis-size will not change
  // until unless its parent's size has changed.

  scrollbar* bar = (scrollbar*)widget->derived;

  uint16 new_w = bar->descriptor.cross_axis_width;
  widget->w = new_w;

  sizing_delta deltas = {.x = (int16)new_w - (int16)widget->w, .y = 0};

  return ok(result_sizing_delta, deltas);
}

static result_bool default_internal_render_callback(const base_widget* widget)
{
  scrollbar* bar = (scrollbar*)widget->derived;

  trace("Scrollbar(%s): internal-render(), (x, y, w, h): (%d, %d, %d, %d)",
        widget->debug_name,
        widget->x,
        widget->y,
        widget->w,
        widget->h);

  rect bounding_rect = common_internal_get_bounding_rect(widget);

  result_void _ = command_buffer_add_render_rect_command(
    widget->context->cmd_buffer, bounding_rect, bar->descriptor.background);
  if(!_.ok)
  {
    return error(result_bool, _.error);
  }

  // adjusting bounding rect for scroll thumb
  bounding_rect.y += bar->private_data->scroll_offset;
  bounding_rect.h =
    (uint16)(((float32)(bar->private_data->target_descriptor.base->h) /
              (int32)(bar->private_data->target_descriptor.content_length)) *
             widget->h);
  trace("Scrollbar(%s): target_base_height: %d, content_length: %d, "
        "bounding_rect_height: %d",
        widget->debug_name,
        bar->private_data->target_descriptor.base->h,
        bar->private_data->target_descriptor.content_length,
        bounding_rect.h);

  _ = command_buffer_add_render_rect_command(
    widget->context->cmd_buffer, bounding_rect, bar->descriptor.foreground);
  if(!_.ok)
  {
    return error(result_bool, _.error);
  }

  return ok(result_bool, true);
}

static bool default_mouse_button_down_callback(base_widget* widget,
                                               mouse_button_event event)
{
  return false;
}

static bool default_mouse_button_up_callback(base_widget* widget,
                                             mouse_button_event event)
{
  return false;
}

static bool default_mouse_enter_callback(base_widget* widget,
                                         mouse_motion_event event)
{
  return false;
}

static bool default_mouse_leave_callback(base_widget* widget,
                                         mouse_motion_event event)
{
  return false;
}
