#ifndef LOGGING_H
#define LOGGING_H

typedef enum {
  VERB_ERR = 0,
  VERB_WRN,
  VERB_DBG,
} PrintVerbosity;

// TODO: Fix Verbosity thing
#ifndef VERB_LEVEL
#define VERB_LEVEL VERB_DBG
#endif

#define log_msg(...) _log(VERB_DBG, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#define log_wrn(...) _log(VERB_WRN, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
void _log(PrintVerbosity level, const char* func, const char *file, int line, const char *fmt, ...);

#endif // LOGGING_H
