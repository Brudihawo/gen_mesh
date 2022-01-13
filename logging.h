#ifndef LOGGING_H
#define LOGGING_H

#define log_msg(...) _log(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
void _log(const char* func, const char *file, int line, const char *fmt, ...);

#endif // LOGGING_H
