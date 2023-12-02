#ifndef SMOLL_WIDGETS__LABEL_H
#define SMOLL_WIDGETS__LABEL_H

#include "../base_widget.h"

typedef struct label_private label_private;

typedef struct label
{
  base_widget* base;

  label_private* private_data;
} label;

typedef struct result_label_ptr
{
  bool ok;
  union
  {
    label* value;
    const char* error;
  };
} result_label_ptr;

result_label_ptr label_new(base_widget* parent_base, const char* text);

result_label_ptr label_new_with_debug_name(base_widget* parent_base,
                                           const char* text,
                                           const char* debug_name);

result_const_char_ptr label_get_text(const label* l);

result_bool label_set_text(label* l, const char* text);

result_color label_get_color(const label* l);

result_bool label_set_color(label* l, const color* c);

#endif