#include "stdarg.h"
#include "stdio.h"

#include "logging.h"

void _log(const char* func, const char *file, int line, const char *fmt, ...) {
#ifndef DEBUG
  va_list args;
  fprintf(stderr, "%s:%i (%s): ", file, line, func);
  va_start(args, fmt);

  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");

  va_end(args);
#endif
}
