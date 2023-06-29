#ifndef SMOLL_WIDGETS__SMOLL_CONTEXT_H
#define SMOLL_WIDGETS__SMOLL_CONTEXT_H

#include "types.h"
#include "events.h"

typedef struct smoll_context smoll_context;

typedef struct result_smoll_context_ptr {
  bool ok;
  union {
    smoll_context* value;
    const char* error;
  };
} result_smoll_context_ptr;

result_smoll_context_ptr smoll_context_create();
result_void smoll_context_process_mouse_motion_event(smoll_context* context, const mouse_motion_event event);
result_void smoll_context_process_mouse_button_event(smoll_context* context, const mouse_button_event event);
result_void smoll_context_process_mouse_wheel_event(smoll_context* context, const mouse_scroll_event event);
result_void smoll_context_destroy(smoll_context* context);

result_base_widget_ptr smoll_context_get_mouse_focused_widget(const smoll_context* context);
result_const_char_ptr smoll_context_get_font(const smoll_context* context);
result_uint8 smoll_context_get_font_size(const smoll_context* context);
result_command_buffer_ptr smoll_context_get_command_buffer(const smoll_context* context);

result_void smoll_context_set_mouse_focused_widget(smoll_context* context, base_widget* widget);

#endif