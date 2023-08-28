#include "../include/base_widget.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/macros.h"

/**
 * @brief      Default callback function for internal mark need resizing
 *             callback.
 *
 * @param      widget   pointer to widget.
 * @param[in]  delta    change in size in x-axis.
 * @param[in]  delta_y  change in size in y-axis.
 *
 * @return     Base widget pointer result.
 */
static result_base_widget_ptr default_internal_mark_need_resizing(
  base_widget* widget, int16 delta_x, int16 delta_y);

/**
 * @brief      Default callback function for internal calculate size
 *             callback.
 *
 * @param      widget  pointer to widget.
 *
 * @return     Void result.
 */
static result_void
default_internal_calculate_size_callback(base_widget* widget);

/**
 * @brief      Default callback function for internal relayout callback.
 *
 * @param[in]  widget  pointer to widget.
 *
 * @return     Void result.
 */
static result_void
default_internal_relayout_callback(const base_widget* widget);

/// @brief Default callback function for internal bounding rect callback.
/// @param widget constant pointer to base widget.
/// @return Returns rect struct.
static rect
default_internal_get_bounding_rect_callback(const base_widget* widget);

static result_bool default_internal_adjust_layout_callback(base_widget* widget);

/// @brief Default callback function for internal free callback.
/// @param widget pointer to base widget.
/// @return Void.
static void default_internal_free_callback(base_widget* widget);

/// @brief Default callback function for internal mouse motion callback.
/// @param widget pointer to base widget.
/// @param internal_event pointer to internal mouse motion event.
/// @return Bool result.
static result_bool default_internal_mouse_motion_callback(
  base_widget* widget, internal_mouse_motion_event* internal_event);

/// @brief Default callback function for internal mouse button callback.
/// @param widget pointer to base widget.
/// @param internal_event pointer to internal mouse button event.
/// @return Bool result.
static result_bool default_internal_mouse_button_callback(
  base_widget* widget, internal_mouse_button_event* internal_event);

/// @brief Default callback function for internal mouse scroll callback.
/// @param widget pointer to base widget.
/// @param internal_event pointer to internal mouse scroll event.
/// @return Bool result.
static result_bool default_internal_mouse_scroll_callback(
  base_widget* widget, internal_mouse_scroll_event* internal_event);

result_base_widget_child_node_ptr base_widget_child_node_new(base_widget* child)
{
  if(!child)
  {
    return error(result_base_widget_child_node_ptr,
                 "Cannot create new child node for NULL pointed child!");
  }

  base_widget_child_node* node =
    (base_widget_child_node*)calloc(1, sizeof(base_widget_child_node));
  if(!node)
  {
    return error(result_base_widget_child_node_ptr,
                 "Unable to allocate memory for new child node!");
  }

  node->child = child;
  node->next = NULL;

  return ok(result_base_widget_child_node_ptr, node);
}

result_void base_widget_child_node_free(base_widget_child_node* node)
{
  if(!node)
  {
    return error(result_void, "Attempt to free a NULL pointed child node!");
  }

  free(node);

  return ok_void();
}

