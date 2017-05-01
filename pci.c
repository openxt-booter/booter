#include "project.h"

#include "pci_regs.h"

/* PCI config read/write */
uint8_t
pci_conf1_read_8 (int bus, int slot, int fn, int off)
{
  int addr = 0xcfc + (off & 3);
  out_32 (0x0cf8,
          0x80000000 | (bus << 16) | (slot << 11) | (fn << 8) | (off & ~3));
  return in_8 (addr);
}

uint16_t
pci_conf1_read_16 (int bus, int slot, int fn, int off)
{
  int addr = 0xcfc + (off & 3);
  out_32 (0x0cf8,
          0x80000000 | (bus << 16) | (slot << 11) | (fn << 8) | (off & ~3));
  return in_16 (addr);
}

uint32_t
pci_conf1_read_32 (int bus, int slot, int fn, int off)
{
  int addr = 0xcfc + (off & 3);
  out_32 (0x0cf8,
          0x80000000 | (bus << 16) | (slot << 11) | (fn << 8) | (off & ~3));
  return in_32 (addr);
}

void
pci_conf1_write_8 (int bus, int slot, int fn, int off, uint8_t val)
{
  int addr = 0xcfc + (off & 3);
  out_32 (0x0cf8,
          0x80000000 | (bus << 16) | (slot << 11) | (fn << 8) | (off & ~3));
  out_8 (addr, val);
}

void
pci_conf1_write_16 (int bus, int slot, int fn, int off, uint16_t val)
{
  int addr = 0xcfc + (off & 3);
  out_32 (0x0cf8,
          0x80000000 | (bus << 16) | (slot << 11) | (fn << 8) | (off & ~3));
  out_16 (addr, val);
}

void
pci_conf1_write_32 (int bus, int slot, int fn, int off, uint32_t val)
{
  int addr = 0xcfc + (off & 3);
  out_32 (0x0cf8,
          0x80000000 | (bus << 16) | (slot << 11) | (fn << 8) | (off & ~3));
  out_32 (addr, val);
}


