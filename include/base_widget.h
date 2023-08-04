#ifndef SMOLL_WIDGETS__BASE_WIDGET_H
#define SMOLL_WIDGETS__BASE_WIDGET_H

#include "events.h"
#include "internal_context.h"
#include "internal_events.h"
#include "types.h"

typedef struct base_widget_child_node base_widget_child_node;

/// @brief Base widget.
typedef struct base_widget
{
  /// @brief Widgets's x-coordinate.
  int16 x;

  /// @brief Widgets's y-coordinate.
  int16 y;

  /// @brief Widget's width (including padding).
  uint16 w;

  /// @brief Widget's height (including padding).
  uint16 h;

  /// @brief Tells if widget should be taken into account while
  ///        layouting and rendering.
  bool visible;

  /// @brief Foreground color of widget.
  ///        Applies to text (if widget conatains any).
  color foreground;

  /// @brief Background color of widget.
  color background;

  /// @brief Pointer to derived widget.
  ///        Casting to derived widget type is required to access it.
  void* derived;

  /// @brief Parent of this widget.
  base_widget* parent;

  /// @brief Head of children linked list.
  base_widget_child_node* children_head;

  /// @brief Context of this widget.
  internal_context* context;

  /// @brief Internal callback for getting bounding rectangle of widget.
  ///        Will be handy when layouting.
  rect (*internal_get_bounding_rect_callback)(base_widget*);

  /// TODO: Give a thought why return types of internal callbacks have to be
  ///       result_bool ?

  /// @brief Internal callback for adjusting layout of this widget.
  result_bool (*internal_fit_layout_callback)(base_widget*);

  /// @brief Internal callback for re-assigning positions to all children.
  result_bool (*internal_assign_positions)(base_widget*);

  /// @brief Internal callback for rendering this widget.
  result_bool (*internal_render_callback)(const base_widget*);

  /// @brief Internal callback for processing internal mouse motion event.
  ///        These internal callbacks are internally handled by base widget.
  result_bool (*internal_mouse_motion_callback)(base_widget*,
                                                internal_mouse_motion_event*);

  /// @brief Internal callback for processing internal mouse button event.
  ///        These internal callbacks are internally handled by base widget.
  result_bool (*internal_mouse_button_callback)(base_widget*,
                                                internal_mouse_button_event*);

  /// @brief Internal callback for processing internal mouse scroll event.
  ///        These internal callbacks are internally handled by base widget.
  result_bool (*internal_mouse_scroll_callback)(base_widget*,
                                                internal_mouse_scroll_event*);

  /// @brief Derived widget's callback for mouse button down callback.
  ///        It's the derived widget's responsibility for using these
  ///        callbacks appropriately.
  bool (*mouse_button_down_callback)(base_widget*, const mouse_button_event);

  /// @brief Derived widget's callback for mouse button up callback.
  ///        It's the derived widget's responsibility for using these
  ///        callbacks appropriately.
  bool (*mouse_button_up_callback)(base_widget*, const mouse_button_event);

  /// @brief Derived widget's callback for mouse enter callback.
  ///        It's the derived widget's responsibility for using these
  ///        callbacks appropriately.
  bool (*mouse_enter_callback)(base_widget*, const mouse_motion_event);

  /// @brief Derived widget's callback for mouse leave callback.
  ///        It's the derived widget's responsibility for using these
  ///        callbacks appropriately.
  bool (*mouse_leave_callback)(base_widget*, const mouse_motion_event);

  /// @brief Derived widget's callback for mouse scroll callback.
  ///        It's the derived widget's responsibility for using these
  ///        callbacks appropriately.
  bool (*mouse_scroll_callback)(base_widget*, const mouse_scroll_event);
} base_widget;

/// @brief Base widget child node.
struct base_widget_child_node
{
  /// @brief Child
  base_widget* child;

  /// @brief Next child node.
  base_widget_child_node* next;
};

typedef struct result_base_widget_ptr
{
  bool ok;
  union
  {
    base_widget* value;
    const char* error;
  };
} result_base_widget_ptr;

typedef struct result_base_widget_child_node_ptr
{
  bool ok;
  union
  {
    base_widget_child_node* value;
    const char* error;
  };
} result_base_widget_child_node_ptr;

/// @brief Creates a new base widegt child node enclosing this widget.
/// @param child the widget to create child node for.
/// @return Base widget child node pointer result.
result_base_widget_child_node_ptr
base_widget_child_node_new(base_widget* child);

/// @brief Frees the resources used by the child node.
///        This doesn't free the widget it encloses.
/// @param node pointer to child node.
/// @return Void result.
result_void base_widget_child_node_free(base_widget_child_node* node);

/// @brief Creates a new base widget.
/// @return Base widget pointer result.
result_base_widget_ptr base_widget_new();

/// @brief Adds the given child widget to this widget.
/// @param base pointer to base widget, for which the child is to be added.
/// @param child pointer to base widget of the child to be added.
/// @return Void result.
result_void base_widget_add_child(base_widget* base, base_widget* child);

/// @brief Removes the given widget from this widget's children.
/// @param base pointer to base widget, from which the child should be removed.
/// @param child pointer to base widget of the child.
/// @return Void result.
result_void base_widget_remove_child(base_widget* base, base_widget* child);

/// @brief Frees the base widget.
/// @param widget pointer to the base widget.
/// @return Void result.
result_void base_widget_free(base_widget* widget);

#endif