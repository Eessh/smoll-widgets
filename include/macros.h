#ifndef SMOLL_WIDGETS__MACROS_H
#define SMOLL_WIDGETS__MACROS_H

/// @brief Ok return type for result_void
#define ok_void() (result_void) { .ok = true, .error = NULL }

/// @brief Ok return type
#define ok(result_type, result_value) (result_type) { .ok = true, .value = result_value }

/// @brief Error return type
#define error(result_type, result_error) (result_type) { .ok = false, .error = result_error }

#endif