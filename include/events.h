#ifndef SMOLL_WIDGETS__EVENTS_H
#define SMOLL_WIDGETS__EVENTS_H

#include "types.h"

typedef struct viewport_resize_event
{
  uint16 w, h;
} viewport_resize_event;

typedef struct mouse_motion_event
{
  uint16 x, y;
  uint16 global_x, global_y;
} mouse_motion_event;

typedef enum mouse_button_type
{
  MOUSE_BUTTON_LEFT,
  MOUSE_BUTTON_RIGHT,
  MOUSE_BUTTON_MIDDLE
} mouse_button_type;

typedef enum mouse_button_state
{
  MOUSE_BUTTON_DOWN,
  MOUSE_BUTTON_UP,
  MOUSE_CLICK,
  MOUSE_DOUBLE_CLICK,
  MOUSE_TRIPLE_CLICK
} mouse_button_state;

typedef struct mouse_button_event
{
  mouse_button_type button;
  mouse_button_state button_state;
  uint16 x, y;
} mouse_button_event;

typedef struct mouse_scroll_event
{
  float32 delta_x, delta_y;
} mouse_scroll_event;

typedef struct clock_tick_event
{
  uint64 clock_ticks;
} clock_tick_event;

#endif