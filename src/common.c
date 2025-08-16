#include "common.h"

int pop_lsb(uint64_t* n)
{
  int index = __builtin_ctzll(*n);
  *n &= *n-1;
  return index;
}

int pop_msb(uint64_t* n)
{
  int index = 63 - (__builtin_clzll(*n));
  *n &= ~((uint64_t)1 << index);
  return index;
}

int hamming(uint64_t n)
{
  return __builtin_popcount(n);
}

void log_log(enum Log_Level ll, char const * const msg)
{
  switch (ll)
  {
  case Log_Level_Message:
    fprintf(stderr, "[ERROR]: %s\n", msg); break;
  case Log_Level_Warning:
    fprintf(stderr, "[WARNING]: %s\n", msg); break;
  case Log_Level_Error:
    fprintf(stderr, "[MESSAGE]: %s\n", msg); break;
  default:
    fprintf(stderr, "%s\n", msg); break;
  }
}

String make_string(char* s)
{
    int len = strlen(s);
    return (String){.data=s,.size=len};
}

bool string_compare(String s1, String s2)
{
    if (s1.size != s2.size) return false;
    for (int i = 0; i < s1.size; i++)
    {
        if (s1.data[i] != s2.data[i]) return false;
    }
    return true;
}

String string_slice(String s, int start, int end)
{
    return (String){.data=s.data+start,.size=end-start};
}

String string_get_extension(String s)
{
  for (int i = s.size-1; i >= 0; i--)
  {
    if (s.data[i] == '.')
    {
      return string_slice(s,i,s.size);
    }
  }

  return STRING_EMPTY;
}

String string_trim_begin(String s)
{
    int i = 0;
    while (isspace(s.data[i]))
    {
        i++;
    }

    return string_slice(s,i,s.size);
}

String string_trim_end(String s)
{
    int i = s.size-1;
    while (isspace(s.data[i]))
    {
        i--;
    }

    return string_slice(s,0,i+1);
}

String string_trim(String s)
{
    return string_trim_begin(string_trim_end(s));
}

long get_file_size(FILE* handle)
{
    long pos = ftell(handle);
    fseek(handle, 0, SEEK_END);
    long size = ftell(handle);
    fseek(handle, 0, pos);
    return size;
}

File load_file(const char* file_name, bool* success)
{
    *success = true;
    FILE* handle = fopen(file_name,"r");
    long size = get_file_size(handle);
    char* contents = malloc(size);
    if (!contents)
    {
        *success = false;
        return STRING_EMPTY;
    }
    long writen = fread(contents, 1, size, handle);
    if (size != writen)
    {
        *success = false;
    }
    
    return (File){.data=contents,.size=size};
}

void unload_file(File file)
{
    free(file.data);
}

bool file_exists(File file_name)
{
    FILE* file = fopen(file_name.data, "r");
    if (file != NULL) {
        fclose(file);
        return true;
    }
    return false;
}

String_Builder make_string_builder(int initial_capacity) {
    String_Builder sb = (String_Builder) {
        .buffer = NULL,
        .buffer_capacity = 0,
        .cursor = 0,
    };

    sb.buffer = (char*)malloc(initial_capacity);
    sb.buffer_capacity = initial_capacity;
    sb.cursor = 0;
    sb.buffer[0] = '\0';
    return sb;
}

void sb_resize(String_Builder* sb) {
    char* nbuff = (char*)malloc(sb->buffer_capacity * 2 * sizeof(char));
    memcpy(nbuff, sb->buffer, sb->cursor);
    free(sb->buffer);
    sb->buffer = nbuff;
    sb->buffer_capacity *= 2;
}

int sb_grow_to_size(String_Builder* sb, int size) {
    int count = 0;
    while (size >= sb->buffer_capacity) {
        sb_resize(sb);
        count++;
        if (count > 5) {
            fprintf(stderr, "String builder buffer resize failed repeatedly: Possible memory allocation issue or corrupted buffer state.\n"
                            "Relevant: buffer_capacity: %d, cursor: %d, provided string size: %d",
                            sb->buffer_capacity, sb->cursor, size);
            return 1;
        }
    }

    return 0;
}

void sb_append(String_Builder* sb, String string) {
    sb_grow_to_size(sb, sb->cursor + string.size);

    memcpy(sb->buffer + sb->cursor, string.data, string.size);
    sb->cursor += string.size;
}

void sb_append_char(String_Builder* sb, char ch) {
    sb_grow_to_size(sb, sb->cursor + 1);

    sb->buffer[sb->cursor] = ch;
    sb->cursor += 1;
}

void sb_clear_and_append(String_Builder* sb, String s) {
    sb->cursor = 0;
    sb_append(sb, s);
}

void sb_append_many(String_Builder* sb, String* strings, int n) {
    int total_length = 0;
    for (int i = 0; i < n; i++) {
        total_length += strings[i].size;
    }

    sb_grow_to_size(sb, sb->cursor + total_length);
    for (int i = 0; i < n; i++) {
        memcpy(sb->buffer + sb->cursor, strings[i].data, strings[i].size);
        sb->cursor += strings[i].size;
    }
}

const char* sb_c_string(String_Builder* sb) {
    sb->buffer[sb->cursor] = '\0';
    return sb->buffer;
}

void sb_free(String_Builder* sb) {
    free(sb->buffer);
    sb->cursor = 0;
    sb->buffer_capacity = 0;
    sb->buffer = NULL;
}

void sb_clear(String_Builder* sb) {
    sb->cursor = 0;
    sb->buffer[0] = '\0';
}
