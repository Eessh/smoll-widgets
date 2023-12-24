#ifndef SMOLL_WIDGETS__COMMAND_BUFFER_H
#define SMOLL_WIDGETS__COMMAND_BUFFER_H

#include "types.h"

/// Command Types
typedef enum command_type
{
  /// Command for rendering line.
  /// Needed data: line start, end points and line color.
  RENDER_LINE,

  /// Command for rendering text.
  /// Needed data: text, text top-left point, text color, font and font-size.
  RENDER_TEXT,

  /// Command for rendering rectangle (filled).
  /// Needed data: bounding rect, rect color.
  RENDER_RECT,

  /// Command for rendering rounded rectangle (filled).
  /// Needed data: bounding rect, border radius, rect color.
  RENDER_ROUNDED_RECT,

  /// Command for rendering rectangle (outlined).
  /// Needed data: bounding rect, rect outline color.
  RENDER_RECT_OUTLINED,

  /// Command for pushing clip rect.
  /// Needed data: bounding rect for clipping.
  PUSH_CLIP_RECT,

  /// Command for popping last added clip rect.
  /// Needed data: NONE
  POP_CLIP_RECT,

  SET_CURSOR_ARROW,
  SET_CURSOR_IBEAM,
  SET_CURSOR_MOVE,
  SET_CURSOR_CROSSHAIR,
  SET_CURSOR_RESIZE_LEFT_RIGHT,
  SET_CURSOR_RESIZE_TOP_LEFT__BOTTOM_RIGHT,
  SET_CURSOR_RESIZE_TOP_RIGHT__BOTTOM_LEFT,
  SET_CURSOR_RESIZE_TOP_BOTTOM,
  SET_CURSOR_HAND,
  SET_CURSOR_PROCESSING,
  SET_CURSOR_LOADING,
  SET_CURSOR_PROHIBITED,

  CLEAR_WINDOW
} command_type;

/// Data for `RENDER_RECT` command.
typedef struct render_rect_data
{
  rect bounding_rect;
  color rect_color;
} render_rect_data;

typedef struct render_rounded_rect_data
{
  rect bounding_rect;
  uint8 border_radius;
  color rect_color;
} render_rounded_rect_data;

/// Data for `RENDER_LINE` command.
typedef struct render_line_data
{
  point begin, end;
} render_line_data;

/// Data for `RENDER_TEXT` command.
typedef struct render_text_data
{
  const char* text;
  color text_color;
  point text_coordinates;
} render_text_data;

/// Command.
/// Sent by widgets to command buffer, for processing by the backend.
typedef struct command
{
  /// Type of command.
  command_type type;

  union
  {
    /// Data of `RENDER_RECT` command.
    render_rect_data render_rect;

    render_rounded_rect_data render_rounded_rect;

    /// Data of `RENDER_LINE` command.
    render_line_data render_line;

    /// Data of `RENDER_TEXT` command.
    render_text_data render_text;
    rect clip_rect;
  } data;
} command;

/// Command pointer result.
typedef struct result_command_ptr
{
  bool ok;
  union
  {
    command* value;
    const char* error;
  };
} result_command_ptr;

typedef struct result_const_command_ptr
{
  bool ok;
  union
  {
    const command* value;
    const char* error;
  };
} result_const_command_ptr;

typedef struct command_node command_node;

typedef struct command_buffer command_buffer;

/// Command buffer pointer result.
typedef struct result_command_buffer_ptr
{
  bool ok;
  union
  {
    command_buffer* value;
    const char* error;
  };
} result_command_buffer_ptr;

typedef struct command_buffer_const_iterator_private_data
  command_buffer_const_iterator_private_data;

typedef struct command_buffer_const_iterator
{
  const command_buffer* cmd_buffer;
  bool good;

  result_const_command_ptr (*next_cmd)(
    struct command_buffer_const_iterator* const_iterator);

  command_buffer_const_iterator_private_data* private_data;
} command_buffer_const_iterator;

typedef struct result_command_buffer_const_iterator_ptr
{
  bool ok;
  union
  {
    command_buffer_const_iterator* value;
    const char* error;
  };
} result_command_buffer_const_iterator_ptr;

///////////////////////////////////////////////////////////////////////////////
/// * Command functions.
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief      Creates a new `RENDER_RECT` command.
 *
 * @param[in]  bounding_rect  bounding rectangle
 * @param[in]  rect_color     rectangle color
 *
 * @return     Command pointer result.
 */
result_command_ptr command_new_render_rect(const rect bounding_rect,
                                           const color rect_color);

result_command_ptr command_new_render_rounded_rect(const rect bounding_rect,
                                                   const uint8 border_radius,
                                                   const color rect_color);

/**
 * @brief      Creates a new `RENDER_RECT_OUTLINED` command.
 *
 * @param[in]  bounding_rect       bounding rectangle
 * @param[in]  rect_outline_color  rectangle outline color
 *
 * @return     Command pointer result.
 */