result_base_widget_ptr base_widget_new(widget_type type)
{
  base_widget* widget = (base_widget*)calloc(1, sizeof(base_widget));
  if(!widget)
  {
    return error(result_base_widget_ptr,
                 "Unable to allocate memory for base_widget!");
  }

  widget->x = 0;
  widget->y = 0;
  widget->w = 0;
  widget->h = 0;

  widget->type = type;
  if(type == FLEX_CONTAINER)
  {
    widget->flexbox_data.container =
      (flex_container_data){.is_fluid = true,
                            .direction = FLEX_DIRECTION_ROW,
                            .justify_content = FLEX_ALIGN_START,
                            .align_items = FLEX_ALIGN_START,
                            .flex_grow = 0,
                            .flex_shrink = 0,
                            .cross_axis_sizing = CROSS_AXIS_SIZING_FIT_CONTENT};
  }
  else
  {
    widget->flexbox_data.item =
      (flex_item_data){.flex_grow = 0,
                       .flex_shrink = 0,
                       .cross_axis_sizing = CROSS_AXIS_SIZING_FIT_CONTENT};
  }

  widget->need_resizing = true;

  widget->visible = true;

  widget->derived = NULL;
  widget->parent = NULL;
  widget->children_head = NULL;

  widget->context = NULL;

  widget->internal_mark_need_resizing = default_internal_mark_need_resizing;
  widget->internal_calculate_size = default_internal_calculate_size_callback;
  widget->internal_relayout = default_internal_relayout_callback;
  widget->internal_get_bounding_rect_callback =
    default_internal_get_bounding_rect_callback;
  widget->internal_get_background_callback = NULL;
  widget->internal_fit_layout_callback = NULL;
  widget->internal_adjust_layout_callback =
    default_internal_adjust_layout_callback;
  widget->internal_assign_positions = NULL;
  widget->internal_render_callback = NULL;

  widget->internal_free_callback = default_internal_free_callback;
  widget->internal_derived_free_callback = NULL;

  widget->internal_mouse_motion_callback =
    default_internal_mouse_motion_callback;
  widget->internal_mouse_button_callback =
    default_internal_mouse_button_callback;
  widget->internal_mouse_scroll_callback =
    default_internal_mouse_scroll_callback;

  widget->mouse_button_down_callback = NULL;
  widget->mouse_button_up_callback = NULL;
  widget->mouse_enter_callback = NULL;
  widget->mouse_leave_callback = NULL;
  widget->mouse_scroll_callback = NULL;

  return ok(result_base_widget_ptr, widget);
}

result_void base_widget_add_child(base_widget* base, base_widget* child)
{
  if(!base)
  {
    return error(result_void,
                 "Cannot attach child to NULL pointed parent widget!");
  }

  if(base->type == FLEX_ITEM)
  {
    return error(result_void,
                 "Cannot attach child to widget of type FLEX_ITEM!");
  }

  if(!child)
  {
    return error(result_void,
                 "Cannot attach NULL pointed child to parent widget!");
  }

  child->context = base->context;

  if(!base->children_head)
  {
    result_base_widget_child_node_ptr _ = base_widget_child_node_new(child);
    if(!_.ok)
    {
      return error(result_void, _.error);
    }

    child->parent = base;
    base->children_head = _.value;

    return ok_void();
  }

  base_widget_child_node* temp = base->children_head;
  while(temp->next)
  {
    temp = temp->next;
  }
  result_base_widget_child_node_ptr _ = base_widget_child_node_new(child);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  child->parent = base;
  temp->next = _.value;

  return ok_void();
}

result_void base_widget_remove_child(base_widget* base, base_widget* child)
{
  if(!base)
  {
    return error(result_void,
                 "Cannot remove child from NULL pointed parent widget!");
  }

  if(!child)
  {
    return error(result_void,
                 "Cannot remove NULL pointed child from parent widget!");
  }

  if(!base->children_head)
  {
    return error(result_void,
                 "Cannot remove child from empty children list of parent!");
  }

  base_widget_child_node* temp = base->children_head;
  base_widget_child_node* prev_temp = NULL;
  while(temp->child != child)
  {
    prev_temp = temp;
    temp = temp->next;
  }

  if(!temp)
  {
    // child node not found with given child
    return error(result_void,
                 "Child node not found with given child to remove!");
  }

  prev_temp->next = temp->next;
  temp->child->parent = NULL;
  base_widget_child_node_free(temp);

  return ok_void();
}

result_void base_widget_free(base_widget* widget)
{
  if(!widget)
  {
    return error(result_void, "Attempt to free a NULL pointed base widget!");
  }

  free(widget);

  return ok_void();
}

