#include "../include/internal_context.h"
#include <stdlib.h>
#include "../include/command_buffer.h"
#include "../include/macros.h"

result_internal_context_ptr internal_context_create()
{
  internal_context* context =
    (internal_context*)calloc(1, sizeof(internal_context));
  if(!context)
  {
    return error(result_internal_context_ptr,
                 "Unable to allocate memory for internal context!");
  }

  context->mouse_x = 0;
  context->mouse_y = 0;

  context->global_mouse_x = 0;
  context->global_mouse_y = 0;

  context->viewport_w = 0;
  context->viewport_h = 0;

  context->root = NULL;
  context->overlay_widget = NULL;
  context->active_scrollbar = NULL;
  context->keyboard_focused_widget = NULL;
  context->mouse_focused_widget = NULL;

  // creating command buffer
  result_command_buffer_ptr _ = command_buffer_new();
  if(!_.ok)
  {
    return error(result_internal_context_ptr, _.error);
  }
  context->cmd_buffer = _.value;

  context->backend = NULL;
}

result_void recursively_free_ui_tree(base_widget* root)
{
  if(!root)
  {
    return error(result_void, "Attempt to free a NULL pointed base widget!");
  }

  if(root->children_head)
  {
    base_widget_child_node* prev_node = root->children_head;
    base_widget_child_node* node = prev_node->next;
    while(node)
    {
      result_void _ = recursively_free_ui_tree(prev_node->child);
      if(!_.ok)
      {
        return _;
      }
      _ = base_widget_child_node_free(prev_node);
      if(!_.ok)
      {
        return _;
      }
      prev_node = node;
      node = node->next;
    }
    result_void _ = recursively_free_ui_tree(prev_node->child);
    if(!_.ok)
    {
      return _;
    }
    _ = base_widget_child_node_free(prev_node);
    if(!_.ok)
    {
      return _;
    }
  }

  result_void _ = base_widget_free(root);
  if(!_.ok)
  {
    return _;
  }

  return ok_void();
}

result_void internal_context_destroy(internal_context* context)
{
  if(!context)
  {
    return error(result_void,
                 "Attempt to free a NULL pointed internal context!");
  }

  // ignoring errors while freeing UI tree
  result_void _ = recursively_free_ui_tree(context->root);

  // ignoring errors while freeing comand buffer
  _ = command_buffer_free(context->cmd_buffer);

  free(context);

  return ok_void();
}