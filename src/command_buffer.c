#include "../include/command_buffer.h"
#include <stdlib.h>
#include <string.h>
#include "../include/macros.h"

/// Command node for linked list of command buffer.
struct command_node
{
  command* cmd;

  struct command_node* next;
};

/// Buffer for holding all commands produced by widgets.
/// This is cleaned up for every frame.
struct command_buffer
{
  command_node* head;
  command_node* tail;

  uint16 length;
};

struct command_buffer_const_iterator_private_data
{
  command_node* node;
};

result_command_ptr command_new_render_rect(const rect bounding_rect,
                                           const color rect_color)
{
  command* cmd = (command*)calloc(1, sizeof(command));
  if(!cmd)
  {
    return error(result_command_ptr, "Unable to allocate memory for command!");
  }

  cmd->type = RENDER_RECT;
  cmd->data.render_rect = (render_rect_data){.bounding_rect = bounding_rect,
                                             .rect_color = rect_color};

  return ok(result_command_ptr, cmd);
}

result_command_ptr command_new_render_rounded_rect(const rect bounding_rect,
                                                   const uint8 border_radius,
                                                   const color rect_color)
{
  command* cmd = (command*)calloc(1, sizeof(command));
  if(!cmd)
  {
    return error(result_command_ptr, "Unable to allocate memory for command!");
  }

  cmd->type = RENDER_ROUNDED_RECT;
  cmd->data.render_rounded_rect =
    (render_rounded_rect_data){.bounding_rect = bounding_rect,
                               .border_radius = border_radius,
                               .rect_color = rect_color};

  return ok(result_command_ptr, cmd);
}

result_command_ptr
command_new_render_rect_outlined(const rect bounding_rect,
                                 const color rect_outline_color)
{
  command* cmd = (command*)calloc(1, sizeof(command));
  if(!cmd)
  {
    return error(result_command_ptr, "Unable to allocate memory for command!");
  }

  cmd->type = RENDER_RECT_OUTLINED;
  cmd->data.render_rect = (render_rect_data){.bounding_rect = bounding_rect,
                                             .rect_color = rect_outline_color};

  return ok(result_command_ptr, cmd);
}

result_command_ptr command_new_render_line(point begin, point end)
{
  command* cmd = (command*)calloc(1, sizeof(command));
  if(!cmd)
  {
    return error(result_command_ptr, "Unable to allocate memory for command!");
  }

  cmd->type = RENDER_LINE;
  cmd->data.render_line = (render_line_data){.begin = begin, .end = end};

  return ok(result_command_ptr, cmd);
}

result_command_ptr command_new_render_text(const char* text,
                                           const color text_color,
                                           point text_coordinates)
{
  if(!text)
  {
    return error(result_command_ptr,
                 "Cannot create command for rendering "
                 "text, with text pointing to NULL!");
  }

  command* cmd = (command*)calloc(1, sizeof(command));
  if(!cmd)
  {
    return error(result_command_ptr, "Unable to allocate memory for command!");
  }

  cmd->type = RENDER_TEXT;
  cmd->data.render_text =
    (render_text_data){.text = strdup(text),
                       .text_color = text_color,
                       .text_coordinates = text_coordinates};

  return ok(result_command_ptr, cmd);
}

result_command_ptr command_new_push_clip_rect(rect clip_rect)
{
  command* cmd = (command*)calloc(1, sizeof(command));
  if(!cmd)
  {
    return error(result_command_ptr, "Unable to allocate memory for command!");
  }

  cmd->type = PUSH_CLIP_RECT;
  cmd->data.clip_rect = clip_rect;

  return ok(result_command_ptr, cmd);
}

result_command_ptr command_new_pop_clip_rect()
{
  command* cmd = (command*)calloc(1, sizeof(command));
  if(!cmd)
  {
    return error(result_command_ptr, "Unable to allocate memory for command!");
  }

  cmd->type = POP_CLIP_RECT;

  return ok(result_command_ptr, cmd);
}

