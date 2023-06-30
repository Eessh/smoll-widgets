#include "../include/smoll_context.h"
#include "../include/base_widget.h"
#include "../include/command_buffer.h"
#include "../include/macros.h"
#include "SDL2/SDL.h"
#include <stdlib.h>

struct smoll_context {
  base_widget* root;

  base_widget* overlay_widget;
  base_widget* active_scrollbar;
  base_widget* keyboard_focused_widget;
  base_widget* mouse_focused_widget;

  uint16 global_mouse_x, global_mouse_y;

  uint16 viewport_w, viewport_h;

  char* font;
  uint8 font_size;

  command_buffer* cmd_buffer;
};

result_smoll_context_ptr smoll_context_create() {
  smoll_context* context = (smoll_context*)calloc(1, sizeof(smoll_context));
  if (!context) {
    return error(result_smoll_context_ptr, "Unable to allocate memory for smoll context!");
  }

  context->root = NULL;

  context->overlay_widget = NULL;
  context->active_scrollbar = NULL;
  context->keyboard_focused_widget = NULL;
  context->mouse_focused_widget = NULL;

  context->global_mouse_x = 0;
  context->global_mouse_y = 0;

  context->viewport_w = 0;
  context->viewport_h = 0;

  context->font = NULL;
  context->font_size = 0;

  result_command_buffer_ptr _ = command_buffer_new();
  if (!_.ok) {
    return error(result_smoll_context_ptr, _.error);
  }

  context->cmd_buffer = _.value;

  return ok(result_smoll_context_ptr, context);
}

result_void base_widget_recursive_free(base_widget* widget) {
  if (!widget) {
    return error(result_void, "Attempt to free a NULL pointed base widget!");
  }

  if (widget->children_head) {
    base_widget_child_node* prev_node = widget->children_head;
    base_widget_child_node* node = prev_node->next;
    while (node) {
      result_void _ = base_widget_recursive_free(prev_node->child);
      if (!_.ok) {
        return _;
      }
      _ = base_widget_child_node_free(prev_node);
      if (!_.ok) {
        return _;
      }
      prev_node = node;
      node = node->next;
    }
    result_void _ = base_widget_recursive_free(prev_node->child);
    if (!_.ok) {
      return _;
    }
    _ = base_widget_child_node_free(prev_node);
    if (!_.ok) {
      return _;
    }
  }

  result_void _ = base_widget_free(widget);
  if (!_.ok) {
    return _;
  }

  return ok_void();
}

result_void smoll_context_destroy(smoll_context* context) {
  if (!context) {
    return error(result_void, "Attempt to free a NULL pointed smoll context!");
  }

  if (context->root) {
    result_void _ = base_widget_recursive_free(context->root);
    if (!_.ok) {
      return _;
    }
  }

  result_void _ = command_buffer_free(context->cmd_buffer);
  if (!_.ok) {
    return _;
  }

  free(context);

  return ok_void();
}

result_base_widget_ptr smoll_context_get_mouse_focused_widget(const smoll_context* context) {
  if (!context) {
    return error(result_base_widget_ptr, "Cannot get mouse focused widget of smoll context, with context pointing to NULL!");
  }

  return ok(result_base_widget_ptr, context->mouse_focused_widget);
}

result_const_char_ptr smoll_context_get_font(const smoll_context* context) {
  if (!context) {
    return error(result_const_char_ptr, "Cannot get font from NULL pointed context!");
  }

  if (!context->font) {
    return error(result_const_char_ptr, "No font is loaded in the context!");
  }

  return ok(result_const_char_ptr, context->font);
}

result_uint8 smoll_context_get_font_size(const smoll_context* context) {
  if (!context) {
    return error(result_uint8, "Cannot get font size from NULL pointed context!");
  }

  if (!context->font) {
    return error(result_uint8, "No font is loaded in the context!");
  }

  return ok(result_uint8, context->font_size);
}

result_command_buffer_ptr smoll_context_get_command_buffer(const smoll_context* context) {
  if (!context) {
    return error(result_command_buffer_ptr, "Cannot get command buffer from a NULL pointed context!");
  }

  return ok(result_command_buffer_ptr, context->cmd_buffer);
}

result_void smoll_context_set_mouse_focused_widget(smoll_context* context, base_widget* widget) {
  if (!context) {
    return error(result_void, "Cannot set mouse focused of smoll context, with context pointing to NULL!");
  }

  context->mouse_focused_widget = widget;

  return ok_void();
}