static result_base_widget_ptr default_internal_mark_need_resizing(
  base_widget* widget, int16 delta_x, int16 delta_y)
{
  if(widget->type == FLEX_ITEM)
  {
    widget->need_resizing = false;
    if(widget->parent)
    {
      return widget->parent->internal_mark_need_resizing(
        widget->parent, delta_x, delta_y);
    }
    return ok(result_base_widget_ptr, widget);
  }

  if(!widget->flexbox_data.container.is_fluid)
  {
    // widget is not fluid, doesn't resize
    printf("Encountered non-fluid widget\n");
    return ok(result_base_widget_ptr, widget);
  }

  if(widget->flexbox_data.container.direction == FLEX_DIRECTION_ROW)
  {
    // consume delta_x from main axis, delta_y from cross axis
    uint16 main_axis_length = widget->w, cross_axis_length = widget->h;
    uint16 needed_main_axis_length = 0, needed_cross_axis_length = 0;
    base_widget_child_node* node = widget->children_head;
    while(node)
    {
      needed_main_axis_length += node->child->w;
      needed_cross_axis_length = max(needed_cross_axis_length, node->child->h);
      node = node->next;
    }
    int16 remaining_main_axis_length =
      main_axis_length - needed_main_axis_length;
    if(remaining_main_axis_length > 0)
    {
      // main axis size needs to be decreased
      if(remaining_main_axis_length >= -delta_x)
      {
        if(!widget->flexbox_data.container.is_fluid)
        {
          delta_x = 0;
        }
      }
      else
      {
        delta_x += remaining_main_axis_length;
      }
    }
    else if(remaining_main_axis_length < 0)
    {
      // main axis size needs to be increased
      if(remaining_main_axis_length >= delta_x)
      {
        delta_x = 0;
      }
      else
      {
        delta_x -= remaining_main_axis_length;
      }
    }
    int16 remaining_cross_axis_length =
      cross_axis_length - needed_cross_axis_length;
    if(remaining_cross_axis_length > 0)
    {
      // main axis size needs to be decreased
      if(remaining_cross_axis_length >= -delta_y)
      {
        if(!widget->flexbox_data.container.is_fluid)
        {
          delta_y = 0;
        }
      }
      else
      {
        delta_y += remaining_cross_axis_length;
      }
    }
    else if(remaining_cross_axis_length < 0)
    {
      // main axis size needs to be increased
      if(remaining_cross_axis_length >= delta_y)
      {
        delta_y = 0;
      }
      else
      {
        delta_y -= remaining_cross_axis_length;
      }
    }
  }
  else
  {
    // consume delta_y from main axis, delta_x from cross axis
    uint16 main_axis_length = widget->h, cross_axis_length = widget->w;
    uint16 needed_main_axis_length = 0, needed_cross_axis_length = 0;
    base_widget_child_node* node = widget->children_head;
    while(node)
    {
      needed_main_axis_length += node->child->h;
      needed_cross_axis_length = max(needed_cross_axis_length, node->child->w);
      node = node->next;
    }
    int16 remaining_main_axis_length =
      main_axis_length - needed_main_axis_length;
    if(remaining_main_axis_length > 0)
    {
      // main axis should be shrinked
      if(remaining_main_axis_length > -delta_y)
      {
        if(!widget->flexbox_data.container.is_fluid)
        {
          delta_y = 0;
        }
      }
      else
      {
        delta_y += remaining_main_axis_length;
      }
    }
    else if(remaining_main_axis_length < 0)
    {
      // main axis should be increased
      if(remaining_main_axis_length > delta_y)
      {
        delta_y = 0;
      }
      else
      {
        delta_y -= remaining_main_axis_length;
      }
    }
    int16 remaining_cross_axis_length =
      cross_axis_length - needed_cross_axis_length;
    if(remaining_cross_axis_length > 0)
    {
      // cross axis should be shrinked
      if(remaining_cross_axis_length > -delta_x)
      {
        if(!widget->flexbox_data.container.is_fluid)
        {
          delta_x = 0;
        }
      }
      else
      {
        delta_x += remaining_cross_axis_length;
      }
    }
    else if(remaining_cross_axis_length < 0)
    {
      // cross axis should be increased
      if(remaining_cross_axis_length > delta_x)
      {
        if(!widget->flexbox_data.container.is_fluid)
        {
          delta_x = 0;
        }
      }
      else
      {
        delta_x -= remaining_cross_axis_length;
      }
    }
  }

  if(delta_x == 0 && delta_y == 0)
  {
    return ok(result_base_widget_ptr, widget);
  }

  if(widget->parent)
  {
    return widget->parent->internal_mark_need_resizing(
      widget->parent, delta_x, delta_y);
  }

  return ok(result_base_widget_ptr, widget);
}