result_command_ptr command_new_set_cursor(command_type cursor_type)
{
  command* cmd = (command*)calloc(1, sizeof(command));
  if(!cmd)
  {
    return error(result_command_ptr, "Unable to allocate memory for command!");
  }

  cmd->type = cursor_type;

  return ok(result_command_ptr, cmd);
}

result_command_ptr command_new_clear_window()
{
  command* cmd = (command*)calloc(1, sizeof(command));
  if(!cmd)
  {
    return error(result_command_ptr, "Unable to allocate memory for command!");
  }

  cmd->type = CLEAR_WINDOW;

  return ok(result_command_ptr, cmd);
}

result_void command_free(command* cmd)
{
  if(!cmd)
  {
    return error(result_void, "Attempt to free a NULL pointed command!");
  }

  free(cmd);

  return ok_void();
}

typedef struct result_command_node_ptr
{
  bool ok;
  union
  {
    command_node* value;
    const char* error;
  };
} result_command_node_ptr;

result_command_node_ptr command_node_new(command* cmd)
{
  command_node* cmd_node = (command_node*)calloc(1, sizeof(command_node));
  if(!cmd_node)
  {
    return error(result_command_node_ptr,
                 "Unable to allocate memory for command node!");
  }

  cmd_node->cmd = cmd;
  cmd_node->next = NULL;

  return ok(result_command_node_ptr, cmd_node);
}

result_void command_node_free(command_node* node)
{
  if(!node)
  {
    return error(result_void, "Attempt to free a NULL pointing command node!");
  }

  result_void _ = command_free(node->cmd);
  if(!_.ok)
  {
    return _;
  }

  free(node);

  return ok_void();
}

result_void command_node_shallow_free(command_node* node)
{
  if(!node)
  {
    return error(result_void,
                 "Attempt to shallow-free a NULL pointing command node!");
  }

  free(node);

  return ok_void();
}

result_command_buffer_ptr command_buffer_new()
{
  command_buffer* buffer = (command_buffer*)calloc(1, sizeof(command_buffer));
  if(!buffer)
  {
    return error(result_command_buffer_ptr,
                 "Unable to allocate memory for command buffer!");
  }

  buffer->head = NULL;
  buffer->tail = NULL;
  buffer->length = 0;

  return ok(result_command_buffer_ptr, buffer);
}

int16 command_buffer_length(const command_buffer* buffer)
{
  if(!buffer)
  {
    return -1;
  }

  return buffer->length;
}

result_void command_buffer_add_command(command_buffer* buffer, command* cmd)
{
  if(!buffer)
  {
    return error(result_void,
                 "Cannot add command to NULL pointed command buffer!");
  }

  if(!cmd)
  {
    return error(result_void,
                 "Cannot add NULL pointed command to command buffer!");
  }

  result_command_node_ptr _ = command_node_new(cmd);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  command_node* cmd_node = _.value;

  buffer->length += 1;

  if(!buffer->head)
  {
    buffer->head = cmd_node;
    buffer->tail = cmd_node;

    return ok_void();
  }

  buffer->tail->next = cmd_node;
  buffer->tail = cmd_node;

  return ok_void();
}

result_void command_buffer_add_render_rect_command(command_buffer* buffer,
                                                   const rect bounding_rect,
                                                   const color rect_color)
{
  if(!buffer)
  {
    return error(result_void,
                 "Cannot add command to NULL pointed command buffer!");
  }

  result_command_ptr _ = command_new_render_rect(bounding_rect, rect_color);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  command* cmd = _.value;
  result_void __ = command_buffer_add_command(buffer, cmd);
  if(!__.ok)
  {
    return __;
  }

  return ok_void();
}

result_void
command_buffer_add_render_rounded_rect_command(command_buffer* buffer,
                                               const rect bounding_rect,
                                               const uint8 border_radius,
                                               const color rect_color)
{
  if(!buffer)
  {
    return error(result_void,
                 "Cannot add command to NULL pointed command buffer!");
  }

  result_command_ptr _ =
    command_new_render_rounded_rect(bounding_rect, border_radius, rect_color);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  command* cmd = _.value;
  result_void __ = command_buffer_add_command(buffer, cmd);
  if(!__.ok)
  {
    return __;
  }

  return ok_void();
}