result_command_ptr
command_new_render_rect_outlined(const rect bounding_rect,
                                 const color rect_outline_color);

/**
 * @brief      Creates a new `RENDER_LINE` command.
 *
 * @param[in]  begin  begining point of line.
 * @param[in]  end    ending point of line.
 *
 * @return     Command pointer result.
 */
result_command_ptr command_new_render_line(point begin, point end);

/**
 * @brief      Creates a new `RENDER_TEXT` command.
 *
 * @param[in]  text              text to render.
 * @param[in]  text_color        text color
 * @param[in]  text_coordinates  text top-left coordinates.
 *
 * @return     Command pointer result.
 */
result_command_ptr command_new_render_text(const char* text,
                                           const color text_color,
                                           point text_coordinates);

/**
 * @brief      Creates a new `PUSH_CLIP_RECT` command.
 *
 * @param[in]  clip_rect  the clip rectangle.
 *
 * @return     Command pointer result.
 */
result_command_ptr command_new_push_clip_rect(rect clip_rect);

/**
 * @brief      Creates a new `POP_CLIP_RECT` command.
 *
 * @return     Command pointer result.
 */
result_command_ptr command_new_pop_clip_rect();

/**
 * @brief      Creates a new set cursor command.
 *
 * @param[in]  cursor_type  the cursor command type.
 *
 * @return     Command pointer result.
 */
result_command_ptr command_new_set_cursor(command_type cursor_type);

result_command_ptr command_new_clear_window();

/**
 * @brief      Frees command.
 *
 * @param      cmd   the command to free.
 *
 * @return     Void result.
 */
result_void command_free(command* cmd);

///////////////////////////////////////////////////////////////////////////////
/// * Command buffer functions.
///////////////////////////////////////////////////////////////////////////////

/// Creates a new command buffer.
///
/// Returns command buffer pointer result (`result_command_buffer_ptr`).
result_command_buffer_ptr command_buffer_new();

/// Gives length of command buffer.
/// Returns `-1` if pointer to buffer is `NULL`.
int16 command_buffer_length(const command_buffer* buffer);

/// Adds the given command to the command buffer.
///
/// Returns void result (`result_void`).
result_void command_buffer_add_command(command_buffer* buffer, command* cmd);

/// Adds `RENDER_RECT` command to the command buffer.
///
/// Returns void result (`result_void`).
result_void command_buffer_add_render_rect_command(command_buffer* buffer,
                                                   const rect bounding_rect,
                                                   const color rect_color);

result_void
command_buffer_add_render_rounded_rect_command(command_buffer* buffer,
                                               const rect bounding_rect,
                                               const uint8 border_radius,
                                               const color rect_color);

/// Adds `RENDER_RECT_OUTLINED` command to the command buffer.
///
/// Returns void result (`result_void`).
result_void
command_buffer_add_render_rect_outline_command(command_buffer* buffer,
                                               const rect bounding_rect,
                                               const color rect_outline_color);

/// Adds `RENDER_LINE` command to the command buffer.
///
/// Returns void result (`result_void`).
result_void command_buffer_add_render_line_command(command_buffer* buffer,
                                                   point begin,
                                                   point end);

/// Adds `RENDER_TEXT` command to the command buffer.
///
/// Returns void result (`result_void`).
result_void command_buffer_add_render_text_command(command_buffer* buffer,
                                                   const char* text,
                                                   const color text_color,
                                                   point text_coordinates);

/// Adds `PUSH_CLIP_RECT` command to the command buffer.
///
/// Returns void result (`result_void`).
result_void command_buffer_add_push_clip_rect_command(command_buffer* buffer,
                                                      rect clip_rect);

/// Adds `POP_CLIP_RECT` command to the command buffer.
///
/// Returns void result (`result_void`).
result_void command_buffer_add_pop_clip_rect_command(command_buffer* buffer);

/// Adds set cursor commands to the command buffer.
///
/// Returns void result (`result_void`).
result_void command_buffer_add_set_cursor_command(command_buffer* buffer,
                                                  command_type cursor_type);

result_void command_buffer_add_clear_window_command(command_buffer* buffer);

/// Returns the next command in the command buffer.
/// It's the backend's responsibility to free the command
/// using `command_free()`.
///
/// Returns command pointer result (`result_command_ptr`).
result_command_ptr command_buffer_get_next_command(command_buffer* buffer);

/// Clears all commands in the command buffer.
/// Not just the nodes in the command buffer, but also frees the commands.
///
/// Returns void result (`result_void`).
result_void command_buffer_clear_commands(command_buffer* buffer);

/// Frees the command buffer.
result_void command_buffer_free(command_buffer* buffer);

result_command_buffer_const_iterator_ptr
command_buffer_const_iterator_new(const command_buffer* buffer);

result_void
command_buffer_const_iterator_free(command_buffer_const_iterator* iterator);

#endif
