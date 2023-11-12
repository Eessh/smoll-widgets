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
// static result_base_widget_ptr default_internal_mark_need_resizing(
//   base_widget* widget, int16 delta_x, int16 delta_y);

/**
 * @brief      Default callback function for internal calculate size
 *             callback.
 *
 * @param      widget  pointer to widget.
 *
 * @return     Void result.
 */
// static result_void
// default_internal_calculate_size_callback(base_widget* widget);

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
// static rect
// default_internal_get_bounding_rect_callback(const base_widget* widget);

static result_bool default_internal_adjust_layout_callback(base_widget* widget);

/// @brief Default callback function for internal free callback.
/// @param widget pointer to base widget.
/// @return Void.
// static void default_internal_free_callback(base_widget* widget);

/// @brief Default callback function for internal mouse motion callback.
/// @param widget pointer to base widget.
/// @param internal_event pointer to internal mouse motion event.
/// @return Bool result.
// static result_bool default_internal_mouse_motion_callback(
//   base_widget* widget, internal_mouse_motion_event* internal_event);

/// @brief Default callback function for internal mouse button callback.
/// @param widget pointer to base widget.
/// @param internal_event pointer to internal mouse button event.
/// @return Bool result.
// static result_bool default_internal_mouse_button_callback(
//   base_widget* widget, internal_mouse_button_event* internal_event);

/// @brief Default callback function for internal mouse scroll callback.
/// @param widget pointer to base widget.
/// @param internal_event pointer to internal mouse scroll event.
/// @return Bool result.
// static result_bool default_internal_mouse_scroll_callback(
//   base_widget* widget, internal_mouse_scroll_event* internal_event);

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
                            .justify_content = JUSTIFY_CONTENT_START,
                            .align_items = ALIGN_ITEMS_START,
                            .flex_grow = 0,
                            .flex_shrink = 0,
                            .gap = 0,
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

  // widget->internal_mark_need_resizing = default_internal_mark_need_resizing;
  // widget->internal_calculate_size = default_internal_calculate_size_callback;
  widget->internal_relayout = default_internal_relayout_callback;
  widget->pre_internal_relayout_hook = NULL;
  // widget->internal_get_bounding_rect_callback =
  //   default_internal_get_bounding_rect_callback;
  widget->internal_get_background_callback = NULL;
  widget->internal_fit_layout_callback = NULL;
  widget->internal_adjust_layout_callback =
    default_internal_adjust_layout_callback;
  widget->internal_assign_positions = NULL;
  widget->internal_render_callback = NULL;

  // widget->internal_free_callback = default_internal_free_callback;
  widget->internal_derived_free_callback = NULL;

  // widget->internal_mouse_motion_callback =
  //   default_internal_mouse_motion_callback;
  // widget->internal_mouse_button_callback =
  //   default_internal_mouse_button_callback;
  // widget->internal_mouse_scroll_callback =
  //   default_internal_mouse_scroll_callback;

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

