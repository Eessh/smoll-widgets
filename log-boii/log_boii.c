#include "log_boii.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef LOG_BOII__COLORED_LOGS
#define COLOR_TRACE "\x1b[37m"
#define COLOR_DEBUG "\x1b[36m"
#define COLOR_INFO "\x1b[32m"
#define COLOR_WARN "\x1b[35m"
#define COLOR_ERROR "\x1b[33m"
#define COLOR_FATAL "\x1b[31m"
#define COLOR_RESET "\x1b[0m"
#endif

/*
 * Returns string format of `log_level`.
 *
 */
const char*
log_level_string(log_level level)
{
  switch (level) {
    case LOG_TRACE:
      return "TRACE";
    case LOG_DEBUG:
      return "DEBUG";
    case LOG_INFO:
      return "INFO ";
    case LOG_WARN:
      return "WARN ";
    case LOG_ERROR:
      return "ERROR";
    default:
      break;
  }
  return "FATAL";
}

char*
format_to_2_digits(int n)
{
  char* result = (char*)malloc(sizeof(char)*3);
  result[0] = '0';
  result[1] = '0';
  result[2] = '\0';
  result[1] += n%10;
  if (n < 10) {
    return result;
  }
  result[0] += n/10;
  return result;
}

/*
 * Returns time struct for current time.
 *
 */
struct tm*
time_info()
{
  time_t rawtime;
  struct tm* time_info;

  time(&rawtime);
  time_info = localtime(&rawtime);

  return time_info;
}

/*
 * Base function for all logging functions.
 *
 * Use `log_level` properly :)
 *
 * References how to use log levels properly:
 * https://stackoverflow.com/questions/2031163/when-to-use-the-different-log-levels
 * https://betterstack.com/community/guides/logging/log-levels-explained/
 *
 */
void
log_boii(log_level level,
         const char* function,
         const char* file,
         int line,
         const char* format_string,
         ...)
{
  struct tm* timeinfo = time_info();

  char* formatted_hour_str = format_to_2_digits(timeinfo->tm_hour);
  char* formatted_minute_str = format_to_2_digits(timeinfo->tm_min);
  char* formatted_second_str = format_to_2_digits(timeinfo->tm_sec);

#ifdef LOG_BOII__COLORED_LOGS
  switch (level) {
    case LOG_TRACE: {
      fprintf(stderr,
              "[%s:%s:%s] " COLOR_TRACE "%s" COLOR_RESET " %s:%s:%d: ",
              formatted_hour_str,
              formatted_minute_str,
              formatted_second_str,
              log_level_string(level),
              function,
              file,
              line);
      break;
    }
    case LOG_DEBUG: {
      fprintf(stderr,
              "[%s:%s:%s] " COLOR_DEBUG "%s" COLOR_RESET " %s:%s:%d: ",
              formatted_hour_str,
              formatted_minute_str,
              formatted_second_str,
              log_level_string(level),
              function,
              file,
              line);
      break;
    }
    case LOG_INFO: {
      fprintf(stderr,
              "[%s:%s:%s] " COLOR_INFO "%s" COLOR_RESET " %s:%s:%d: ",
              formatted_hour_str,
              formatted_minute_str,
              formatted_second_str,
              log_level_string(level),
              function,
              file,
              line);
      break;
    }
    case LOG_WARN: {
#ifdef LOG_BOII__HIGHLIGHT_WARN_ERROR_FATAL_STRINGS
      fprintf(stderr,
              "[%s:%s:%s] " COLOR_WARN "%s %s:%s:%d: " COLOR_RESET,
              formatted_hour_str,
              formatted_minute_str,
              formatted_second_str,
              log_level_string(level),
              function,
              file,
              line);
#else
      fprintf(stderr,
              "[%s:%s:%s] " COLOR_WARN "%s" COLOR_RESET " %s:%s:%d: ",
              formatted_hour_str,
              formatted_minute_str,
              formatted_second_str,
              log_level_string(level),
              function,
              file,
              line);
#endif
      break;
    }
    case LOG_ERROR: {
#ifdef LOG_BOII__HIGHLIGHT_WARN_ERROR_FATAL_STRINGS
      fprintf(stderr,
              "[%s:%s:%s] " COLOR_ERROR "%s %s:%s:%d: " COLOR_RESET,
              formatted_hour_str,
              formatted_minute_str,
              formatted_second_str,
              log_level_string(level),
              function,
              file,
              line);
#else
      fprintf(stderr,
              "[%s:%s:%s] " COLOR_ERROR "%s" COLOR_RESET " %s:%s:%d: ",
              formatted_hour_str,
              formatted_minute_str,
              formatted_second_str,
              log_level_string(level),
              function,
              file,
              line);
#endif
      break;
    }
    case LOG_FATAL: {
#ifdef LOG_BOII__HIGHLIGHT_WARN_ERROR_FATAL_STRINGS
      fprintf(stderr,
              "[%s:%s:%s] " COLOR_FATAL "%s %s:%s:%d: " COLOR_RESET,
              formatted_hour_str,
              formatted_minute_str,
              formatted_second_str,
              log_level_string(level),
              function,
              file,
              line);
#else
      fprintf(stderr,
              "[%s:%s:%s] " COLOR_FATAL "%s" COLOR_RESET " %s:%s:%d: ",
              formatted_hour_str,
              formatted_minute_str,
              formatted_second_str,
              log_level_string(level),
              function,
              file,
              line);
#endif
      break;
    }
    default:
      break;
  }
#else
  // printing usual stuff: time, log level, function, file, line
  fprintf(stderr,
          "[%s:%s:%s] %s %s:%s:%d: ",
          formatted_hour_str,
          formatted_minute_str,
          formatted_second_str,
          log_level_string(level),
          function,
          file,
          line);
#endif

  // handling variable arguments
  va_list format_args;
  va_start(format_args, format_string);
#ifdef LOG_BOII__HIGHLIGHT_WARN_ERROR_FATAL_STRINGS
  if (level == LOG_WARN) {
    fprintf(stderr, COLOR_WARN);
  } else if (level == LOG_ERROR) {
    fprintf(stderr, COLOR_ERROR);
  } else if (level == LOG_FATAL) {
    fprintf(stderr, COLOR_FATAL);
  }
#endif
  vfprintf(stderr, format_string, format_args);
#ifdef LOG_BOII__HIGHLIGHT_WARN_ERROR_FATAL_STRINGS
  fprintf(stderr, COLOR_RESET);
#endif
  va_end(format_args);

  fprintf(stderr, "\n");

  // Freeing formatted strings
  free(formatted_hour_str);
  free(formatted_minute_str);
  free(formatted_second_str);
}