result_void default_internal_calculate_size_callback(base_widget* widget)
{
  if(widget->type == FLEX_ITEM)
  {
    if(widget->need_resizing)
    {
      if(widget->internal_fit_layout_callback)
      {
        widget->internal_fit_layout_callback(widget, false);
      }
      widget->need_resizing = false;
    }
    return ok_void();
  }

  if(!widget->flexbox_data.container.is_fluid)
  {
    // skip this widget but call calculate sizing on children widgets
    base_widget_child_node* node = widget->children_head;
    while(node)
    {
      result_void _ = node->child->internal_calculate_size(node->child);
      if(!_.ok)
      {
        return _;
      }
      node = node->next;
    }

    return ok_void();
  }

  uint16 main_axis_length = 0, cross_axis_length = 0;
  base_widget_child_node* node = widget->children_head;
  while(node)
  {
    if(node->child->need_resizing)
    {
      node->child->internal_calculate_size(node->child);
    }
    if(widget->flexbox_data.container.direction == FLEX_DIRECTION_ROW)
    {
      main_axis_length += node->child->w;
      cross_axis_length = max(cross_axis_length, node->child->h);
    }
    else
    {
      main_axis_length += node->child->h;
      cross_axis_length = max(cross_axis_length, node->child->w);
    }
    node = node->next;
  }

  // assigning calculated axes lengths to conatiner widget's dimensions.
  if(widget->flexbox_data.container.direction == FLEX_DIRECTION_ROW)
  {
    widget->w = main_axis_length;
    widget->h = cross_axis_length;
  }
  else
  {
    widget->w = cross_axis_length;
    widget->h = main_axis_length;
  }

  return ok_void();
}

result_void default_internal_relayout_callback(const base_widget* widget)
{
  if(widget->type == FLEX_ITEM)
  {
    return error(
      result_void,
      "Should not call internal relayout callback on FLEX_ITEM widget!");
  }

  uint16 main_axis_length =
    widget->flexbox_data.container.direction == FLEX_DIRECTION_ROW ? widget->w
                                                                   : widget->h;
  uint16 cross_axis_length =
    widget->flexbox_data.container.direction == FLEX_DIRECTION_ROW ? widget->h
                                                                   : widget->w;

  uint16 needed_main_axis_length = 0, needed_cross_axis_length = 0;
  uint16 total_flex_grow = 0, total_flex_shrink = 0;

  base_widget_child_node* node = widget->children_head;
  while(node)
  {
    if(widget->flexbox_data.container.direction == FLEX_DIRECTION_ROW)
    {
      needed_main_axis_length += node->child->w;
      needed_cross_axis_length = max(needed_cross_axis_length, node->child->h);
    }
    else
    {
      needed_main_axis_length += node->child->h;
      needed_cross_axis_length = max(needed_cross_axis_length, node->child->w);
    }
    total_flex_grow += node->child->type == FLEX_CONTAINER
                         ? node->child->flexbox_data.container.flex_grow
                         : node->child->flexbox_data.item.flex_grow;
    total_flex_shrink += node->child->type == FLEX_CONTAINER
                           ? node->child->flexbox_data.container.flex_shrink
                           : node->child->flexbox_data.item.flex_shrink;
    node = node->next;
  }

  printf("Needed main, cross axes lengths: %d, %d\n",
         needed_main_axis_length,
         needed_cross_axis_length);

  // adjusting sizing in main axis
  int16 remaining_main_axis_length = main_axis_length - needed_main_axis_length;
  if(remaining_main_axis_length > 0 && total_flex_grow > 0)
  {
    // share remaining space according to flex-grow of each child.
    node = widget->children_head;
    while(node)
    {
      if(widget->flexbox_data.container.direction == FLEX_DIRECTION_ROW)
      {
        node->child->w +=
          (uint16)(((node->child->type == FLEX_CONTAINER
                       ? node->child->flexbox_data.container.flex_grow
                       : node->child->flexbox_data.item.flex_grow) /
                    total_flex_grow) *
                   remaining_main_axis_length);
      }
      else
      {
        node->child->h += ((node->child->type == FLEX_CONTAINER
                              ? node->child->flexbox_data.container.flex_grow
                              : node->child->flexbox_data.item.flex_grow) /
                           total_flex_grow) *
                          remaining_main_axis_length;
      }
      node = node->next;
    }
  }
  else if(remaining_main_axis_length < 0 && total_flex_shrink > 0)
  {
    // acquire needed space by shrinking children according
    // to their flex-shrink
    node = widget->children_head;
    while(node)
    {
      if(widget->flexbox_data.container.direction == FLEX_DIRECTION_ROW)
      {
        node->child->w += ((node->child->type == FLEX_CONTAINER
                              ? node->child->flexbox_data.container.flex_shrink
                              : node->child->flexbox_data.item.flex_shrink) /
                           total_flex_shrink) *
                          remaining_main_axis_length;
      }
      else
      {
        node->child->h += ((node->child->type == FLEX_CONTAINER
                              ? node->child->flexbox_data.container.flex_shrink
                              : node->child->flexbox_data.item.flex_shrink) /
                           total_flex_shrink) *
                          remaining_main_axis_length;
      }
      node = node->next;
    }
  }

  // adjusting sizing in cross axis
  if(needed_cross_axis_length != cross_axis_length)
  {
    cross_axis_length = needed_cross_axis_length;
    node = widget->children_head;
    while(node)
    {
      if(widget->flexbox_data.container.direction == FLEX_DIRECTION_ROW)
      {
        node->child->h = cross_axis_length;
      }
      else
      {
        node->child->w = cross_axis_length;
      }
      node = node->next;
    }
  }

  // assigning positions
  int16 x = widget->x, y = widget->y;
  node = widget->children_head;
  while(node)
  {
    printf("assigning positions x, y: %d, %d\n", x, y);
    node->child->x = x;
    node->child->y = y;
    if(widget->flexbox_data.container.direction == FLEX_DIRECTION_ROW)
    {
      x += node->child->w;
    }
    else
    {
      y += node->child->h;
    }
    node = node->next;
  }

  // calling re-layout on children which are container widgets
  node = widget->children_head;
  while(node)
  {
    if(node->child->type == FLEX_CONTAINER)
    {
      node->child->internal_relayout(node->child);
    }
    node = node->next;
  }

  return ok_void();
}

