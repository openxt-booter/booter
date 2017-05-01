#ifndef _SHARED_SERIAL_H_
#define _SHARED_SERIAL_H_

typedef struct  {
  int present;
  int sick;
  int pci;
  uint16_t io_base;
  uint64_t clock;
  unsigned b, d, f;
  char status[128];
} serial_t;

#endif

