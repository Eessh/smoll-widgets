#include "../include/base_widget.h"
#include <stdlib.h>
#include "../include/macros.h"

/// @brief Default callback for internal bounding rect callback.
/// @param widget pointer to base widget.
/// @return Returns rect.
static rect default_internal_get_bounding_rect_callback(base_widget* widget);

/// @brief Default callback for internal mouse motion callback.
/// @param widget pointer to base widget.
/// @param internal_event pointer to internal mouse motion event.
/// @return Bool result.
static result_bool default_internal_mouse_motion_callback(
  base_widget* widget, internal_mouse_motion_event* internal_event);

/// @brief Default callback for internal mouse button callback.
/// @param widget pointer to base widget.
/// @param internal_event pointer to internal mouse button event.
/// @return Bool result.
static result_bool default_internal_mouse_button_callback(
  base_widget* widget, internal_mouse_button_event* internal_event);

/// @brief Default callback for internal mouse scroll callback.
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

result_base_widget_ptr base_widget_new()
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

  widget->visible = true;

  widget->foreground = (color){255, 255, 255, 255};
  widget->background = (color){0, 0, 0, 255};

  widget->derived = NULL;
  widget->parent = NULL;
  widget->children_head = NULL;

  widget->context = NULL;

  widget->internal_get_bounding_rect_callback =
    default_internal_get_bounding_rect_callback;
  widget->internal_fit_layout_callback = NULL;
  widget->internal_assign_positions = NULL;
  widget->internal_render_callback = NULL;

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

rect default_internal_get_bounding_rect_callback(base_widget* widget)
{
  return (rect){.x = widget->x, .y = widget->y, .w = widget->w, .h = widget->h};
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
    if(widget->context->mouse_focused_widget->mouse_leave_callback)
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

  // bubbling up
  internal_event->state = BUBBLING_UP;

  return widget->parent->internal_mouse_motion_callback(widget->parent,
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