rect default_internal_get_bounding_rect_callback(const base_widget* widget)
{
  return (rect){.x = widget->x, .y = widget->y, .w = widget->w, .h = widget->h};
}

static result_bool default_internal_adjust_layout_callback(base_widget* widget)
{
  if(!widget->internal_fit_layout_callback)
  {
    return ok(result_bool, false);
  }

  // call fit layout on this widget, that should return the delta_x, delta_y
  // using these deltas, mark need resizing from this widget, that should
  // return the lowest ancestor which satisfies these deltas
  // now call calculate sizing on this ancestor
  // then call relayout on this ancestor
  // then call internal render on this ancestor.
  result_sizing_delta _ = widget->internal_fit_layout_callback(widget, false);
  if(!_.ok)
  {
    return error(result_bool, _.error);
  }

  if(_.value.x == 0 && _.value.y == 0)
  {
    // deltas are 0
    return ok(result_bool, true);
  }

  result_base_widget_ptr __ =
    widget->internal_mark_need_resizing(widget, _.value.x, _.value.y);
  if(!__.ok)
  {
    return error(result_bool, __.error);
  }

  base_widget* ancestor = __.value;
  printf("Ancestor parent: %s\n", ancestor->parent ? "EXISTS" : "(NULL)");
  ancestor->internal_calculate_size(ancestor);
  ancestor->internal_relayout(ancestor);
  ancestor->internal_render_callback(ancestor);

  return ok(result_bool, true);
}

static void default_internal_free_callback(base_widget* widget)
{
  if(!widget)
  {
    return;
  }

  // freeing children widgets (not children nodes, node will be free later
  // in base_widget_free)
  if(widget->children_head)
  {
    base_widget_child_node* node = widget->children_head;
    while(node)
    {
      node->child->internal_free_callback(node->child);
      node = node->next;
    }
  }

  // freeing derived widget fields
  if(widget->derived && widget->internal_derived_free_callback)
  {
    widget->internal_derived_free_callback(widget);
  }

  base_widget_free(widget);
}

