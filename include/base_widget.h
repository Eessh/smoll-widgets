#ifndef SMOLL_WIDGETS__BASE_WIDGET_H
#define SMOLL_WIDGETS__BASE_WIDGET_H

#include "backend.h"
#include "command_buffer.h"
#include "events.h"
#include "types.h"

/// Forward declarations
typedef struct base_widget base_widget;
typedef struct base_widget_child_node base_widget_child_node;
typedef struct internal_mouse_motion_event internal_mouse_motion_event;
typedef struct internal_mouse_button_event internal_mouse_button_event;
typedef struct internal_mouse_scroll_event internal_mouse_scroll_event;
typedef struct internal_context internal_context;

///////////////////////////////////////////////////////////////////////////////
/// * Widget's Flex-Box
///////////////////////////////////////////////////////////////////////////////

/// @brief Type of widget, either it is a flex-box container or just a
///        flex-box item.
typedef enum widget_type
{
  /// @brief This type of widget acts as a fluid container for its children
  ///        with flex-box layouting.
  FLEX_CONTAINER,

  /// @brief This type of widget goes into a parent widget which is of type
  ///        `FLEX_CONTAINER`.
  ///        The `FLEX_ITEM` widgets should not be having any children.
  FLEX_ITEM
} widget_type;

/// @brief Flex-box loyouting direction.
typedef enum flex_direction
{
  /// @brief Flex-items will be arranged horizonatlly.
  ///        To be more precise the main-axis will be horizontal,
  ///        cross-axis will be vertical.
  FLEX_DIRECTION_ROW,

  /// @brief Flex-items will be arranged vertically.
  ///        To be more precise the main-axis will be vertical,
  ///        cross-axis will be horizontally.
  FLEX_DIRECTION_COLUMN
} flex_direction;

/// @brief Alignment of children in flex-box container.
typedef enum flex_align
{
  /// @brief Children will be aligned from left-end
  ///        (or top if `FLEX_DIRECTION_COLUMN`) of parent.
  FLEX_ALIGN_START,

  /// @brief Children will be placed in center of parent.
  FLEX_ALIGN_CENTER,

  /// @brief Children will be aligned from right-end
  ///        (or bottom if `FLEX_DIRECTION_COLUMN`) of parent.
  FLEX_ALIGN_END,

  /// @brief Not supported yet.
  FLEX_ALIGN_SPACE_BETWEEN,
  FLEX_ALIGN_SPACE_AROUND,
  FLEX_ALIGN_SPACE_EVENLY
} flex_align;

/// @brief Flex-box related data for `FLEX_CONTAINER`.
typedef struct flex_container_data
{
  /// @brief Tells whether container sizing is controlled by layouting engine
  ///        or by the user.
  ///        If `true` (default value) sizing is set according to children,
  ///        else this widget is going to have fixed width and height,
  ///        unless changed by user.
  bool is_fluid;

  /// @brief Flex-box layouting direction.
  flex_direction direction;

  /// @brief Main-axis flex-items alignment.
  flex_align justify_content;

  /// @brief Cross-axis flex-items alignment.
  flex_align align_items;

  uint8 flex_grow;
  uint8 flex_shrink;
} flex_container_data;

/// @brief Flex-box related data for `FLEX_ITEM`.
typedef struct flex_item_data
{
  /// @brief Determines the flex-item's share of remaining space in parent.
  uint8 flex_grow;

  /// @brief Determines the flex-item's potential to shrink.
  uint8 flex_shrink;
} flex_item_data;

///////////////////////////////////////////////////////////////////////////////
/// * Base Widget
///////////////////////////////////////////////////////////////////////////////

/// @brief Base widget.
struct base_widget
{
  /// @brief Widget's x-coordinate.
  int16 x;

  /// @brief Widget's y-coordinate.
  int16 y;

  /// @brief Widget's width (including padding).
  uint16 w;

  /// @brief Widget's height (including padding).
  uint16 h;

  /// @brief Widget's flex-box type.
  ///        If `FLEX_CONTAINER` this widget can have children otherwise not.
  widget_type type;

  /// @brief Flex-box related data of widget.
  union
  {
    /// @brief Flex-container related data of this widget.
    flex_container_data container;

