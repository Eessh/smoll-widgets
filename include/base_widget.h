#ifndef SMOLL_WIDGETS__BASE_WIDGET_H
#define SMOLL_WIDGETS__BASE_WIDGET_H

#include "types.h"
#include "events.h"
#include "smoll_context.h"

typedef struct base_widget base_widget;
typedef struct base_widget_child_node base_widget_child_node;

typedef struct result_base_widget_ptr {
  bool ok;
  union {
    base_widget* value;
    const char* error;
  };
} result_base_widget_ptr;

typedef struct result_base_widget_child_node_ptr {
  bool ok;
  union {
    base_widget_child_node* value;
    const char* error;
  };
} result_base_widget_child_node_ptr;

typedef enum callback_type {
  PASSING_DOWN,
  BUBBLING_UP
} callback_type;

typedef enum event_callback_type {
  EVENT_CALLBACK_TYPE_MOUSE_ENTER,
  EVENT_CALLBACK_TYPE_MOUSE_LEAVE,
  EVENT_CALLBACK_TYPE_MOUSE_BUTTON_DOWN,
  EVENT_CALLBACK_TYPE_MOUSE_BUTTON_UP,
  EVENT_CALLBACK_TYPE_MOUSE_SCROLL
} event_callback_type;

struct base_widget {
  int16 x, y;
  uint16 w, h;
  bool visible;
  color foreground, background;

  void* derived;
  base_widget* parent;
  base_widget_child_node* children_head;

  smoll_context* context;
  
  rect (*internal_get_bounding_rect_callback)(base_widget*);
  result_bool (*internal_fit_layout_callback)(base_widget*);
  result_bool (*internal_assign_positions)(base_widget*);
  result_bool (*internal_render_callback)(const base_widget*);

  result_bool (*internal_mouse_motion_callback)(base_widget*, const mouse_motion_event, const callback_type);
  result_bool (*internal_mouse_button_callback)(base_widget*, const mouse_button_event, const callback_type);
  result_bool (*internal_mouse_scroll_callback)(base_widget*, const mouse_scroll_event, const callback_type);
  
  bool (*mouse_button_down_callback)(base_widget*, const mouse_button_event);
  bool (*mouse_button_up_callback)(base_widget*, const mouse_button_event);
  bool (*mouse_enter_callback)(base_widget*, const mouse_motion_event);
  bool (*mouse_leave_callback)(base_widget*, const mouse_motion_event);
  bool (*mouse_scroll_callback)(base_widget*, const mouse_scroll_event);
};

struct base_widget_child_node {
  base_widget* child;
  base_widget* next;
};

result_base_widget_child_node_ptr base_widget_child_node_new(base_widget* child);
result_void base_widget_child_node_free(base_widget_child_node* node);

result_base_widget_ptr base_widget_new(smoll_context* context);
result_void base_widget_add_child(base_widget* base, base_widget* child);
result_void base_widget_remove_child(base_widget* base, base_widget* child);
result_void base_widget_free(base_widget* widget);

#endif