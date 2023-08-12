#ifndef SMOLL_WIDGETS__COMMAND_BUFFER_H
#define SMOLL_WIDGETS__COMMAND_BUFFER_H

#include "types.h"

typedef enum command_type
{
  RENDER_TEXT,
  RENDER_RECT,
  PUSH_CLIP_RECT,
  POP_CLIP_RECT
} command_type;

typedef struct render_rect_data
{
  rect bounding_rect;
  color rect_color;
} render_rect_data;

typedef struct render_text_data
{
  const char* text;
  color text_color;
  point text_coordinates;
} render_text_data;

typedef struct command
{
  command_type type;
  union
  {
    render_rect_data render_rect;
    render_text_data render_text;
    rect clip_rect;
  } data;
} command;

typedef struct command_node
{
  command* cmd;

  struct command_node* next;
} command_node;

typedef struct command_buffer
{
  command_node* head;
  command_node* tail;

  uint16 length;
} command_buffer;

typedef struct result_command_ptr
{
  bool ok;
  union
  {
    command* value;
    const char* error;
  };
} result_command_ptr;

typedef struct result_command_buffer_ptr
{
  bool ok;
  union
  {
    command_buffer* value;
    const char* error;
  };
} result_command_buffer_ptr;

result_command_ptr command_new_render_rect(const rect bounding_rect,
                                           const color rect_color);
result_command_ptr command_new_render_text(const char* text,
                                           const color text_color,
                                           point text_coordinates);
result_command_ptr command_new_push_clip_rect(rect clip_rect);
result_command_ptr command_new_pop_clip_rect();
result_void command_free(command* cmd);

result_command_buffer_ptr command_buffer_new();
result_void command_buffer_add_command(command_buffer* buffer, command* cmd);
result_void command_buffer_add_render_rect_command(command_buffer* buffer,
                                                   const rect bounding_rect,
                                                   const color rect_color);
result_void command_buffer_add_render_text_command(command_buffer* buffer,
                                                   const char* text,
                                                   const color text_color,
                                                   point text_coordinates);
result_void command_buffer_add_push_clip_rect_command(command_buffer* buffer,
                                                      rect clip_rect);
result_void command_buffer_add_pop_clip_rect_command(command_buffer* buffer);
result_command_ptr command_buffer_get_next_command(command_buffer* buffer);
result_void command_buffer_clear_commands(command_buffer* buffer);
result_void command_buffer_free(command_buffer* buffer);

#endif