    /// @brief Flex-item related data of this widget.
    flex_item_data item;
  } flexbox_data;

  /// @brief Flag to know whether widget's width and height needs to be
  ///        re-calculated.
  bool need_resizing;

  /// @brief Tells if widget should be taken into account while
  ///        layouting and rendering.
  bool visible;

  /// @brief Pointer to derived widget.
  ///        Casting to derived widget type is required to access it.
  void* derived;

  /// @brief Parent of this widget.
  base_widget* parent;

  /// @brief Head of children linked list.
  base_widget_child_node* children_head;

  /// @brief Context of this widget.
  internal_context* context;

  result_void (*calculate_size)(base_widget*);

  result_void (*internal_relayout)(const base_widget*);

  /// @brief Internal callback for getting bounding rectangle of widget.
  ///        Will be handy when layouting.
  rect (*internal_get_bounding_rect_callback)(const base_widget*);

  /**
   * @brief Internal callback for getting active background of this widget.
   * @param widget pointer to widget's base.
   * @return Color struct.
   */
  color (*internal_get_background_callback)(const base_widget* widget);

  /// TODO: Give a thought why return types of internal callbacks have to be
  ///       result_bool ?

  /// @brief Internal callback for adjusting layout of this widget.
  result_bool (*internal_fit_layout_callback)(base_widget*, bool);

  /// @brief Internal callback for adjusting layout and sizing of this widget,
  ///        and automatically calling this callback on parent widgets
  ///        if needs resizing.
  result_bool (*internal_adjust_layout_callback)(base_widget*);

  /// @brief Internal callback for re-assigning positions to all children.
  result_bool (*internal_assign_positions)(base_widget*);

  /// @brief Internal callback for rendering this widget.
  result_bool (*internal_render_callback)(const base_widget*);

  /// @brief Internal callback for freeing UI tree recursively.
  void (*internal_free_callback)(base_widget* widget);

  /// @brief Internal callback for freeing derived widget (if exists) specific
  ///        fields. This should be implemented by all derived widgets.
  void (*internal_derived_free_callback)(base_widget* widget);

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
};

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
result_base_widget_ptr base_widget_new(widget_type type);

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

///////////////////////////////////////////////////////////////////////////////
/// * Internal Events
///////////////////////////////////////////////////////////////////////////////

/// @brief Internal event types.
typedef enum internal_event_type
{
  /// @brief Internal mouse motion event type.
  MOUSE_MOTION_INTERNAL_EVENT,

  /// @brief Internal mouse button event type.
  MOUSE_BUTTON_INTERNAL_EVENT,

  /// @brief Internal mouse scroll event type.
  MOUSE_SCROLL_INTERNAL_EVENT
} internal_event_type;

/// @brief State (or) pahse of internal events.
typedef enum internal_event_state
{
  /// @brief Internal event is at its target, and event is fired.
  AT_TARGET,

  /// @brief Internal event has passed it target, and now it is propagating
  ///        through its parents.
  BUBBLING_UP
} internal_event_state;

/// @brief Internal mouse motion event.
///        This event has BUBBLING_UP phase.
typedef struct internal_mouse_motion_event
{
  /// @brief The actual mouse motion event with data about event.
  mouse_motion_event event;

  /// @brief Target widget of this internal event.
  base_widget* target;

  /// @brief State of this internal event.
  internal_event_state state;

  /// @brief Tells if this internal event still propagates through
  ///        the target's parents.
  bool propagation;
} internal_mouse_motion_event;

/// @brief Internal mouse button event.
///        This event has BUBBLING_UP phase.
typedef struct internal_mouse_button_event
{
  /// @brief The actual mouse button event with data about event.
  mouse_button_event event;

  /// @brief Target widget of this internal event.
  base_widget* target;

  /// @brief State of this internal event.
  internal_event_state state;

  /// @brief Tells if this internal event still propagates through
  ///        the target's parents.
  bool propagation;
} internal_mouse_button_event;

