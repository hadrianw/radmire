#include "utils.h"
#include <stdio.h>

void rr_fatal(const char *file, int line, const char *function, const char *format, ...) {
	va_list argp;
	fprintf(stderr, "%s: In `%s`:\n%s:%d: fatal runtime error: ", file, function, file, line);
	va_start(argp, format);
	vfprintf(stderr, format, argp);
	va_end(argp);
	fprintf(stderr, "\n");
}

void rr_error(const char *file, int line, const char *function, const char *format, ...) {
	va_list argp;
	fprintf(stderr, "%s: In `%s`:\n%s:%d: runtime error: ", file, function, file, line);
	va_start(argp, format);
	vfprintf(stderr, format, argp);
	va_end(argp);
	fprintf(stderr, "\n");
}

void rr_warn(const char *file, int line, const char *function, const char *format, ...) {
	va_list argp;
	fprintf(stderr, "%s: In `%s`:\n%s:%d: runtime warning: ", file, function, file, line);
	va_start(argp, format);
	vfprintf(stderr, format, argp);
	va_end(argp);
	fprintf(stderr, "\n");
}

void rr_info(const char *format, ...) {
	va_list argp;
	fprintf(stderr, " ");
	va_start(argp, format);
	vfprintf(stderr, format, argp);
	va_end(argp);
	fprintf(stderr, "\n");
}
