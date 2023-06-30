#include "../include/command_buffer.h"
#include "../include/macros.h"
#include <stdlib.h>

result_command_ptr command_new() {
  command* cmd = (command*)calloc(1, sizeof(command));
  if (!cmd) {
    return error(result_command_ptr, "Unable to allocate memory for command!");
  }

  cmd->type = RENDER_RECT;
  cmd->bounding_rect = (rect){0, 0, 0, 0};
  cmd->rect_color = (color){0, 0, 0, 255};
  cmd->text = NULL;
  cmd->text_color = (color){0, 0, 0, 255};

  return ok(result_command_ptr, cmd);
}

result_command_ptr command_new_render_rect(const rect bounding_rect, const color rect_color) {
  result_command_ptr _ = command_new();
  if (!_.ok) {
    return _;
  }

  command* cmd = _.value;
  cmd->type = RENDER_RECT;
  cmd->bounding_rect = bounding_rect;
  cmd->rect_color = rect_color;

  return ok(result_command_ptr, cmd);
}

result_command_ptr command_new_render_text(const char* text, const color text_color, const rect bounding_rect, const color rect_color) {
  if (!text) {
    return error(result_command_ptr, "Cannot create command for rendering text, with text pointing to NULL!");
  }

  result_command_ptr _ = command_new();
  if (!_.ok) {
    return _;
  }

  command* cmd = _.value;
  cmd->bounding_rect = bounding_rect;
  cmd->rect_color = rect_color;
  cmd->text = text;
  cmd->text_color = text_color;

  return ok(result_command_ptr, cmd);
}

result_void command_free(command* cmd) {
  if (!cmd) {
    return error(result_void, "Attempt to free a NULL pointed command!");
  }

  free(cmd);

  return ok_void();
}

result_command_buffer_ptr command_buffer_new() {
  command_buffer* buffer = (command_buffer*)calloc(1, sizeof(command_buffer));
  if (!buffer) {
    return error(result_command_buffer_ptr, "Unable to allocate memory for command buffer!");
  }

  buffer->head = NULL;
  buffer->tail = NULL;
  buffer->length = 0;

  return ok(result_command_buffer_ptr, buffer);
}

result_void command_buffer_add_command(command_buffer* buffer, command* cmd) {
  if (!buffer) {
    return error(result_void, "Cannot add command to NULL pointed command buffer!");
  }

  if (!cmd) {
    return error(result_void, "Cannot add NULL pointed command to command buffer!");
  }

  buffer->length += 1;

  if (!buffer->head) {
    buffer->head = cmd;
    buffer->tail = cmd;

    return ok_void();
  }

  buffer->tail->next = cmd;
  buffer->tail = cmd;

  return ok_void();
}

result_void command_buffer_add_render_rect_command(command_buffer* buffer, const rect bounding_rect, const color rect_color) {
  if (!buffer) {
    return error(result_void, "Cannot add command to NULL pointed command buffer!");
  }

  result_command_ptr _ = command_new_render_rect(bounding_rect, rect_color);
  if (!_.ok) {
    return error(result_void, _.error);
  }

  command* cmd = _.value;
  result_void __ = command_buffer_add_command(buffer, cmd);
  if (!__.ok) {
    return __;
  }

  return ok_void();
}

result_void command_buffer_add_render_text_command(command_buffer* buffer, const char* text, const color text_color, const rect bounding_rect, const color rect_color) {
  if (!buffer) {
    return error(result_void, "Cannot add command to NULL pointed command buffer!");
  }

  if (!text) {
    return error(result_void, "Cannot add a render text command, with text pointing to NULL!");
  }

  result_command_ptr _ = command_new_render_text(text, text_color, bounding_rect, rect_color);
  if (!_.ok) {
    return error(result_void, _.error);
  }

  command* cmd = _.value;
  result_void __ = command_buffer_add_command(buffer, cmd);
  if (!__.ok) {
    return __;
  }

  return ok_void();
}

result_uint16 command_buffer_get_length(const command_buffer* buffer) {
  if (!buffer) {
    return error(result_uint16, "Cannot get length of NULL pointing command buffer!");
  }

  return ok(result_uint16, buffer->length);
}

result_void command_buffer_process_commands(command_buffer* buffer) {
  // TODO: Command buffer process commands

  return error(result_void, "Un-implemented!");
}

result_void command_buffer_clear_commands(command_buffer* buffer) {
  if (!buffer) {
    return error(result_void, "Cannot clear commands in NULL pointed command buffer!");
  }

  // TODO: Maybe we should use dynamic array
  // for storing these commands
  // as this linked list entirely depends on speed
  // of random access of memory

  if (!buffer->head) {
    return ok_void();
  }

  command* prev_cmd = buffer->head;
  command* cmd = prev_cmd->next;

  while (cmd) {
    result_void _ = command_free(prev_cmd);
    if (!_.ok) {
      return _;
    }

    prev_cmd = cmd;
    cmd = cmd->next;
  }
  result_void _ = command_free(prev_cmd);
  if (!_.ok) {
    return _;
  }

  buffer->head = NULL;
  buffer->tail = NULL;

  return ok_void();
}

result_void command_buffer_free(command_buffer* buffer) {
  if (!buffer) {
    return error(result_void, "Attempt to free a NULL pointed command buffer!");
  }

  if (buffer->head) {
    command* prev_cmd = buffer->head;
    command* cmd = prev_cmd->next;
    while (cmd) {
      result_void _ = command_free(prev_cmd);
      if (!_.ok) {
        return _;
      }

      prev_cmd = cmd;
      cmd = cmd->next;
    }
    result_void _ = command_free(prev_cmd);
    if (!_.ok) {
      return _;
    }
  }

  free(buffer);

  return ok_void();
}