result_void
command_buffer_add_render_rect_outline_command(command_buffer* buffer,
                                               const rect bounding_rect,
                                               const color rect_outline_color)
{
  if(!buffer)
  {
    return error(result_void,
                 "Cannot add command to NULL pointed command buffer!");
  }

  result_command_ptr _ =
    command_new_render_rect_outlined(bounding_rect, rect_outline_color);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  command* cmd = _.value;
  result_void __ = command_buffer_add_command(buffer, cmd);
  if(!__.ok)
  {
    return __;
  }

  return ok_void();
}

result_void command_buffer_add_render_line_command(command_buffer* buffer,
                                                   point begin,
                                                   point end)
{
  if(!buffer)
  {
    return error(result_void,
                 "Cannot add command to NULL pointed command buffer!");
  }

  result_command_ptr _ = command_new_render_line(begin, end);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  command* cmd = _.value;
  result_void __ = command_buffer_add_command(buffer, cmd);
  if(!__.ok)
  {
    return __;
  }

  return ok_void();
}

result_void command_buffer_add_render_text_command(command_buffer* buffer,
                                                   const char* text,
                                                   const color text_color,
                                                   point text_coordinates)
{
  if(!buffer)
  {
    return error(result_void,
                 "Cannot add command to NULL pointed command buffer!");
  }

  if(!text)
  {
    return error(
      result_void,
      "Cannot add a render text command, with text pointing to NULL!");
  }

  result_command_ptr _ =
    command_new_render_text(text, text_color, text_coordinates);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  command* cmd = _.value;
  result_void __ = command_buffer_add_command(buffer, cmd);
  if(!__.ok)
  {
    return __;
  }

  return ok_void();
}

result_void command_buffer_add_push_clip_rect_command(command_buffer* buffer,
                                                      rect clip_rect)
{
  if(!buffer)
  {
    return error(result_void,
                 "Cannot add command to NULL pointed command buffer!");
  }

  result_command_ptr _ = command_new_push_clip_rect(clip_rect);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  command* cmd = _.value;
  result_void __ = command_buffer_add_command(buffer, cmd);
  if(!__.ok)
  {
    return __;
  }

  return ok_void();
}

result_void command_buffer_add_pop_clip_rect_command(command_buffer* buffer)
{
  if(!buffer)
  {
    return error(result_void,
                 "Cannot add command to NULL pointed command buffer!");
  }

  result_command_ptr _ = command_new_pop_clip_rect();
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  command* cmd = _.value;
  result_void __ = command_buffer_add_command(buffer, cmd);
  if(!__.ok)
  {
    return __;
  }

  return ok_void();
}

result_void command_buffer_add_set_cursor_command(command_buffer* buffer,
                                                  command_type cursor_type)
{
  if(!buffer)
  {
    return error(result_void,
                 "Cannot add command to NULL pointed command buffer!");
  }

  result_command_ptr _ = command_new_set_cursor(cursor_type);
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  command* cmd = _.value;
  result_void __ = command_buffer_add_command(buffer, cmd);
  if(!__.ok)
  {
    return __;
  }

  return ok_void();
}

result_void command_buffer_add_clear_window_command(command_buffer* buffer)
{
  if(!buffer)
  {
    return error(result_void,
                 "Cannot add command to NULL pointed command buffer!");
  }

  result_command_ptr _ = command_new_clear_window();
  if(!_.ok)
  {
    return error(result_void, _.error);
  }

  command* cmd = _.value;
  result_void __ = command_buffer_add_command(buffer, cmd);
  if(!__.ok)
  {
    return __;
  }

  return ok_void();
}

result_command_ptr command_buffer_get_next_command(command_buffer* buffer)
{
  if(!buffer)
  {
    return error(
      result_command_ptr,
      "Cannot get next command from a NULL pointing command buffer!");
  }

  if(!buffer->length)
  {
    return error(result_command_ptr,
                 "Command buffer is empty, cannot get next command!");
  }

  command_node* cmd_node = buffer->head;
  command* cmd = cmd_node->cmd;
  buffer->head = cmd_node->next;
  buffer->length -= 1;

  // shallow freeing command node
  command_node_shallow_free(cmd_node);

  // need to free cmd externally
  return ok(result_command_ptr, cmd);
}

