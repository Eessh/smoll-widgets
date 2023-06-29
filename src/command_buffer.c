#include "../include/command_buffer.h"
#include <stdio.h>

result_command_ptr command_new() {
  command* cmd = (command*)calloc(1, sizeof(command));
  if (!cmd) {
    return (result_command_ptr) {
      .ok = false,
      .error = "Unable to allocate memory for command!"
    };
  }

  cmd->type = RENDER_RECT;
  cmd->bounding_rect = (rect){0, 0, 0, 0};
  cmd->rect_color = (color){0, 0, 0, 255};
  cmd->text = NULL;
  cmd->text_color = (color){0, 0, 0, 255};

  return (result_command_ptr) {
    .ok = true,
    .value = cmd
  };
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

  return (result_command_ptr) {
    .ok = true,
    .value = cmd
  };
}

result_command_ptr command_new_render_text(const char* text, const color text_color, const rect bounding_rect, const color rect_color) {
  if (!text) {
    return (result_command_ptr) {
      .ok = false,
      .error = "Cannot create command for rendering text, with text pointing to NULL!"
    };
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

  return (result_command_ptr) {
    .ok = true,
    .value = cmd
  };
}

result_void command_free(command* cmd) {
  if (!cmd) {
    return (result_void) {
      .ok = false,
      .error = "Attempt to free a NULL pointed command!"
    };
  }

  free(cmd);

  return (result_void) {
    .ok = true,
    .error = NULL
  };
}

result_command_buffer_ptr command_buffer_new() {
  command_buffer* buffer = (command_buffer*)calloc(1, sizeof(command_buffer));
  if (!buffer) {
    return (result_command_buffer_ptr) {
      .ok = false,
      .error = "Unable to allocate memory for command buffer!"
    };
  }

  buffer->head = NULL;
  buffer->tail = NULL;

  return (result_command_buffer_ptr) {
    .ok = true,
    .value = buffer
  };
}

result_void command_buffer_add_command(command_buffer* buffer, command* cmd) {
  if (!buffer) {
    return (result_void) {
      .ok = false,
      .error = "Cannot add command to NULL pointed command buffer!"
    };
  }

  if (!cmd) {
    return (result_void) {
      .ok = false,
      .error = "Cannot add NULL pointed command to command buffer!"
    };
  }

  if (!buffer->head) {
    buffer->head = cmd;
    buffer->tail = cmd;

    return (result_void) {
      .ok = true,
      .error = NULL
    };
  }

  buffer->tail->next = cmd;
  buffer->tail = cmd;

  return (result_void) {
    .ok = true,
    .error = NULL
  };
}

result_void command_buffer_add_render_rect_command(command_buffer* buffer, const rect bounding_rect, const color rect_color) {
  if (!buffer) {
    return (result_void) {
      .ok = false,
      .error = "Cannot add command to NULL pointed command buffer!"
    };
  }

  result_command_ptr _ = command_new_render_rect(bounding_rect, rect_color);
  if (!_.ok) {
    return (result_void) {
      .ok = false,
      .error = _.error
    };
  }

  command* cmd = _.value;
  result_void __ = command_buffer_add_command(buffer, cmd);
  if (!__.ok) {
    return __;
  }

  return (result_void) {
    .ok = true,
    .error = NULL
  };
}

result_void command_buffer_add_render_text_command(command_buffer* buffer, const char* text, const color text_color, const rect bounding_rect, const color rect_color) {
  if (!buffer) {
    return (result_void) {
      .ok = false,
      .error = "Cannot add command to NULL pointed command buffer!"
    };
  }

  if (!text) {
    return (result_void) {
      .ok = false,
      .error = "Cannot add a render text command, with text pointing to NULL!"
    };
  }

  result_command_ptr _ = command_new_render_text(text, text_color, bounding_rect, rect_color);
  if (!_.ok) {
    return (result_void) {
      .ok = false,
      .error = _.error
    };
  }

  command* cmd = _.value;
  result_void __ = command_buffer_add_command(buffer, cmd);
  if (!__.ok) {
    return __;
  }

  return (result_void) {
    .ok = true,
    .error = NULL
  };
}

result_void command_buffer_process_commands(command_buffer* buffer) {
  // TODO: Command buffer process commands

  return (result_void) {
    .ok = false,
    .error = "Un-implemented!"
  };
}

result_void command_buffer_free(command_buffer* buffer) {
  if (!buffer) {
    return (result_void) {
      .ok = false,
      .error = "Attempt to free a NULL pointed command buffer!"
    };
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

  return (result_void) {
    .ok = true,
    .error = NULL
  };
}