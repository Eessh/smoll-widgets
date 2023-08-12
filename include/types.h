#ifndef SMOLL_WIDGETS__TYPES_H
#define SMOLL_WIDGETS__TYPES_H

#include <stdbool.h>
#include <stdint.h>

/**
 * 8-bit (1-byte) character, Range: (-128, 127)
 */
typedef char char8;

/**
 * 8-bit (1-byte) unsigned character, Range: (0, 255)
 */
typedef unsigned char uchar8;

/**
 * 8-bit (1-byte) integer, Range: (-128, 127)
 */
typedef int8_t int8;

/**
 * 8-bit (1-byte) unsigned integer, Range: (0, 255)
 */
typedef uint8_t uint8;

/**
 * 16-bit (2-bytes) integer, Range: (-32768, 32767)
 */
typedef int16_t int16;

/**
 * 16-bit (2-bytes) unsigned integer, Range: (0, 65535)
 */
typedef uint16_t uint16;

/**
 * 32-bit (4-bytes) integer, Range: (-2147483648, 2147483647)
 */
typedef int32_t int32;

/**
 * 32-bit (4-bytes) unsigned integer, Range: (0, 4294967295)
 */
typedef uint32_t uint32;

/**
 * 64-bit (8-bytes) integer,
 * Range: (-9223372036854775808, 9223372036854775807)
 */
typedef int64_t int64;

/**
 * 64-bit (8-bytes) unsigned integer,
 * Range: (0, 18446744073709551615)
 */
typedef uint64_t uint64;

/**
 * 32-bit (4-bytes) float, Range: (3.4e-38, 3.4e+38)
 */
typedef float float32;

/**
 * 64-bit (8-bytes) float(double), Range: (1.7e-308, 1.7e+308)
 */
typedef double float64;

/**
 * 80-bit (10-bytes) float(long double), Range: (3.4e-4932, 3.4e+4932)
 */
typedef long double float80;

/**
 * Point struct, contains coordinates of point with origin at top-left.
 */
typedef struct point
{
  /**
   * X-coordinate of point.
   */
  int16 x;

  /**
   * Y-coordinate of point.
   */
  int16 y;
} point;

/**
 * Rect struct, contains coordinates and size of rectangle with
 * origin at top-left.
 */
typedef struct rect
{
  /**
   * X-coordinate of top-left corner of rectangle.
   */
  int16 x;

  /**
   * Y-coordinate of top-left corner of rectangle.
   */
  int16 y;

  /**
   * Width of rectangle.
   */
  uint16 w;

  /**
   * Height of rectangle.
   */
  uint16 h;
} rect;

/**
 * Color with RGBA components, each component ranges from [0, 255].
 */
typedef struct color
{
  /**
   * Red color component.
   */
  uint8 r;

  /**
   * Green color component.
   */
  uint8 g;

  /**
   * Blue color component.
   */
  uint8 b;

  /**
   * Alpha color component.
   */
  uint8 a;
} color;

///////////////////////////////////////////////////////////////////////////////
/// * Result Types
///////////////////////////////////////////////////////////////////////////////

/**
 * Void result, contains no data. Just reports if there is an error.
 * 
 * @member ok - `true` if no errors. Use this to check if this is valid result.
 * 
 * @member error - pointer to error string. `NULL` if no errors.
 */
typedef struct result_void
{
  bool ok;
  const char* error;
} result_void;

/**
 * Bool result, contains `bool` value optionally.
 * 
 * @member ok - `true` if no errors. Use this to check if this is valid result.
 * 
 * @member value - the `bool` value.
 * 
 * @member error - pointer to error string. `NULL` if no errors.
 */
typedef struct result_bool
{
  bool ok;
  union
  {
    bool value;
    const char* error;
  };
} result_bool;

/**
 * Char result, contains `char` value optionally.
 * 
 * @member ok - `true` if no errors. Use this to check if this is valid result.
 * 
 * @member value - the `char` value.
 * 
 * @member error - pointer to error string. `NULL` if no errors.
 */
typedef struct result_char
{
  bool ok;
  union
  {
    char value;
    const char* error;
  };
} result_char;

/**
 * UChar result, contains `uchar` value optionally.
 * 
 * @member ok - `true` if no errors. Use this to check if this is valid result.
 * 
 * @member value - the `uchar` value.
 * 
 * @member error - pointer to error string. `NULL` if no errors.
 */
typedef struct result_uchar
{
  bool ok;
  union
  {
    unsigned char value;
    const char* error;
  };
} result_uchar;

/**
 * CharPtr result, contains `char*` optionally.
 * 
 * @member ok - `true` if no errors. Use this to check if this is valid result.
 * 
 * @member value - the `char*` value.
 * 
 * @member error - pointer to error string. `NULL` if no errors.
 */
typedef struct result_char_ptr
{
  bool ok;
  union
  {
    char* value;
    const char* error;
  };
} result_char_ptr;

/**
 * ConstCharPtr result, contains `const char*` value optionally.
 * 
 * @member ok - `true` if no errors. Use this to check if this is valid result.
 * 
 * @member value - the `const char*` value.
 * 
 * @member error - pointer to error string. `NULL` if no errors.
 */
