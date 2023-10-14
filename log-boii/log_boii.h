#ifndef LOG_BOII
#define LOG_BOII

#ifdef __cplusplus
extern "C"
{
#endif

  /*
   * Log levels.
   *
   * Use `log_level` properly :)
   *
   * References how to use log levels properly:
   * https://stackoverflow.com/questions/2031163/when-to-use-the-different-log-levels
   * https://betterstack.com/community/guides/logging/log-levels-explained/
   *
   */
  typedef enum log_level
  {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
  } log_level;

#define log_trace(...)                                                         \
  log_boii(LOG_TRACE, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...)                                                         \
  log_boii(LOG_DEBUG, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)                                                          \
  log_boii(LOG_INFO, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)                                                          \
  log_boii(LOG_WARN, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...)                                                         \
  log_boii(LOG_ERROR, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...)                                                         \
  log_boii(LOG_FATAL, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)

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
  void log_boii(log_level level,
                const char* function,
                const char* file,
                int line,
                const char* format_string,
                ...);

#ifdef __cplusplus
}
#endif

#endif
