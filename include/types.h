#ifndef SMOLL_WIDGETS__TYPES_H
#define SMOLL_WIDGETS__TYPES_H

#include <stdbool.h>
#include <stdint.h>

/// @brief 8-bit (1-byte) character,
///        Range: (-128, 127)
typedef char char8;

/// @brief 8-bit (1-byte) unsigned character,
///        Range: (0, 255)
typedef unsigned char uchar8;

/// @brief 8-bit (1-byte) integer,
///        Range: (-128, 127)
typedef int8_t int8;

/// @brief 8-bit (1-byte) unsigned integer,
///        Range: (0, 255)
typedef uint8_t uint8;

/// @brief 16-bit (2-bytes) integer,
///        Range: (-32768, 32767)
typedef int16_t int16;

/// @brief 16-bit (2-bytes) unsigned integer,
///        Range: (0, 65535)
typedef uint16_t uint16;

/// @brief 32-bit (4-bytes) integer,
///        Range: (-2147483648, 2147483647)
typedef int32_t int32;

/// @brief 32-bit (4-bytes) unsigned integer,
///        Range: (0, 4294967295)
typedef uint32_t uint32;

/// @brief 64-bit (8-bytes) integer,
///        Range: (-9223372036854775808, 9223372036854775807)
typedef int64_t int64;

/// @brief 64-bit (8-bytes) unsigned integer,
///        Range: (0, 18446744073709551615)
typedef uint64_t uint64;

/// @brief 32-bit (4-bytes) float,
///        Range: (3.4e-38, 3.4e+38)
typedef float float32;

/// @brief 64-bit (8-bytes) float (double),
///        Range: (1.7e-308, 1.7e+308)
typedef double float64;

/// @brief 80-bit (10-bytes) float (long double),
///        Range: (3.4e-4932, 3.4e+4932)
typedef long double float80;

/// @brief Rectange with top-left as origin
typedef struct rect
{
  int16 x, y;
  uint16 w, h;
} rect;

/// @brief Color with RGBA,
///        each component ranges from [0, 255]
typedef struct color
{
  uint8 r, g, b, a;
} color;

typedef struct result_void
{
  bool ok;
  const char* error;
} result_void;

typedef struct result_bool
{
  bool ok;
  union
  {
    bool value;
    const char* error;
  };
} result_bool;

typedef struct result_char
{
  bool ok;
  union
  {
    char value;
    const char* error;
  };
} result_char;

typedef struct result_uchar
{
  bool ok;
  union
  {
    unsigned char value;
    const char* error;
  };
} result_uchar;

typedef struct result_char_ptr
{
  bool ok;
  union
  {
    char* value;
    const char* error;
  };
} result_char_ptr;

typedef struct result_const_char_ptr
{
  bool ok;
  union
  {
    const char* value;
    const char* error;
  };
} result_const_char_ptr;

typedef struct result_int8
{
  bool ok;
  union
  {
    int8 value;
    const char* error;
  };
} result_int8;

typedef struct result_uint8
{
  bool ok;
  union
  {
    uint8 value;
    const char* error;
  };
} result_uint8;

typedef struct result_int16
{
  bool ok;
  union
  {
    int16 value;
    const char* error;
  };
} result_int16;

typedef struct result_uint16
{
  bool ok;
  union
  {
    uint16 value;
    const char* error;
  };
} result_uint16;

typedef struct result_int32
{
  bool ok;
  union
  {
    int32 value;
    const char* error;
  };
} result_int32;

typedef struct result_uint32
{
  bool ok;
  union
  {
    uint32 value;
    const char* error;
  };
} result_uint32;

typedef struct result_int64
{
  bool ok;
  union
  {
    int64 value;
    const char* error;
  };
} result_int64;

typedef struct result_uint64
{
  bool ok;
  union
  {
    uint64 value;
    const char* error;
  };
} result_uint64;

typedef struct result_float32
{
  bool ok;
  union
  {
    float32 value;
    const char* error;
  };
} result_float32;

typedef struct result_float64
{
  bool ok;
  union
  {
    float64 value;
    const char* error;
  };
} result_float64;

typedef struct result_float80
{
  bool ok;
  union
  {
    float80 value;
    const char* error;
  };
} result_float80;

typedef struct result_rect
{
  bool ok;
  union
  {
    rect value;
    const char* error;
  };
} result_rect;

typedef struct result_color
{
  bool ok;
  union
  {
    color value;
    const char* error;
  };
} result_color;

#endif