typedef struct result_const_char_ptr
{
  bool ok;
  union
  {
    const char* value;
    const char* error;
  };
} result_const_char_ptr;

/**
 * Int8 result, contains `int8` value optionally.
 * 
 * @member ok - `true` if no errors. Use this to check if this is valid result.
 * 
 * @member value - the `int8` value.
 * 
 * @member error - pointer to error string. `NULL` if no errors.
 */
typedef struct result_int8
{
  bool ok;
  union
  {
    int8 value;
    const char* error;
  };
} result_int8;

/**
 * UInt8 result, contains `uint8` value optionally.
 * 
 * @member ok - `true` if no errors. Use this to check if this is valid result.
 * 
 * @member value - the `uint8` value.
 * 
 * @member error - pointer to error string. `NULL` if no errors.
 */
typedef struct result_uint8
{
  bool ok;
  union
  {
    uint8 value;
    const char* error;
  };
} result_uint8;

/**
 * Int16 result, contains `int16` optionally.
 * 
 * @member ok - `true` if no errors. Use this to check if this is valid result.
 * 
 * @member value - the `int16` value.
 * 
 * @member error - pointer to error string. `NULL` if no errors.
 */
typedef struct result_int16
{
  bool ok;
  union
  {
    int16 value;
    const char* error;
  };
} result_int16;

/**
 * UInt16 result, contains `uint16` optionally.
 * 
 * @member ok - `true` if no errors. Use this to check if this is valid result.
 * 
 * @member value - the `uint16` value.
 * 
 * @member error - pointer to error string. `NULL` if no errors.
 */
typedef struct result_uint16
{
  bool ok;
  union
  {
    uint16 value;
    const char* error;
  };
} result_uint16;

/**
 * Int32 result, contains `int32` optionally.
 * 
 * @member ok - `true` if no errors. Use this to check if this is valid result.
 * 
 * @member value - the `int32` value.
 * 
 * @member error - pointer to error string. `NULL` if no errors.
 */
typedef struct result_int32
{
  bool ok;
  union
  {
    int32 value;
    const char* error;
  };
} result_int32;

/**
 * UInt32 result, contains `uint32` optionally.
 * 
 * @member ok - `true` if no errors. Use this to check if this is valid result.
 * 
 * @member value - the `uint32` value.
 * 
 * @member error - pointer to error string. `NULL` if no errors.
 */
typedef struct result_uint32
{
  bool ok;
  union
  {
    uint32 value;
    const char* error;
  };
} result_uint32;

/**
 * Int64 result, contains `int64` optionally.
 * 
 * @member ok - `true` if no errors. Use this to check if this is valid result.
 * 
 * @member value - the `int64` value.
 * 
 * @member error - pointer to error string. `NULL` if no errors.
 */
typedef struct result_int64
{
  bool ok;
  union
  {
    int64 value;
    const char* error;
  };
} result_int64;

/**
 * UInt64 result, contains `uint64` optionally.
 * 
 * @member ok - `true` if no errors. Use this to check if this is valid result.
 * 
 * @member value - the `uint64` value.
 * 
 * @member error - pointer to error string. `NULL` if no errors.
 */
typedef struct result_uint64
{
  bool ok;
  union
  {
    uint64 value;
    const char* error;
  };
} result_uint64;

/**
 * Float32 result, contains `float32` optionally.
 * 
 * @member ok - `true` if no errors. Use this to check if this is valid result.
 * 
 * @member value - the `float32` value.
 * 
 * @member error - pointer to error string. `NULL` if no errors.
 */
typedef struct result_float32
{
  bool ok;
  union
  {
    float32 value;
    const char* error;
  };
} result_float32;

/**
 * Float64 result, contains `float64` optionally.
 * 
 * @member ok - `true` if no errors. Use this to check if this is valid result.
 * 
 * @member value - the `float64` value.
 * 
 * @member error - pointer to error string. `NULL` if no errors.
 */
typedef struct result_float64
{
  bool ok;
  union
  {
    float64 value;
    const char* error;
  };
} result_float64;

/**
 * Float80 result, contains `float80` optionally.
 * 
 * @member ok - `true` if no errors. Use this to check if this is valid result.
 * 
 * @member value - the `float80` value.
 * 
 * @member error - pointer to error string. `NULL` if no errors.
 */
typedef struct result_float80
{
  bool ok;
  union
  {
    float80 value;
    const char* error;
  };
} result_float80;

/**
 * Rect result, contains `rect` optionally.
 * 
 * @member ok - `true` if no errors. Use this to check if this is valid result.
 * 
 * @member value - the `rect` value.
 * 
 * @member error - pointer to error string. `NULL` if no errors.
 */
typedef struct result_rect
{
  bool ok;
  union
  {
    rect value;
    const char* error;
  };
} result_rect;

/**
 * Color result, contains `color` optionally.
 * 
 * @member ok - `true` if no errors. Use this to check if this is valid result.
 * 
 * @member value - the `color` value.
 * 
 * @member error - pointer to error string. `NULL` if no errors.
 */
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