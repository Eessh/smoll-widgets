#include "../../include/widgets/view.h"
#include <stdlib.h>
#include "../../include/macros.h"

struct view_private
{
  view_layout_type layout_type;
};

result_view_ptr view_new(base_widget* parent_base, view_layout_type layout_type)
{
  view* v = (view*)calloc(1, sizeof(view));
  if(!v)
  {
    return error(result_view_ptr, "Unable to allocate memory for view widget!");
  }

  result_base_widget_ptr _ = base_widget_new();
  if(!_.ok)
  {
    free(v);
    return error(result_view_ptr, _.error);
  }

  view_private* v_private = (view_private*)calloc(1, sizeof(view_private));
  if(!v_private)
  {
    free(v);
    base_widget_free(_.value);
    return error(result_view_ptr,
                 "Unable to allocate memory for view widget's private fileds!");
  }

  v->base = _.value;
  v->base->derived = v;
  v->base->parent = parent_base;

  v->private_data = v_private;

  // attaching this button to parent's children list
  // and assigning context
  if(parent_base)
  {
    base_widget_add_child(parent_base, v->base);
    v->base->context = parent_base->context;
  }
}

result_void view_set_layout_type(view* v, view_layout_type layout_type);