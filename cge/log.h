/**
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

#define LOG_VERSION "0.1.0"

typedef struct {
  va_list ap;
  const char *fmt;
  const char *file;
  struct tm *time;
  void *udata;
  int line;
  int level;
} log_Event;

typedef void (*log_LogFn)(log_Event *ev, bool no_header);
typedef void (*log_LockFn)(bool lock, void *udata);

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

#define log_trace(...) log_log(LOG_TRACE,false, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG,false, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_log(LOG_INFO, false, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_log(LOG_WARN, false, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR,false, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL,false, __FILE__, __LINE__, __VA_ARGS__)

//补充no_header参数，用于不打印额外信息，方便拼接日志信息, zipxing
#define log_trace_n(...) log_log(LOG_TRACE,true, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug_n(...) log_log(LOG_DEBUG,true, __FILE__, __LINE__, __VA_ARGS__)
#define log_info_n(...)  log_log(LOG_INFO, true, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn_n(...)  log_log(LOG_WARN, true, __FILE__, __LINE__, __VA_ARGS__)
#define log_error_n(...) log_log(LOG_ERROR,true, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal_n(...) log_log(LOG_FATAL,true, __FILE__, __LINE__, __VA_ARGS__)

const char* log_level_string(int level);
void log_set_lock(log_LockFn fn, void *udata);
void log_set_level(int level);
void log_set_quiet(bool enable);
int log_add_callback(log_LogFn fn, void *udata, int level);
int log_add_fp(FILE *fp, int level);

//补充no_header参数，用于不打印额外信息，方便拼接日志信息, zipxing
void log_log(int level, bool no_header, const char *file, int line, const char *fmt, ...);

#endif
