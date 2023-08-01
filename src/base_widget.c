#include "../include/base_widget.h"
#include <stdlib.h>
#include "../include/macros.h"

static rect default_internal_get_bounding_rect_callback(base_widget* widget);
static result_bool default_internal_mouse_motion_callback(
  base_widget* widget, internal_mouse_motion_event* internal_event);
static result_bool default_internal_mouse_button_callback(
  base_widget* widget, internal_mouse_button_event* internal_event);
static result_bool default_internal_mouse_scroll_callback(
  base_widget* widget, internal_mouse_scroll_event* internal_event);

result_base_widget_child_node_ptr base_widget_child_node_new(base_widget* child)
{
  if(!child)
  {
    return (result_base_widget_child_node_ptr){
      .ok = false,
      .error = "Cannot create new child node for NULL pointed child!"};
  }

  base_widget_child_node* node =
    (base_widget_child_node*)calloc(1, sizeof(base_widget_child_node));
  if(!node)
  {
    return (result_base_widget_child_node_ptr){
      .ok = false, .error = "Unable to allocate memory for new child node!"};
  }

  node->child = child;
  node->next = NULL;

  return (result_base_widget_child_node_ptr){.ok = true, .value = node};
}

result_void base_widget_child_node_free(base_widget_child_node* node)
{
  if(!node)
  {
    return (result_void){.ok = false,
                         .error = "Attempt to free a NULL pointed child node!"};
  }

  free(node);

  return (result_void){.ok = true, .error = NULL};
}

