#ifndef SMOLL_WIDGETS__MACROS_H
#define SMOLL_WIDGETS__MACROS_H

#include "../log-boii/log_boii.h"

#ifdef DEBUG
#  define DEBUG_MODE 1
#else
#  define DEBUG_MODE 0
#endif

/// @brief Ok return type for result_void
#define ok_void()                                                              \
  (result_void)                                                                \
  {                                                                            \
    .ok = true, .error = NULL                                                  \
  }

/// @brief Ok return type
#define ok(result_type, result_value)                                          \
  (result_type)                                                                \
  {                                                                            \
    .ok = true, .value = result_value                                          \
  }

/// @brief Error return type
#define error(result_type, result_error)                                       \
  (result_type)                                                                \
  {                                                                            \
    .ok = false, .error = result_error                                         \
  }

#define max(a, b) (((a) > (b)) ? (a) : (b))

#define min(a, b) (((a) < (b)) ? (a) : (b))

/// @brief Macro ot get base widget of given widget.
#define base(widget) (widget->base)

#define trace(...)                                                             \
  do                                                                           \
  {                                                                            \
    if(DEBUG_MODE)                                                             \
      log_trace(__VA_ARGS__);                                                  \
  } while(0)

#define debug(...)                                                             \
  do                                                                           \
  {                                                                            \
    if(DEBUG_MODE)                                                             \
      log_debug(__VA_ARGS__);                                                  \
  } while(0)

#define info(...)                                                              \
  do                                                                           \
  {                                                                            \
    if(DEBUG_MODE)                                                             \
      log_info(__VA_ARGS__);                                                   \
  } while(0)

#define warn(...)                                                              \
  do                                                                           \
  {                                                                            \
    if(DEBUG_MODE)                                                             \
      log_warn(__VA_ARGS__);                                                   \
  } while(0)

//#define error(...)                                                             \
//  do                                                                           \
//  {                                                                            \
//    if(DEBUG_MODE)                                                             \
//      log_error(__VA_ARGS__);                                                   \
//  } while(0)

#define fatal(...)                                                             \
  do                                                                           \
  {                                                                            \
    if(DEBUG_MODE)                                                             \
      log_fatal(__VA_ARGS__);                                                  \
  } while(0)

#endif