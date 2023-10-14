#ifndef SMOLL_WIDGETS__CHECKBOX_H
#define SMOLL_WIDGETS__CHECKBOX_H

#include "../base_widget.h"

typedef struct checkbox_private checkbox_private;

typedef struct checkbox
{
  const char* debug_name;
  base_widget* base;

  checkbox_private* private_data;
} checkbox;

typedef struct result_checkbox_ptr
{
  bool ok;
  union
  {
    checkbox* value;
    const char* error;
  };
} result_checkbox_ptr;

result_checkbox_ptr checkbox_new(base_widget* parent_base, color foreground);

result_checkbox_ptr checkbox_new_with_debug_name(base_widget* parent_base,
                                                 color foreground,
                                                 const char* debug_name);

result_void checkbox_set_ticked_callback(checkbox* box,
                                         void (*callback)(checkbox*));

result_void checkbox_set_unticked_callback(checkbox* box,
                                           void (*callback)(checkbox*));

#endif