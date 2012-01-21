#include <stdarg.h>
#include <assert.h>

#define LOG_FATAL(fmt, ...) rr_fatal(__FILE__, __LINE__, __ASSERT_FUNCTION, fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...) rr_error(__FILE__, __LINE__, __ASSERT_FUNCTION, fmt, __VA_ARGS__)
#define LOG_WARN(fmt, ...) rr_warn(__FILE__, __LINE__, __ASSERT_FUNCTION, fmt, __VA_ARGS__)
#define LOG_INFO(fmt, ...) rr_info(fmt, __VA_ARGS__)

void rr_fatal(const char *file, int line, const char *function, const char *format, ...);
void rr_error(const char *file, int line, const char *function, const char *format, ...);
void rr_warn(const char *file, int line, const char *function, const char *format, ...);
void rr_info(const char *format, ...);

#define LENGTH(X) (sizeof X / sizeof X[0])

