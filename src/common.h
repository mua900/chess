#ifndef _COMMON_H
#define _COMMON_H

#include "std_include.h"

[[noreturn]] void panic(char const * const msg);

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef struct {
  const unsigned char* data;
  int size;
} String;

#define STRING_EMPTY ((String){.data=NULL,.size=0})
#define CSTRING_LENGTH(s) (sizeof(s)-1)
#define MAKE_STRING(s) (String){.data=s,.size=CSTRING_LENGTH(s)}

String make_string(const char* s);
bool string_compare(String s1, String s2);
String string_slice(String s, int start, int end);
String string_trim(String s);
String string_get_extension(String s);
String string_copy(String s);

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

void log_log(enum Log_Level ll, char const * const msg, ...);

#define LOG_ERROR(msg)    log_log(Log_Level_Error, msg)
#define LOG_WARNING(msg)  log_log(Log_Level_Warning, msg)
#define LOG_MSG(msg)      log_log(Log_Level_Message, msg)
#define LOG_ERRORF(msg, ...)    log_log(Log_Level_Error, msg, __VA_ARGS__)
#define LOG_WARNINGF(msg, ...)  log_log(Log_Level_Warning, msg, __VA_ARGS__)
#define LOG_MSGF(msg, ...)      log_log(Log_Level_Message, msg, __VA_ARGS__)

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

typedef String File;

long get_file_size(FILE* handle);
File load_file(const char* file_name, bool* success);
void unload_file(File file);
bool file_exists(File file);

typedef struct {
    char* buffer;
    int buffer_capacity;
    int cursor;
} String_Builder;

String_Builder make_string_builder(int initial_capacity);
void sb_append(String_Builder* sb, String string);
void sb_append_char(String_Builder* sb, char ch);
const char* sb_c_string(String_Builder* sb);
void sb_remove(String_Builder* sb, int amount);  // remove the last n characters from the buffer
void sb_clear_and_append(String_Builder* sb, String s);
void sb_append_many(String_Builder* sb, String* strings, int n);
void sb_free(String_Builder* sb);
void sb_clear(String_Builder* sb);

static inline bool is_digit (char c)
{
    return c >= '0' && c <= '9';
}

static inline bool is_alpha_lower (char c)
{
    return c >= 'a' && c <= 'z';
}

static inline bool is_alpha_upper (char c)
{
    return c >= 'A' && c <= 'Z';
}

static inline bool is_alpha (char c)
{
    return is_alpha_lower(c) || is_alpha_upper(c);
}

#endif