result_base_widget_ptr base_widget_new()
{
  base_widget* widget = (base_widget*)calloc(1, sizeof(base_widget));
  if(!widget)
  {
    return (result_base_widget_ptr){
      .ok = false, .error = "Unable to allocate memory for base_widget!"};
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

  return (result_base_widget_ptr){.ok = true, .value = widget};
}

result_void base_widget_add_child(base_widget* base, base_widget* child)
{
  if(!base)
  {
    return (result_void){
      .ok = false,
      .error = "Cannot attach child to NULL pointed parent widget!"};
  }

  if(!child)
  {
    return (result_void){
      .ok = false,
      .error = "Cannot attach NULL pointed child to parent widget!"};
  }

  child->context = base->context;

  if(!base->children_head)
  {
    result_base_widget_child_node_ptr _ = base_widget_child_node_new(child);
    if(!_.ok)
    {
      return (result_void){.ok = false, .error = _.error};
    }

    child->parent = base;
    base->children_head = _.value;

    return (result_void){.ok = true, .error = NULL};
  }

  base_widget_child_node* temp = base->children_head;
  while(temp->next)
  {
    temp = temp->next;
  }
  result_base_widget_child_node_ptr _ = base_widget_child_node_new(child);
  if(!_.ok)
  {
    return (result_void){.ok = false, .error = _.error};
  }

  child->parent = base;
  temp->next = _.value;

  return (result_void){.ok = true, .error = NULL};
}

result_void base_widget_remove_child(base_widget* base, base_widget* child)
{
  if(!base)
  {
    return (result_void){
      .ok = false,
      .error = "Cannot remove child from NULL pointed parent widget!"};
  }

  if(!child)
  {
    return (result_void){
      .ok = false,
      .error = "Cannot remove NULL pointed child from parent widget!"};
  }

  if(!base->children_head)
  {
    return (result_void){
      .ok = false,
      .error = "Cannot remove child from empty children list of parent!"};
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
    return (result_void){
      .ok = false, .error = "Child node not found with given child to remove!"};
  }

  prev_temp->next = temp->next;
  temp->child->parent = NULL;
  base_widget_child_node_free(temp);

  return (result_void){.ok = true, .error = NULL};
}

result_void base_widget_free(base_widget* widget)
{
  if(!widget)
  {
    return (result_void){
      .ok = false, .error = "Attempt to free a NULL pointed base widget!"};
  }

  free(widget);

  return (result_void){.ok = true, .error = NULL};
}

rect default_internal_get_bounding_rect_callback(base_widget* widget)
{
  return (rect){.x = widget->x, .y = widget->y, .w = widget->w, .h = widget->h};
}

result_bool widget_encloses_point(base_widget* widget, uint16 x, uint16 y)
{
  if(!widget)
  {
    return (result_bool){.ok = false,
                         .error = "Cannot check if widget encloses point, for "
                                  "a NULL pointing widget!"};
  }

  rect bounding_rect = widget->internal_get_bounding_rect_callback(widget);

  return (result_bool){
    .ok = true,
    .value = (bounding_rect.x <= x && x <= bounding_rect.x + bounding_rect.w &&
              bounding_rect.y <= y && y <= bounding_rect.y + bounding_rect.h)};
}

result_bool widget_has_valid_mouse_motion_callbacks(base_widget* widget)
{
  if(!widget)
  {
    return (result_bool){
      .ok = false,
      .error = "Cannot check if widget has valid mouse motion callbacks "
               "(mouse enter or leave), with widget pointing to NULL!"};
  }

  return (result_bool){.ok = true,
                       .value = widget->mouse_enter_callback ||
                                widget->mouse_leave_callback};
}

result_bool widget_has_valid_mouse_button_callbacks(base_widget* widget)
{
  if(!widget)
  {
    return (result_bool){
      .ok = false,
      .error = "Cannot check if widget has valid mouse button callbacks "
               "(mouse button down or up), with widget pointing to NULL!"};
  }

  return (result_bool){.ok = true,
                       .value = widget->mouse_button_up_callback ||
                                widget->mouse_button_up_callback};
}

result_base_widget_ptr
get_deepest_child_with_point(base_widget* widget, uint16 x, uint16 y)
{
  if(!widget)
  {
    return (result_base_widget_ptr){
      .ok = false,
      .error = "Cannot get deepest child with given point, in a NULL "
               "pointing base widget!"};
  }

  result_bool _ = widget_encloses_point(widget, x, y);
  if(!_.ok)
  {
    return (result_base_widget_ptr){.ok = false, .error = _.error};
  }

  if(!_.value)
  {
    // widget doesn't contain point
    return (result_base_widget_ptr){
      .ok = false,
      .error = "Widget doesn't contain point, cannot get deepest child!"};
  }

  if(!widget->children_head)
  {
    // as there are no children, we return this widget as the deepest
    return (result_base_widget_ptr){.ok = true, .value = widget};
  }

  base_widget_child_node* node = widget->children_head;
  while(node)
  {
    result_bool _ = widget_encloses_point(node->child, x, y);
    if(!_.ok)
    {
      return (result_base_widget_ptr){.ok = false, .error = _.error};
    }
    if(_.value)
    {
      result_base_widget_ptr __ =
        get_deepest_child_with_point(node->child, x, y);
      if(!__.ok)
      {
        return __;
      }
      return (result_base_widget_ptr){.ok = true, .value = __.value};
    }
    node = node->next;
  }

  // none of the children contains the point
  // so returning this widget as the deepest widget
  return (result_base_widget_ptr){.ok = true, .value = widget};
}

result_base_widget_ptr
get_nearest_parent_with_valid_mouse_motion_callbacks(base_widget* widget)
{
  if(!widget)
  {
    return (result_base_widget_ptr){
      .ok = false,
      .error = "Cannot get nearest parent with valid mouse motion callbacks, "
               "with widget pointing to NULL!"};
  }

  if(!widget->parent)
  {
    // recursive calls have reached root
    return (result_base_widget_ptr){.ok = true, .value = NULL};
  }

  result_bool _ = widget_has_valid_mouse_motion_callbacks(widget->parent);
  if(!_.ok)
  {
    return (result_base_widget_ptr){.ok = false, .error = _.error};
  }

  if(_.value)
  {
    return (result_base_widget_ptr){.ok = true, .value = widget->parent};
  }

  result_base_widget_ptr __ =
    get_nearest_parent_with_valid_mouse_motion_callbacks(widget->parent);
  if(!__.ok)
  {
    return __;
  }

  return (result_base_widget_ptr){.ok = true, .value = __.value};
}

// result_base_widget_ptr
// get_nearest_parent_with_valid_event_callback(base_widget* widget,
//                                              const event_callback_type type)
// {
//   if(!widget)
//   {
//     return (result_base_widget_ptr){
//       .ok = false,
//       .error = "Cannot get nearest parent with valid event callback, with "
//                "widget pointing to NULL!"};
//   }

//   if(type < EVENT_CALLBACK_TYPE_MOUSE_ENTER ||
//      type > EVENT_CALLBACK_TYPE_MOUSE_SCROLL)
//   {
//     return (result_base_widget_ptr){
//       .ok = false,
//       .error = "Cannot get nearest parent with valid event callback, with "
//                "invalid event callback type!"};
//   }

//   if(!widget->parent)
//   {
//     return (result_base_widget_ptr){.ok = true, .value = NULL};
//   }

//   switch(type)
//   {
//   case EVENT_CALLBACK_TYPE_MOUSE_ENTER: {
//     if(widget->parent->mouse_enter_callback)
//     {
//       return (result_base_widget_ptr){.ok = true, .value = widget->parent};
//     }
//   }
//   case EVENT_CALLBACK_TYPE_MOUSE_LEAVE: {
//     if(widget->parent->mouse_leave_callback)
//     {
//       return (result_base_widget_ptr){.ok = true, .value = widget->parent};
//     }
//   }
//   case EVENT_CALLBACK_TYPE_MOUSE_BUTTON_DOWN: {
//     if(widget->parent->mouse_button_down_callback)
//     {
//       return (result_base_widget_ptr){.ok = true, .value = widget->parent};
//     }
//   }
//   case EVENT_CALLBACK_TYPE_MOUSE_BUTTON_UP: {
//     if(widget->parent->mouse_button_up_callback)
//     {
//       return (result_base_widget_ptr){.ok = true, .value = widget->parent};
//     }
//   }
//   case EVENT_CALLBACK_TYPE_MOUSE_SCROLL: {
//     if(widget->parent->mouse_scroll_callback)
//     {
//       return (result_base_widget_ptr){.ok = true, .value = widget->parent};
//     }
//   }
//   default:
//     break;
//   }

//   result_base_widget_ptr __ =
//     get_nearest_parent_with_valid_event_callback(widget->parent, type);
//   if(!__.ok)
//   {
//     return __;
//   }

//   return (result_base_widget_ptr){.ok = true, .value = __.value};
// }

result_bool default_internal_mouse_motion_callback(
  base_widget* widget, internal_mouse_motion_event* internal_event)
{
  if(!widget)
  {
    return (result_bool){.ok = false,
                         .error = "Cannot process internal mouse motion "
                                  "callback on NULL pointed base widget!"};
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
    return (result_bool){.ok = true,
                         .error = "Cannot process internal mouse button "
                                  "callback, with widget pointing to NULL!"};
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
    return (result_bool){.ok = true,
                         .error = "Cannot process internal mouse scroll "
                                  "callback, with widget pointing to NULL!"};
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