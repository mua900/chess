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
