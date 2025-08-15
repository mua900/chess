#ifndef _COMMON_H
#define _COMMON_H

#include "std_include.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef struct {
  char* data;
  int size;
} String;

typedef struct {
  int x, y;
} ivec2;

typedef struct {
  float x, y;
} vec2;

#define BIT(x) (1 << x)

int pop_lsb(uint64_t* n);
int pop_msb(uint64_t* n);

int hamming(uint64_t n);


enum Log_Level {
  Log_Level_Message,
  Log_Level_Warning,
  Log_Level_Error,
};

void log_log(enum Log_Level ll, char const * const msg);

#define LOG_ERROR(msg)    log_log(Log_Level_Error, msg)
#define LOG_WARNING(msg)  log_log(Log_Level_Warning, msg)
#define LOG_MSG(msg)      log_log(Log_Level_Message, msg)

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))


#define MIN(x,y) (((x) > (y)) ? (y) : (x))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

typedef struct {
  unsigned char r,b,g,a;
} Color;


#define COLOR_WHITE ((Color){0xff,0xff,0xff,0xff})
#define COLOR_BLACK ((Color){0,0,0,0xff})
#define COLOR_RED   ((Color){0xff,0,0,0xff})
#define COLOR_GREEN ((Color){0,0xff,0,0xff})
#define COLOR_BLUE  ((Color){0,0,0xff,0xff})

#define COLOR_ARG(color) color.r,color.g,color.b,color.a

#endif
