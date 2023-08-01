#ifndef SMOLL_WIDGETS__INTERNAL_EVENTS_H
#define SMOLL_WIDGETS__INTERNAL_EVENTS_H

#include "base_widget.h"

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

#endif