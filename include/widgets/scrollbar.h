#ifndef SMOLL_WIDGETS__SCROLLBAR_H
#define SMOLL_WIDGETS__SCROLLBAR_H

#include "../base_widget.h"

/// Scrollbar widget descriptor.
typedef struct scrollbar_descriptor
{
  /// Cross Axis Width corresponds to:
  /// - Width of scrollbar, if it is a vertical scrollbar.
  /// - Height of scrollbar, if it is a horizontal scrollbar.
  uint8 cross_axis_width;

  /// Uniform spacing between scrollbar track and thumb.
  uint8 padding;

  /// Color of scrollbar thumb.
  color foreground;

  /// Color of scrollbar track.
  color background;
} scrollbar_descriptor;

/// Default scrollbar widget descriptor.
/// This will be used if `NULL` is passed as argument for
/// parameter descriptor in function `scrollbar_new()`.
static const scrollbar_descriptor default_scrollbar_descriptor = {
  .cross_axis_width = 10,
  .padding = 2,
  .foreground = {.r = 174, .g = 175, .b = 177, .a = 255},
  .background = {.r = 53, .g = 57, .b = 61, .a = 255}};

/// Scrollbar target widget descriptor.
typedef struct scrollbar_target_descriptor
{
  /// Pointer to base widget of scrollbar's target.
  base_widget* base;

  /// Content length of scrollbar's target.
  uint32 content_length;
} scrollbar_target_descriptor;

typedef struct scrollbar_private scrollbar_private;

/// Scrollbar widget.
typedef struct scrollbar
{
  /// Descriptor for this scrollbar widget.
  scrollbar_descriptor descriptor;

  /// Pointer to base widget of this scrollbar.
  base_widget* base;

  /// Pointer to private data of this scrollbar.
  scrollbar_private* private_data;
} scrollbar;

/// Scrollbar pointer result.
typedef struct result_scrollbar_ptr
{
  bool ok;
  union
  {
    scrollbar* value;
    const char* error;
  };
} result_scrollbar_ptr;

result_scrollbar_ptr
scrollbar_new(base_widget* parent_base,
              const scrollbar_descriptor* const descriptor,
              const scrollbar_target_descriptor* const target_descriptor);

result_scrollbar_ptr scrollbar_new_with_debug_name(
  base_widget* parent_base,
  const scrollbar_descriptor* const descriptor,
  const scrollbar_target_descriptor* const target_descriptor,
  const char* debug_name);

#endif