result_void base_widget_add_child_after(base_widget* base,
                                        base_widget* child,
                                        base_widget* after_this_widget)
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

  if(!after_this_widget)
  {
    return error(
      result_void,
      "Cannot attach child after a widget, which is pointing to NULL!");
  }

  base_widget_child_node* temp = base->children_head;
  while(temp && temp->child != after_this_widget)
  {
    temp = temp->next;
  }
  if(!temp)
  {
    return error(result_void,
                 "Cannot find the 'after_this_widget' in children of the given "
                 "base widget!");
  }

  result_base_widget_child_node_ptr _ = base_widget_child_node_new(child);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  _.value->next = temp->next;
  temp->next = _.value;
  child->context = base->context;
  child->parent = base;

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
  while(temp && temp->child != child)
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

  if(!prev_temp)
  {
    // first child is to be deleted
    base_widget_child_node* _ = base->children_head;
    base->children_head = _->next;
    base_widget_child_node_free(_);
    return ok_void();
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

result_bool widget_set_visibility(base_widget* widget, bool visible)
{
  if(!widget)
  {
    return error(result_bool, "Cannot set visibility of NULL pointed widget!");
  }

  widget->visible = visible;

  // calling internal adjust layout callback
  // on parent widget as visibility of one of its child has changed
  if(!widget->parent)
  {
    /// TODO: Handle case when root node is set invisible. What to do?
    return ok(result_bool, true);
  }

  return widget->parent->internal_adjust_layout_callback(widget->parent);
}

result_bool widget_set_flex_direction(base_widget* widget,
                                      flex_direction direction)
{
  if(!widget)
  {
    return error(result_bool,
                 "Cannot set flex-drection of NULL pointed widget!");
  }

  if(widget->type != FLEX_CONTAINER)
  {
    return ok(result_bool, false);
  }

  widget->flexbox_data.container.direction = direction;

  // triggering re-adjust layout on this widget
  widget->internal_adjust_layout_callback(widget);

  return ok(result_bool, true);
}

result_bool widget_set_justify_content(base_widget* widget,
                                       flex_justify_content justify_content)
{
  if(!widget)
  {
    return error(result_bool,
                 "Cannot set justify-container of NULL pointed widget!");
  }

  if(widget->type != FLEX_CONTAINER)
  {
    return ok(result_bool, false);
  }

  widget->flexbox_data.container.justify_content = justify_content;

  // triggering re-adjust layout on this widget
  widget->internal_adjust_layout_callback(widget);

  return ok(result_bool, true);
}

result_bool widget_set_align_items(base_widget* widget,
                                   flex_align_items align_items)
{
  if(!widget)
  {
    return error(result_bool, "Cannot set align-items of NULL pointed widget!");
  }

  if(widget->type != FLEX_CONTAINER)
  {
    return ok(result_bool, false);
  }

  widget->flexbox_data.container.align_items = align_items;

  // triggering re-adjust layout on this widget
  widget->internal_adjust_layout_callback(widget);

  return ok(result_bool, true);
}

result_bool widget_set_gap(base_widget* widget, uint8 gap)
{
  if(!widget)
  {
    return error(result_bool, "Cannot set gap of NULL pointed widget!");
  }

  if(widget->type != FLEX_CONTAINER)
  {
    return ok(result_bool, false);
  }

  widget->flexbox_data.container.gap = gap;

  // triggering re-adjust layout on this widget
  widget->internal_adjust_layout_callback(widget);

  return ok(result_bool, true);
}

result_void widget_set_flex_grow(base_widget* widget, uint8 flex_grow)
{
  if(!widget)
  {
    return error(result_void, "Cannot set flex-grow of NULL pointed widget!");
  }

  if(widget->type == FLEX_CONTAINER)
  {
    widget->flexbox_data.container.flex_grow = flex_grow;

    // triggering re-adjust layout on this widget
    widget->internal_adjust_layout_callback(widget);
  }
  else
  {
    widget->flexbox_data.item.flex_grow = flex_grow;
  }

  return ok_void();
}

result_void widget_set_flex_shrink(base_widget* widget, uint8 flex_shrink)
{
  if(!widget)
  {
    return error(result_void, "Cannot set flex-shrink of NULL pointed widget!");
  }

  if(widget->type == FLEX_CONTAINER)
  {
    widget->flexbox_data.container.flex_shrink = flex_shrink;

    // triggering re-adjust layout on this widget
    widget->internal_adjust_layout_callback(widget);
  }
  else
  {
    widget->flexbox_data.item.flex_shrink = flex_shrink;
  }

  return ok_void();
}

result_void
widget_set_cross_axis_sizing(base_widget* widget,
                             flex_cross_axis_sizing cross_axis_sizing)
{
  if(!widget)
  {
    return error(result_void,
                 "Cannot set cross-axis-sizing of NULL pointed widget!");
  }

  if(widget->type == FLEX_CONTAINER)
  {
    widget->flexbox_data.container.cross_axis_sizing = cross_axis_sizing;

    // triggering re-adjust layout on this widget
    widget->internal_adjust_layout_callback(widget);
  }
  else
  {
    widget->flexbox_data.item.cross_axis_sizing = cross_axis_sizing;
  }

  return ok_void();
}

// static result_base_widget_ptr default_internal_mark_need_resizing(
//   base_widget* widget, int16 delta_x, int16 delta_y)
// {
//   if(widget->type == FLEX_ITEM)
//   {
//     widget->need_resizing = false;
//     if(widget->parent)
//     {
//       return widget->parent->internal_mark_need_resizing(
//         widget->parent, delta_x, delta_y);
//     }
//     return ok(result_base_widget_ptr, widget);
//   }

//   if(!widget->flexbox_data.container.is_fluid)
//   {
//     // widget is not fluid, doesn't resize
//     debug("Encountered non-fluid widget.");
//     return ok(result_base_widget_ptr, widget);
//   }

//   if(widget->flexbox_data.container.direction == FLEX_DIRECTION_ROW)
//   {
//     // consume delta_x from main axis, delta_y from cross axis
//     uint16 main_axis_length = widget->w, cross_axis_length = widget->h;
//     uint16 needed_main_axis_length = 0, needed_cross_axis_length = 0;
//     base_widget_child_node* node = widget->children_head;
//     while(node)
//     {
//       needed_main_axis_length +=
//         node->child->w + widget->flexbox_data.container.gap;
//       needed_cross_axis_length = max(needed_cross_axis_length, node->child->h);
//       node = node->next;
//     }
//     needed_main_axis_length -= widget->flexbox_data.container.gap;
//     int16 remaining_main_axis_length =
//       main_axis_length - needed_main_axis_length;
//     if(remaining_main_axis_length > 0)
//     {
//       // main axis size needs to be decreased
//       if(remaining_main_axis_length >= -delta_x)
//       {
//         if(!widget->flexbox_data.container.is_fluid)
//         {
//           delta_x = 0;
//         }
//       }
//       else
//       {
//         delta_x += remaining_main_axis_length;
//       }
//     }
//     else if(remaining_main_axis_length < 0)
//     {
//       // main axis size needs to be increased
//       if(remaining_main_axis_length >= delta_x)
//       {
//         delta_x = 0;
//       }
//       else
//       {
//         delta_x -= remaining_main_axis_length;
//       }
//     }
//     int16 remaining_cross_axis_length =
//       cross_axis_length - needed_cross_axis_length;
//     if(remaining_cross_axis_length > 0)
//     {
//       // main axis size needs to be decreased
//       if(remaining_cross_axis_length >= -delta_y)
//       {
//         if(!widget->flexbox_data.container.is_fluid)
//         {
//           delta_y = 0;
//         }
//       }
//       else
//       {
//         delta_y += remaining_cross_axis_length;
//       }
//     }
//     else if(remaining_cross_axis_length < 0)
//     {
//       // main axis size needs to be increased
//       if(remaining_cross_axis_length >= delta_y)
//       {
//         delta_y = 0;
//       }
//       else
//       {
//         delta_y -= remaining_cross_axis_length;
//       }
//     }
//   }
//   else
//   {
//     // consume delta_y from main axis, delta_x from cross axis
//     uint16 main_axis_length = widget->h, cross_axis_length = widget->w;
//     uint16 needed_main_axis_length = 0, needed_cross_axis_length = 0;
//     base_widget_child_node* node = widget->children_head;
//     while(node)
//     {
//       needed_main_axis_length +=
//         node->child->h + widget->flexbox_data.container.gap;
//       needed_cross_axis_length = max(needed_cross_axis_length, node->child->w);
//       node = node->next;
//     }
//     needed_main_axis_length -= widget->flexbox_data.container.gap;
//     int16 remaining_main_axis_length =
//       main_axis_length - needed_main_axis_length;
//     if(remaining_main_axis_length > 0)
//     {
//       // main axis should be shrinked
//       if(remaining_main_axis_length > -delta_y)
//       {
//         if(!widget->flexbox_data.container.is_fluid)
//         {
//           delta_y = 0;
//         }
//       }
//       else
//       {
//         delta_y += remaining_main_axis_length;
//       }
//     }
//     else if(remaining_main_axis_length < 0)
//     {
//       // main axis should be increased
//       if(remaining_main_axis_length > delta_y)
//       {
//         delta_y = 0;
//       }
//       else
//       {
//         delta_y -= remaining_main_axis_length;
//       }
//     }
//     int16 remaining_cross_axis_length =
//       cross_axis_length - needed_cross_axis_length;
//     if(remaining_cross_axis_length > 0)
//     {
//       // cross axis should be shrinked
//       if(remaining_cross_axis_length > -delta_x)
//       {
//         if(!widget->flexbox_data.container.is_fluid)
//         {
//           delta_x = 0;
//         }
//       }
//       else
//       {
//         delta_x += remaining_cross_axis_length;
//       }
//     }
//     else if(remaining_cross_axis_length < 0)
//     {
//       // cross axis should be increased
//       if(remaining_cross_axis_length > delta_x)
//       {
//         if(!widget->flexbox_data.container.is_fluid)
//         {
//           delta_x = 0;
//         }
//       }
//       else
//       {
//         delta_x -= remaining_cross_axis_length;
//       }
//     }
//   }

//   if(delta_x == 0 && delta_y == 0)
//   {
//     return ok(result_base_widget_ptr, widget);
//   }

//   if(widget->parent)
//   {
//     return widget->parent->internal_mark_need_resizing(
//       widget->parent, delta_x, delta_y);
//   }

//   return ok(result_base_widget_ptr, widget);
// }

// result_void default_internal_calculate_size_callback(base_widget* widget)
// {
//   if(!widget->visible)
//   {
//     // avoiding calculations when widget is not visible
//     return ok_void();
//   }

//   if(widget->type == FLEX_ITEM)
//   {
//     if(widget->need_resizing)
//     {
//       if(widget->internal_fit_layout_callback)
//       {
//         widget->internal_fit_layout_callback(widget, false);
//       }
//       widget->need_resizing = false;
//     }
//     return ok_void();
//   }

//   if(!widget->flexbox_data.container.is_fluid)
//   {
//     // skip this widget but call calculate sizing on children widgets
//     base_widget_child_node* node = widget->children_head;
//     while(node)
//     {
//       if(node->child->visible)
//       {
//         // result_void _ = node->child->internal_calculate_size(node->child);
//         result_void _ = common_internal_calculate_size(node->child);
//         if(!_.ok)
//         {
//           return _;
//         }
//       }
//       node = node->next;
//     }

//     return ok_void();
//   }

//   uint16 main_axis_length = 0, cross_axis_length = 0;
//   base_widget_child_node* node = widget->children_head;
//   while(node)
//   {
//     // skipping invisible nodes
//     if(!node->child->visible)
//     {
//       node = node->next;
//       continue;
//     }

//     if(node->child->need_resizing)
//     {
//       // node->child->internal_calculate_size(node->child);
//       common_internal_calculate_size(node->child);
//     }
//     if(widget->flexbox_data.container.direction == FLEX_DIRECTION_ROW)
//     {
//       main_axis_length += node->child->w;
//       cross_axis_length = max(cross_axis_length, node->child->h);
//     }
//     else
//     {
//       main_axis_length += node->child->h;
//       cross_axis_length = max(cross_axis_length, node->child->w);
//     }
//     main_axis_length += widget->flexbox_data.container.gap;
//     node = node->next;
//   }
//   main_axis_length -= widget->flexbox_data.container.gap;

//   // assigning calculated axes lengths to widget's dimensions.
//   if(widget->flexbox_data.container.direction == FLEX_DIRECTION_ROW)
//   {
//     widget->w = main_axis_length;
//     widget->h = cross_axis_length;
//   }
//   else
//   {
//     widget->w = cross_axis_length;
//     widget->h = main_axis_length;
//   }

//   return ok_void();
// }

result_void default_internal_relayout_callback(const base_widget* widget)
{
  if(!widget->visible)
  {
    // avoiding calculations when widget is not visible
    return ok_void();
  }

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
    // avoiding invisible children in layout tree
    if(!node->child->visible)
    {
      node = node->next;
      continue;
    }

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
    needed_main_axis_length += widget->flexbox_data.container.gap;
    total_flex_grow += node->child->type == FLEX_CONTAINER
                         ? node->child->flexbox_data.container.flex_grow
                         : node->child->flexbox_data.item.flex_grow;
    total_flex_shrink += node->child->type == FLEX_CONTAINER
                           ? node->child->flexbox_data.container.flex_shrink
                           : node->child->flexbox_data.item.flex_shrink;
    node = node->next;
  }
  needed_main_axis_length -= widget->flexbox_data.container.gap;

  debug("Needed main, cross axes lengths: %d, %d",
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
      // avoiding invisible children in layout tree
      if(!node->child->visible)
      {
        node = node->next;
        continue;
      }

      if(widget->flexbox_data.container.direction == FLEX_DIRECTION_ROW)
      {
        node->child->w += ((node->child->type == FLEX_CONTAINER
                              ? node->child->flexbox_data.container.flex_grow
                              : node->child->flexbox_data.item.flex_grow) /
                           total_flex_grow) *
                          remaining_main_axis_length;
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
      // avoiding invisible children in layout tree
      if(!node->child->visible)
      {
        node = node->next;
        continue;
      }

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
  node = widget->children_head;
  while(node)
  {
    // avoiding invisible children in layout tree
    if(!node->child->visible)
    {
      node = node->next;
      continue;
    }

    flex_cross_axis_sizing cross_axis_sizing =
      node->child->type == FLEX_CONTAINER
        ? node->child->flexbox_data.container.cross_axis_sizing
        : node->child->flexbox_data.item.cross_axis_sizing;

    if(cross_axis_sizing == CROSS_AXIS_SIZING_FIT_CONTENT)
    {
      node = node->next;
      continue;
    }

    // expand child widget's width or height to cross-axis length
    if(widget->flexbox_data.container.direction == FLEX_DIRECTION_ROW)
    {
      debug("Height expanded!");
      node->child->h = cross_axis_length;
    }
    else
    {
      debug("Width expanded!");
      node->child->w = cross_axis_length;
    }

    node = node->next;
  }

  // assigning positions
  int16 x = widget->x, y = widget->y;

  // justify-content flex-align
  switch(widget->flexbox_data.container.justify_content)
  {
  case JUSTIFY_CONTENT_START: {
    // do nothing
    break;
  }
  case JUSTIFY_CONTENT_CENTER: {
    x += remaining_main_axis_length / 2;
    break;
  }
  case JUSTIFY_CONTENT_END: {
    x += remaining_main_axis_length;
    break;
  }
  case JUSTIFY_CONTENT_SPACE_BETWEEN: {
    return error(result_void, "flex-align: space-between not implemented yet!");
  }
  case JUSTIFY_CONTENT_SPACE_AROUND: {
    return error(result_void, "flex-align: space-around not implemented yet!");
  }
  case JUSTIFY_CONTENT_SPACE_EVENLY: {
    return error(result_void, "flex-align: space-evenly not implemented yet!");
  }
  default:
    break;
  }

  node = widget->children_head;
  while(node)
  {
    // avoiding invisible children in layout tree
    if(!node->child->visible)
    {
      node = node->next;
      continue;
    }

    debug("assigning positions x, y: %d, %d", x, y);
    node->child->x = x;

    // flex-align in cross-axis
    switch(widget->flexbox_data.container.align_items)
    {
    case ALIGN_ITEMS_START: {
      node->child->y = y;
      break;
    }
    case ALIGN_ITEMS_CENTER: {
      uint16 remaining_space = cross_axis_length - node->child->h;
      node->child->y = y + (remaining_space / 2);
      break;
    }
    case ALIGN_ITEMS_END: {
      uint16 remaining_space = cross_axis_length - node->child->h;
      node->child->y = y + remaining_space;
      break;
    }
    default:
      break;
    }

    if(widget->flexbox_data.container.direction == FLEX_DIRECTION_ROW)
    {
      x += node->child->w + widget->flexbox_data.container.gap;
    }
    else
    {
      y += node->child->h + widget->flexbox_data.container.gap;
    }
    node = node->next;
  }

  // calling re-layout on children which are container widgets
  node = widget->children_head;
  while(node)
  {
    // avoiding invisible children in layout tree
    if(!node->child->visible)
    {
      node = node->next;
      continue;
    }

    if(node->child->type == FLEX_CONTAINER)
    {
      if(node->child->pre_internal_relayout_hook)
      {
        node->child->pre_internal_relayout_hook(node->child);
      }
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
  if(!widget->visible)
  {
    // escalate this call to its parent
    if(!widget->parent)
    {
      /// TODO: What to do when root widget is invisible?
      /// And internal adjust layout callback is called.
      return ok(result_bool, true);
    }

    return widget->parent->internal_adjust_layout_callback(widget->parent);
  }

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
    common_internal_mark_need_resizing(widget, _.value.x, _.value.y);
  if(!__.ok)
  {
    return error(result_bool, __.error);
  }

  base_widget* ancestor = __.value;
  debug("Ancestor parent: %s", ancestor->parent ? "EXISTS" : "(NULL)");
  // ancestor->internal_calculate_size(ancestor);
  common_internal_calculate_size(ancestor);
  if(ancestor->pre_internal_relayout_hook)
  {
    ancestor->pre_internal_relayout_hook(ancestor);
  }
  ancestor->internal_relayout(ancestor);
  ancestor->internal_render_callback(ancestor);

  return ok(result_bool, true);
}

// static void default_internal_free_callback(base_widget* widget)
// {
//   if(!widget)
//   {
//     return;
//   }

//   // freeing children widgets (not children nodes, node will be free later
//   // in base_widget_free)
//   base_widget_child_node* node = widget->children_head;
//   while(node)
//   {
//     node->child->internal_free_callback(node->child);
//     node = node->next;
//   }

//   // freeing derived widget fields
//   if(widget->derived && widget->internal_derived_free_callback)
//   {
//     widget->internal_derived_free_callback(widget);
//   }

//   base_widget_free(widget);
// }

// result_bool default_internal_mouse_motion_callback(
//   base_widget* widget, internal_mouse_motion_event* internal_event)
// {
//   if(!widget)
//   {
//     return error(result_bool,
//                  "Cannot process internal mouse motion callback on NULL "
//                  "pointed base widget!");
//   }

//   if(internal_event->state == BUBBLING_UP)
//   {
//     // call callbacks if present
//     // setting context's mouse focused widget has already been done by
//     // some child widget down somewhere and set state to BUBBLING_UP.
//     if(widget->mouse_enter_callback)
//     {
//       widget->mouse_enter_callback(widget, internal_event->event);
//     }

//     // return if parent doesn't exist (root element)
//     if(!widget->parent)
//     {
//       return ok(result_bool, true);
//     }

//     // still bubble up if parent exists
//     return widget->parent->internal_mouse_motion_callback(widget->parent,
//                                                           internal_event);
//   }

//   // this widget is target
//   // setting context's mouse focused widget and calling mouse enter callback
//   if(widget->context->mouse_focused_widget != widget)
//   {
//     // should call mouse leave on previously mouse focused widget
//     if(widget->context->mouse_focused_widget &&
//        widget->context->mouse_focused_widget->mouse_leave_callback)
//     {
//       widget->context->mouse_focused_widget->mouse_leave_callback(
//         widget->context->mouse_focused_widget, internal_event->event);
//     }

//     widget->context->mouse_focused_widget = widget;
//     // should call mouse enter on this widget
//     if(widget->mouse_enter_callback)
//     {
//       widget->mouse_enter_callback(widget, internal_event->event);
//     }
//   }
//   else
//   {
//     // call target widget's mouse motion callback if present
//     if(widget->mouse_move_callback)
//     {
//       widget->mouse_move_callback(widget, internal_event->event);
//     }
//   }

//   if(widget->context->active_draggable_widget &&
//      widget->context->active_draggable_widget == widget)
//   {
//     // this widget is the scrollbar widget
//     // consuming mouse motion events
//     // no use of bubbling up these events
//     return ok(result_bool, true);
//   }

//   if(widget->context->overlay_widget &&
//      widget->context->overlay_widget == widget)
//   {
//     // this widget is the overlay widget
//     // no use of bubbling up these events
//     return ok(result_bool, true);
//   }

//   if(!widget->parent)
//   {
//     return ok(result_bool, true);
//   }

//   // bubbling up
//   internal_event->state = BUBBLING_UP;

//   return widget->parent->internal_mouse_motion_callback(widget->parent,
//                                                         internal_event);
// }

// result_bool default_internal_mouse_button_callback(
//   base_widget* widget, internal_mouse_button_event* internal_event)
// {
//   if(!widget)
//   {
//     return error(result_bool,
//                  "Cannot process internal mouse button callback, with widget "
//                  "pointing to NULL!");
//   }

//   if(internal_event->state == BUBBLING_UP)
//   {
//     // call callbacks if present
//     if(internal_event->event.button_state == MOUSE_BUTTON_DOWN)
//     {
//       if(widget->mouse_button_down_callback)
//       {
//         widget->mouse_button_down_callback(widget, internal_event->event);
//       }
//     }
//     else
//     {
//       if(widget->mouse_button_up_callback)
//       {
//         widget->mouse_button_up_callback(widget, internal_event->event);
//       }
//     }

//     // return if parent doesn't exist (root element)
//     if(!widget->parent)
//     {
//       return ok(result_bool, true);
//     }

//     // still bubble up if parent exists
//     return widget->parent->internal_mouse_button_callback(widget->parent,
//                                                           internal_event);
//   }

//   // this widget is target
//   if(internal_event->event.button_state == MOUSE_BUTTON_DOWN)
//   {
//     if(widget->mouse_button_down_callback)
//     {
//       widget->mouse_button_down_callback(widget, internal_event->event);
//     }
//   }
//   else
//   {
//     if(widget->mouse_button_up_callback)
//     {
//       widget->mouse_button_up_callback(widget, internal_event->event);
//     }
//   }

//   if(widget->context->active_draggable_widget &&
//      widget->context->active_draggable_widget == widget)
//   {
//     // this widget is the scrollbar widget
//     // consuming mouse button events
//     // no use of bubbling up these events
//     return ok(result_bool, true);
//   }

//   if(widget->context->overlay_widget &&
//      widget->context->overlay_widget == widget)
//   {
//     // this widget is the overlay widget
//     // no use of bubbling up these events
//     return ok(result_bool, true);
//   }

//   if(!widget->parent)
//   {
//     return ok(result_bool, true);
//   }

//   // bubbling up
//   internal_event->state = BUBBLING_UP;

//   return widget->parent->internal_mouse_button_callback(widget->parent,
//                                                         internal_event);
// }

// result_bool default_internal_mouse_scroll_callback(
//   base_widget* widget, internal_mouse_scroll_event* internal_event)
// {
//   if(!widget)
//   {
//     return error(result_bool,
//                  "Cannot process internal mouse scroll callback, with widget "
//                  "pointing to NULL!");
//   }

//   // mouse scroll event has no bubbling phase
//   // so direclty call the callback if present on this widget

//   if(widget->mouse_scroll_callback)
//   {
//     return ok(result_bool,
//               widget->mouse_scroll_callback(widget, internal_event->event));
//   }

//   return ok(result_bool, false);
// }

//////////////////////////////////////////////////////////
/// Common functions implementation
//////////////////////////////////////////////////////////

rect common_internal_get_bounding_rect(const base_widget* widget)
{
  return (rect){.x = widget->x, .y = widget->y, .w = widget->w, .h = widget->h};
}

result_void common_internal_calculate_size(base_widget* widget) {
  if(!widget->visible)
  {
    // avoiding calculations when widget is not visible
    return ok_void();
  }

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
      if(node->child->visible)
      {
        // result_void _ = node->child->internal_calculate_size(node->child);
        result_void _ = common_internal_calculate_size(node->child);
        if(!_.ok)
        {
          return _;
        }
      }
      node = node->next;
    }

    return ok_void();
  }

  uint16 main_axis_length = 0, cross_axis_length = 0;
  base_widget_child_node* node = widget->children_head;
  while(node)
  {
    // skipping invisible nodes
    if(!node->child->visible)
    {
      node = node->next;
      continue;
    }

    if(node->child->need_resizing)
    {
      common_internal_calculate_size(node->child);
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
    main_axis_length += widget->flexbox_data.container.gap;
    node = node->next;
  }
  main_axis_length -= widget->flexbox_data.container.gap;

  // assigning calculated axes lengths to widget's dimensions.
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

void common_internal_free(base_widget* widget) {
  if(!widget)
  {
    return;
  }

  // freeing children widgets (not children nodes, node will be free later
  // in base_widget_free)
  base_widget_child_node* node = widget->children_head;
  while(node)
  {
    common_internal_free(node->child);
    node = node->next;
  }

  // freeing derived widget fields
  if(widget->derived && widget->internal_derived_free_callback)
  {
    widget->internal_derived_free_callback(widget);
  }

  base_widget_free(widget);
}

result_bool common_internal_mouse_motion(base_widget* widget, internal_mouse_motion_event* internal_event) {
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
    return common_internal_mouse_motion(widget->parent,
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
  else
  {
    // call target widget's mouse motion callback if present
    if(widget->mouse_move_callback)
    {
      widget->mouse_move_callback(widget, internal_event->event);
    }
  }

  if(widget->context->active_draggable_widget &&
     widget->context->active_draggable_widget == widget)
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

  return common_internal_mouse_motion(widget->parent,
                                                        internal_event);
}

result_base_widget_ptr common_internal_mark_need_resizing(base_widget* widget,
                                                          int16 delta_x,
                                                          int16 delta_y) {
  if(widget->type == FLEX_ITEM)
  {
    widget->need_resizing = false;
    if(widget->parent)
    {
      return common_internal_mark_need_resizing(
        widget->parent, delta_x, delta_y);
    }
    return ok(result_base_widget_ptr, widget);
  }

  if(!widget->flexbox_data.container.is_fluid)
  {
    // widget is not fluid, doesn't resize
    debug("Encountered non-fluid widget.");
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
      needed_main_axis_length +=
        node->child->w + widget->flexbox_data.container.gap;
      needed_cross_axis_length = max(needed_cross_axis_length, node->child->h);
      node = node->next;
    }
    needed_main_axis_length -= widget->flexbox_data.container.gap;
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
      needed_main_axis_length +=
        node->child->h + widget->flexbox_data.container.gap;
      needed_cross_axis_length = max(needed_cross_axis_length, node->child->w);
      node = node->next;
    }
    needed_main_axis_length -= widget->flexbox_data.container.gap;
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
    return common_internal_mark_need_resizing(
      widget->parent, delta_x, delta_y);
  }

  return ok(result_base_widget_ptr, widget);
}

result_bool
common_internal_mouse_button(base_widget* widget,
                             internal_mouse_button_event* internal_event) {
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
    return common_internal_mouse_button(widget->parent,
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

  if(widget->context->active_draggable_widget &&
     widget->context->active_draggable_widget == widget)
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

  return common_internal_mouse_button(widget->parent,
                                                        internal_event);
}
result_bool
common_internal_mouse_scroll(base_widget* widget,
                             internal_mouse_scroll_event* internal_event) {
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

result_void common_internal_relayout(const base_widget* widget) {
  if(!widget->visible)
  {
    // avoiding calculations when widget is not visible
    return ok_void();
  }

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
    // avoiding invisible children in layout tree
    if(!node->child->visible)
    {
      node = node->next;
      continue;
    }

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
    needed_main_axis_length += widget->flexbox_data.container.gap;
    total_flex_grow += node->child->type == FLEX_CONTAINER
                         ? node->child->flexbox_data.container.flex_grow
                         : node->child->flexbox_data.item.flex_grow;
    total_flex_shrink += node->child->type == FLEX_CONTAINER
                           ? node->child->flexbox_data.container.flex_shrink
                           : node->child->flexbox_data.item.flex_shrink;
    node = node->next;
  }
  needed_main_axis_length -= widget->flexbox_data.container.gap;

  debug("Needed main, cross axes lengths: %d, %d",
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
      // avoiding invisible children in layout tree
      if(!node->child->visible)
      {
        node = node->next;
        continue;
      }

      if(widget->flexbox_data.container.direction == FLEX_DIRECTION_ROW)
      {
        node->child->w += ((node->child->type == FLEX_CONTAINER
                              ? node->child->flexbox_data.container.flex_grow
                              : node->child->flexbox_data.item.flex_grow) /
                           total_flex_grow) *
                          remaining_main_axis_length;
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
      // avoiding invisible children in layout tree
      if(!node->child->visible)
      {
        node = node->next;
        continue;
      }

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
  node = widget->children_head;
  while(node)
  {
    // avoiding invisible children in layout tree
    if(!node->child->visible)
    {
      node = node->next;
      continue;
    }

    flex_cross_axis_sizing cross_axis_sizing =
      node->child->type == FLEX_CONTAINER
        ? node->child->flexbox_data.container.cross_axis_sizing
        : node->child->flexbox_data.item.cross_axis_sizing;

    if(cross_axis_sizing == CROSS_AXIS_SIZING_FIT_CONTENT)
    {
      node = node->next;
      continue;
    }

    // expand child widget's width or height to cross-axis length
    if(widget->flexbox_data.container.direction == FLEX_DIRECTION_ROW)
    {
      debug("Height expanded!");
      node->child->h = cross_axis_length;
    }
    else
    {
      debug("Width expanded!");
      node->child->w = cross_axis_length;
    }

    node = node->next;
  }

  // assigning positions
  int16 x = widget->x, y = widget->y;

  // justify-content flex-align
  switch(widget->flexbox_data.container.justify_content)
  {
  case JUSTIFY_CONTENT_START: {
    // do nothing
    break;
  }
  case JUSTIFY_CONTENT_CENTER: {
    x += remaining_main_axis_length / 2;
    break;
  }
  case JUSTIFY_CONTENT_END: {
    x += remaining_main_axis_length;
    break;
  }
  case JUSTIFY_CONTENT_SPACE_BETWEEN: {
    return error(result_void, "flex-align: space-between not implemented yet!");
  }
  case JUSTIFY_CONTENT_SPACE_AROUND: {
    return error(result_void, "flex-align: space-around not implemented yet!");
  }
  case JUSTIFY_CONTENT_SPACE_EVENLY: {
    return error(result_void, "flex-align: space-evenly not implemented yet!");
  }
  default:
    break;
  }

  node = widget->children_head;
  while(node)
  {
    // avoiding invisible children in layout tree
    if(!node->child->visible)
    {
      node = node->next;
      continue;
    }

    debug("assigning positions x, y: %d, %d", x, y);
    node->child->x = x;

    // flex-align in cross-axis
    switch(widget->flexbox_data.container.align_items)
    {
    case ALIGN_ITEMS_START: {
      node->child->y = y;
      break;
    }
    case ALIGN_ITEMS_CENTER: {
      uint16 remaining_space = cross_axis_length - node->child->h;
      node->child->y = y + (remaining_space / 2);
      break;
    }
    case ALIGN_ITEMS_END: {
      uint16 remaining_space = cross_axis_length - node->child->h;
      node->child->y = y + remaining_space;
      break;
    }
    default:
      break;
    }

    if(widget->flexbox_data.container.direction == FLEX_DIRECTION_ROW)
    {
      x += node->child->w + widget->flexbox_data.container.gap;
    }
    else
    {
      y += node->child->h + widget->flexbox_data.container.gap;
    }
    node = node->next;
  }

  // calling re-layout on children which are container widgets
  node = widget->children_head;
  while(node)
  {
    // avoiding invisible children in layout tree
    if(!node->child->visible)
    {
      node = node->next;
      continue;
    }

    if(node->child->type == FLEX_CONTAINER)
    {
      if(node->child->pre_internal_relayout_hook)
      {
        node->child->pre_internal_relayout_hook(node->child);
      }
      common_internal_relayout(node->child);
    }
    node = node->next;
  }

  return ok_void();
}

result_bool common_internal_adjust_layout(base_widget* widget) {

}
