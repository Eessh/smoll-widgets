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

typedef struct render_backend
{
  const char8* name;
  struct
  {
    uint8 major, minor, patch;
  } version;

  result_void (*load_font)(const char* font_name, uint8 font_size);

  result_text_dimensions (*get_text_dimensions)(const char* text,
                                                const char* font_name,
                                                uint8 font_size);

  result_void (*process_command)(const command* cmd);
} render_backend;

typedef struct result_backend_ptr
{
  bool ok;
  union
  {
    render_backend* value;
    const char* error;
  };
} result_backend_ptr;

#endif