result_void command_buffer_clear_commands(command_buffer* buffer)
{
  if(!buffer)
  {
    return error(result_void,
                 "Cannot clear commands in NULL pointed command buffer!");
  }

  // TODO: Maybe we should use dynamic array
  // for storing these commands
  // as this linked list entirely depends on speed
  // of random access of memory

  if(!buffer->head)
  {
    return ok_void();
  }

  command_node* prev_cmd_node = buffer->head;
  command_node* cmd_node = prev_cmd_node->next;

  while(cmd_node)
  {
    result_void _ = command_node_free(prev_cmd_node);
    if(!_.ok)
    {
      return _;
    }

    prev_cmd_node = cmd_node;
    cmd_node = cmd_node->next;
  }
  result_void _ = command_node_free(prev_cmd_node);
  if(!_.ok)
  {
    return _;
  }

  buffer->head = NULL;
  buffer->tail = NULL;
  buffer->length = 0;

  return ok_void();
}

result_void command_buffer_free(command_buffer* buffer)
{
  if(!buffer)
  {
    return error(result_void, "Attempt to free a NULL pointed command buffer!");
  }

  if(buffer->head)
  {
    command_node* prev_cmd_node = buffer->head;
    command_node* cmd_node = prev_cmd_node->next;
    while(cmd_node)
    {
      result_void _ = command_node_free(prev_cmd_node);
      if(!_.ok)
      {
        return _;
      }

      prev_cmd_node = cmd_node;
      cmd_node = cmd_node->next;
    }
    result_void _ = command_node_free(prev_cmd_node);
    if(!_.ok)
    {
      return _;
    }
  }

  free(buffer);

  return ok_void();
}

static result_const_command_ptr
const_iterator_next_command(command_buffer_const_iterator* iterator)
{
  if(!iterator)
  {
    return error(result_const_command_ptr,
                 "Cannot get next command from NULL pointing const iterator of "
                 "command buffer!");
  }

  if(!iterator->private_data->node)
  {
    return error(result_const_command_ptr,
                 "Cannot get next command, as iterator is not good. Verify "
                 "const_iterator->good before calling for next command!");
  }

  const command* next_cmd = iterator->private_data->node->cmd;

  /// moving to next node
  iterator->private_data->node = iterator->private_data->node->next;
  if(!iterator->private_data->node)
  {
    iterator->good = false;
  }

  return ok(result_const_command_ptr, next_cmd);
}

result_command_buffer_const_iterator_ptr
command_buffer_const_iterator_new(const command_buffer* buffer)
{
  if(!buffer)
  {
    return error(
      result_command_buffer_const_iterator_ptr,
      "Cannot create const iterator for NULL pointing command buffer!");
  }

  command_buffer_const_iterator* iterator =
    (command_buffer_const_iterator*)calloc(
      1, sizeof(command_buffer_const_iterator));
  if(!iterator)
  {
    return error(
      result_command_buffer_const_iterator_ptr,
      "Unable to allocate memory for const iterator of command buffer!");
  }

  command_buffer_const_iterator_private_data* private_data =
    (command_buffer_const_iterator_private_data*)calloc(
      1, sizeof(command_buffer_const_iterator_private_data));
  if(!private_data)
  {
    free(iterator);
    return error(result_command_buffer_const_iterator_ptr,
                 "Unable to allocate memory for private data of const iterator "
                 "for command buffer!");
  }

  iterator->cmd_buffer = buffer;
  iterator->next_cmd = const_iterator_next_command;
  private_data->node = buffer->head;
  iterator->private_data = private_data;
  iterator->good = buffer->head ? true : false;

  return ok(result_command_buffer_const_iterator_ptr, iterator);
}

result_void
command_buffer_const_iterator_free(command_buffer_const_iterator* iterator)
{
  if(!iterator)
  {
    return error(
      result_void,
      "Attempt to free NULL pointing command buffer const iterator!");
  }

  /// freeing private data of iterator
  free(iterator->private_data);

  free(iterator);

  return ok_void();
}
