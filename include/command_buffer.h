#ifndef SMOLL_WIDGETS__COMMAND_BUFFER_H
#define SMOLL_WIDGETS__COMMAND_BUFFER_H

#include "types.h"

typedef enum command_type {
  RENDER_TEXT,
  RENDER_RECT
} command_type;

typedef struct command {
  command_type type;
  rect bounding_rect;
  color rect_color;
  const char* text;
  color text_color;

  struct command* next;
} command;

typedef struct command_buffer {
  command* head;
  command* tail;
} command_buffer;

typedef struct result_command_ptr {
  bool ok;
  union {
    command* value;
    const char* error;
  };
} result_command_ptr;

typedef struct result_command_buffer_ptr {
  bool ok;
  union {
    command_buffer* value;
    const char* error;
  };
} result_command_buffer_ptr;

result_command_ptr command_new_render_rect(const rect bounding_rect, const color rect_color);
result_command_ptr command_new_render_text(const char* text, const color text_color, const rect bounding_rect, const color rect_color);
result_void command_free(command* cmd);

result_command_buffer_ptr command_buffer_new();
result_void command_buffer_add_command(command_buffer* buffer, command* cmd);
result_void command_buffer_add_render_rect_command(command_buffer* buffer, const rect bounding_rect, const color rect_color);
result_void command_buffer_add_render_text_command(command_buffer* buffer, const char* text, const color text_color, const rect bounding_rect, const color rect_color);
result_void command_buffer_process_commands(command_buffer* buffer);
result_void command_buffer_free(command_buffer* buffer);

#endif