/// @brief Internal mouse scroll event.
///        This event does not have a BUBBLING_UP phase.
typedef struct internal_mouse_scroll_event
{
  /// @brief The actual mouse scroll event with data about event.
  mouse_scroll_event event;

  /// @brief Target widget of this internal event.
  base_widget* target;
} internal_mouse_scroll_event;

///////////////////////////////////////////////////////////////////////////////
/// * Internal Context
///////////////////////////////////////////////////////////////////////////////

/// @brief Internal context.
///        This is public to all widgets.
///        Use `internal_context_new()` to create this context.
///        Do not allocate this on stack, as there will be high possibility
///        of memory leak by not freeing UI tree when the context
///        goes out of scope.
typedef struct internal_context
{
  /// @brief Local mouse x-coordinate.
  uint16 mouse_x;

  /// @brief Local mouse y-coordinate.
  uint16 mouse_y;

  /// @brief Global mouse x-coordinate.
  uint16 global_mouse_x;

  /// @brief Global mouse y-coodinate.
  uint16 global_mouse_y;

  /// @brief Viewport width.
  uint16 viewport_w;

  /// @brief Viewport height.
  uint16 viewport_h;

  /// @brief Root widget.
  base_widget* root;

  /// @brief Overlay (or) popup widget.
  ///        If this exists, all events will be forwarded to this widget.
  ///        No events will be given to the root widget.
  base_widget* overlay_widget;

  /// @brief Active scrollbar widget.
  ///        If this exists, all mouse motion events will be forwarded
  ///        to this widget.
  base_widget* active_scrollbar;

  /// @brief Widget with keyboard focus.
  ///        If this exists, all keyboard events will be forwarded
  ///        to this widget.
  base_widget* keyboard_focused_widget;

  /// @brief Widget with mouse focus.
  ///        This is used to send mouse leave events, when mouse enters
  ///        a new widget.
  base_widget* mouse_focused_widget;

  /// @brief Default font for context.
  ///        This should be loaded by backend.
  char* font;

  /// @brief Default font size.
  uint8 font_size;

  /// @brief Command buffer.
  command_buffer* cmd_buffer;

  /// @brief Backend.
  render_backend* backend;
} internal_context;

/// @brief Internal context pointer result.
typedef struct result_internal_context_ptr
{
  bool ok;
  union
  {
    internal_context* value;
    const char* error;
  };
} result_internal_context_ptr;

/// @brief Creates a new internal context.
/// @return Internal context pointer result.
result_internal_context_ptr internal_context_create();

/// @brief Frees resources used by internal context.
/// @param context pointer to internal context.
/// @return Void result.
result_void internal_context_destroy(internal_context* context);

/// @brief Gets the deepest widget which encloses the point.
/// @param context const pointer to internal context.
/// @param x point x-coordinate.
/// @param y point y-coordinate.
/// @return Base widget pointer result.
result_base_widget_ptr internal_context_get_deepest_widget_with_point(
  const internal_context* context, uint16 x, uint16 y);

/// @brief Gets the deepest widget which encloses the point, and the widget
///        has subscribed to the event type.
/// @param context const pointer to internal context.
/// @param x point x-coordinate.
/// @param y point y-coordinate.
/// @param type event type.
/// @return Base widget pointer result.
result_base_widget_ptr
internal_context_get_deepest_widget_with_point_and_event_type(
  const internal_context* context,
  uint16 x,
  uint16 y,
  internal_event_type type);

/// @brief Processes internal mouse motion event.
/// @param context pointer to internal context.
/// @param internal_event poiinter to internal mouse motion event.
/// @return void result.
result_void internal_context_process_mouse_motion_event(
  internal_context* context, internal_mouse_motion_event* internal_event);

/// @brief Processes internal mouse button event.
/// @param context pointer to internal context.
/// @param internal_event poiinter to internal mouse button event.
/// @return Void result.
result_void internal_context_process_mouse_button_event(
  internal_context* context, internal_mouse_button_event* internal_event);

/// @brief Processes internal mouse scroll event.
/// @param context pointer to internal context.
/// @param internal_event poiinter to internal mouse scroll event.
/// @return Void result.
result_void internal_context_process_mouse_scroll_event(
  internal_context* context, internal_mouse_scroll_event* internal_event);

#endif