#ifndef SMOLL_WIDGETS__BACKEND_H
#define SMOLL_WIDGETS__BACKEND_H

#include "command_buffer.h"
#include "types.h"

typedef struct text_dimensions
{
  uint16 w, h;
} text_dimensions;

typedef struct result_text_dimensions
{
  bool ok;
  union
  {
    text_dimensions value;
    const char* error;
  };
} result_text_dimensions;

typedef struct version
{
  uint8 major, minor, patch;
} version;

typedef struct render_backend
{
  char8* name;

  version backend_version;

  bool supports_curve_rendering;

  result_void (*load_font)(const char* font_name, uint8 font_size);

  result_text_dimensions (*get_text_dimensions)(const char* text,
                                                const char* font_name,
                                                uint8 font_size);

  /// Process command.
  /// Use this api to update UI from each command.
  result_void (*process_command)(const command* cmd);

  /// Process command buffer.
  /// Use this api to update UI from whole command buffer.
  /// This function is useful for backend implementations, when they need to
  /// make optimizations for rendering all update commands at once.
  result_void (*process_command_buffer)(const command_buffer* cmd_buffer);
} render_backend;

typedef struct result_render_backend_ptr
{
  bool ok;
  union
  {
    render_backend* value;
    const char* error;
  };
} result_render_backend_ptr;

#endif