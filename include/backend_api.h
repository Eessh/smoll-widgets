#ifndef SMOLL_WIDGETS__API_H
#define SMOLL_WIDGETS__API_H

#include "types.h"
#include "command_buffer.h"

typedef struct text_coordinates {
  int16 x, h;
} text_coordinates;

typedef struct text_dimensions {
  uint16 w, h;
} text_dimensions;

typedef struct result_text_dimensions {
  bool ok;
  union {
    text_dimensions value;
    const char* error;
  };
} result_text_dimensions;

result_void backend_api_load_font(const char* font, const uint8 font_size);
result_text_dimensions backend_api_get_text_dimensions(const char* text, const char* font, const uint8 font_size);

result_void backend_api_render_text(const command* cmd);
result_void backend_api_render_rect(const command* cmd);

#endif