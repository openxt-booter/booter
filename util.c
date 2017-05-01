#include "project.h"


int memcmp (const void *_s1, const void *_s2, size_t n)
{
  const uint8_t *s1 = _s1;
  const uint8_t *s2 = _s2;

  while (n--)  {
    if (*s1 > *s2)
      return -1;

    if (*s2 < *s1)
      return 1;

    s1++;
    s2++;
  }

  return 0;
}

void *memcpy (void *_d, const void *_s, size_t n)
{
  const uint8_t *s = _s;
  uint8_t *d = _d;

  while (n--)
    * (d++) = * (s++);

  return _d;
}


void *memset (void *_d, int c, size_t n)
{
  uint8_t *d = _d;

  while (n--)
    * (d++) = c;

  return _d;
}