result_bool default_internal_mouse_motion_callback(
  base_widget* widget, internal_mouse_motion_event* internal_event)
{
  if(!widget)
  {
    return error(result_bool,
                 "Cannot process internal mouse motion callback on NULL "
                 "pointed base widget!");
  }

  if(internal_event->state == BUBBLING_UP)
  {
    // call callbacks if present
    // setting context's mouse focused widget has already been done by
    // some child widget down somewhere and set state to BUBBLING_UP.
    if(widget->mouse_enter_callback)
    {
      widget->mouse_enter_callback(widget, internal_event->event);
    }

    // return if parent doesn't exist (root element)
    if(!widget->parent)
    {
      return ok(result_bool, true);
    }

    // still bubble up if parent exists
    return widget->parent->internal_mouse_motion_callback(widget->parent,
                                                          internal_event);
  }

  // this widget is target
  // setting context's mouse focused widget and calling mouse enter callback
  if(widget->context->mouse_focused_widget != widget)
  {
    // should call mouse leave on previously mouse focused widget
    if(widget->context->mouse_focused_widget &&
       widget->context->mouse_focused_widget->mouse_leave_callback)
    {
      widget->context->mouse_focused_widget->mouse_leave_callback(
        widget->context->mouse_focused_widget, internal_event->event);
    }

    widget->context->mouse_focused_widget = widget;
    // should call mouse enter on this widget
    if(widget->mouse_enter_callback)
    {
      widget->mouse_enter_callback(widget, internal_event->event);
    }
  }

  if(widget->context->active_scrollbar &&
     widget->context->active_scrollbar == widget)
  {
    // this widget is the scrollbar widget
    // consuming mouse motion events
    // no use of bubbling up these events
    return ok(result_bool, true);
  }

  if(widget->context->overlay_widget &&
     widget->context->overlay_widget == widget)
  {
    // this widget is the overlay widget
    // no use of bubbling up these events
    return ok(result_bool, true);
  }

  if(!widget->parent)
  {
    return ok(result_bool, true);
  }

  // bubbling up
  internal_event->state = BUBBLING_UP;

  return widget->parent->internal_mouse_motion_callback(widget->parent,
                                                        internal_event);
}

result_bool default_internal_mouse_button_callback(
  base_widget* widget, internal_mouse_button_event* internal_event)
{
  if(!widget)
  {
    return error(result_bool,
                 "Cannot process internal mouse button callback, with widget "
                 "pointing to NULL!");
  }

  if(internal_event->state == BUBBLING_UP)
  {
    // call callbacks if present
    if(internal_event->event.button_state == MOUSE_BUTTON_DOWN)
    {
      if(widget->mouse_button_down_callback)
      {
        widget->mouse_button_down_callback(widget, internal_event->event);
      }
    }
    else
    {
      if(widget->mouse_button_up_callback)
      {
        widget->mouse_button_up_callback(widget, internal_event->event);
      }
    }

    // return if parent doesn't exist (root element)
    if(!widget->parent)
    {
      return ok(result_bool, true);
    }

    // still bubble up if parent exists
    return widget->parent->internal_mouse_button_callback(widget->parent,
                                                          internal_event);
  }

  // this widget is target
  if(internal_event->event.button_state == MOUSE_BUTTON_DOWN)
  {
    if(widget->mouse_button_down_callback)
    {
      widget->mouse_button_down_callback(widget, internal_event->event);
    }
  }
  else
  {
    if(widget->mouse_button_up_callback)
    {
      widget->mouse_button_up_callback(widget, internal_event->event);
    }
  }

  if(widget->context->active_scrollbar &&
     widget->context->active_scrollbar == widget)
  {
    // this widget is the scrollbar widget
    // consuming mouse button events
    // no use of bubbling up these events
    return ok(result_bool, true);
  }

  if(widget->context->overlay_widget &&
     widget->context->overlay_widget == widget)
  {
    // this widget is the overlay widget
    // no use of bubbling up these events
    return ok(result_bool, true);
  }

  if(!widget->parent)
  {
    return ok(result_bool, true);
  }

  // bubbling up
  internal_event->state = BUBBLING_UP;

  return widget->parent->internal_mouse_button_callback(widget->parent,
                                                        internal_event);
}

result_bool default_internal_mouse_scroll_callback(
  base_widget* widget, internal_mouse_scroll_event* internal_event)
{
  if(!widget)
  {
    return error(result_bool,
                 "Cannot process internal mouse scroll callback, with widget "
                 "pointing to NULL!");
  }

  // mouse scroll event has no bubbling phase
  // so direclty call the callback if present on this widget

  if(widget->mouse_scroll_callback)
  {
    return ok(result_bool,
              widget->mouse_scroll_callback(widget, internal_event->event));
  }

  return ok(result_bool, false);
}
