#include "project.h"
#include "serial.h"
#include "serial_regs.h"
#include "pci_regs.h"

#define BAUD_RATE   115200
static serial_t default_port;

//
// magic runes to stop windows turning off the
// power to the serial port on boot
//

static void
dell_lpcb_write (uint8_t index, uint8_t value)
{
  out_8 (0x4E, index);
  out_8 (0x4f, value);
}

static uint8_t
dell_lpcb_read (uint8_t index)
{
  out_8 (0x4E, index);
  return in_8 (0x4f);
}

void
serial_dell_power_on (serial_t *s)
{
  if (!s->present) return;

  if (s->io_base != 0x3f8) return;

  dell_lpcb_write (0x55, 0);
  dell_lpcb_write (0x02, dell_lpcb_read (0x02) | 0x8);
  dell_lpcb_write (0xaa, 0);
}



void serial_write_reg (serial_t *s, unsigned reg, uint8_t val)
{
  out_8 (s->io_base + reg, val);
}

uint8_t serial_read_reg (serial_t *s, unsigned reg)
{
  return in_8 (s->io_base + reg);
}



void serial_set_baud (serial_t *s, unsigned baud)
{
  uint32_t divisor = (s->clock >> 4) / baud;
  uint8_t lcr;

  lcr = serial_read_reg (s, LCR);
  serial_write_reg (s, LCR, lcr | LCR_DLAB);

  serial_write_reg (s, DLHR, divisor >> 8);
  serial_write_reg (s, DLLR, divisor & 0xff);

  serial_write_reg (s, LCR, lcr);

}


int serial_check_baud (serial_t *s, unsigned baud)
{
  uint32_t divisor = (s->clock >> 4) / baud;
  uint8_t lcr;
  int sad = 0;

  lcr = serial_read_reg (s, LCR);
  serial_write_reg (s, LCR, lcr | LCR_DLAB);

  if (serial_read_reg (s, DLHR) != (divisor >> 8)) sad++;

  if (serial_read_reg (s, DLLR) != (divisor & 0xff)) sad++;

  serial_write_reg (s, LCR, lcr);


  return sad;
}


void serial_setup (serial_t *s)
{
  uint8_t lcr;

  if (!s) s = &default_port;

  if (s->pci)
    pci_conf1_write_16 (s->b, s->d, s->f, PCI_COMMAND, 1);

  if (!serial_check_baud (s, BAUD_RATE)) return;


  serial_write_reg (s, FCR, 0x6); /*disable FIFOs and reset */
  serial_write_reg (s, IER, 0); /* no interrupts*/

  /* Clear all interrupts */
  serial_read_reg (s, IIR);
  serial_read_reg (s, MSR);
  serial_read_reg (s, RXR);
  serial_read_reg (s, LSR);

  serial_set_baud (s, BAUD_RATE);

  serial_write_reg (s, LCR, LCR_WS0 | LCR_WS1); /*8 bits, 1 stop bit, no parity*/
  serial_write_reg (s, MCR, MCR_RTS | MCR_DTR); /*rts and dtr on */
}


static int serial_probe (serial_t *s)
{
  uint8_t ier, lcr, dlh, test;

  lcr = serial_read_reg (s, LCR);
  serial_write_reg (s, LCR, lcr | LCR_DLAB);
  dlh = serial_read_reg (s, DLHR);
  serial_write_reg (s, DLHR, 0x55);
  serial_write_reg (s, LCR, lcr);

  ier = serial_read_reg (s, IER);
  serial_write_reg (s, IER, 0x00);

  if (serial_read_reg (s, IER) != 0x00) return 1;

  serial_write_reg (s, IER, ier);

  serial_write_reg (s, LCR, lcr | LCR_DLAB);

  if (serial_read_reg (s, DLHR) != 0x55) return 1;

  serial_write_reg (s, DLHR, 0xaa);

  if (serial_read_reg (s, DLHR) != 0xaa) return 1;

  serial_write_reg (s, DLHR, dlh);
  serial_write_reg (s, LCR, lcr);

  return 0;
}


static int
serial_scan_pci (serial_t *s, uint8_t start_bus)
{
  unsigned b, d, f;
  uint16_t class;

  uint32_t bar0, didvid;
  uint16_t cmd;
  unsigned len;


  for (b = start_bus; b < 0x100; b++)
    for (d = 0; d < 0x1f; d++)
      for (f = 0; f < 0x8; f++) {

        didvid = pci_conf1_read_32 (b, d, f, PCI_VENDOR_ID);

        if ((didvid == 0) || (didvid == 0xffffffff))
          continue;

        class = pci_conf1_read_16 (b, d, f, PCI_CLASS_DEVICE);

        /*Serial port? */
        if (class != 0x700)
          continue;

        /*Using IO */
        bar0 = pci_conf1_read_32 (b, d, f, PCI_BASE_ADDRESS_0);

        if (! (bar0 & 1)) continue;

        cmd = pci_conf1_read_16 (b, d, f, PCI_COMMAND);

#if 1
        pci_conf1_write_16 (b, d, f, PCI_COMMAND, cmd & ~1);

        pci_conf1_write_32 (b, d, f, PCI_BASE_ADDRESS_0, 0xffffffff);
        len = pci_conf1_read_32 (b, d, f, PCI_BASE_ADDRESS_0);
        pci_conf1_write_32 (b, d, f, PCI_BASE_ADDRESS_0, bar0);

        pci_conf1_write_16 (b, d, f, PCI_COMMAND, cmd);

        //FIXME use proper constants
        len &= 0xfffc;
        len = 0x10000 - len;
#else
        len = 256;
#endif

        if ((len != 8) && (len != 16) && (len != 256)) continue;

        /*Ok well have this then*/

        cmd |= 1;
        pci_conf1_write_16 (b, d, f, PCI_COMMAND, cmd);

        s->io_base = bar0 & ~1;
        s->clock = BAUD_BASE;

        switch (didvid) {
        case 0x99009710:
          s->clock = 1638400;
          break;
          break;

        case 0x99229710:
          s->clock = 1638400;
          break;

        case 0x950b1415:
          s->clock = BAUD_BASE * 8;
          break;
        }

        while (len) {

          if (!serial_probe (s)) {
            s->present = 1;
            s->pci = 1;
            s->b = b;
            s->d = d;
            s->f = f;
            return 0;
          }

          s->io_base += 8;
          len -= 8;
        }



      }

  return 1;
}

static int serial_scan_isa (serial_t *s, uint16_t port)
{

  s->io_base = port;
  s->clock = BAUD_BASE;

  if (!serial_probe (s)) {
    s->present = 1;
    return 0;
  }

  return 1;
}



static int serial_scan (serial_t *s)
{
  /*Start at bus 1 to avoid AMT ports in the root complex */
  if (!serial_scan_pci (s, 1)) return 0;

#if 1

  /*Try the canonical ISA places*/
  if (!serial_scan_isa (s, 0x3f8)) return 0;

  if (!serial_scan_isa (s, 0x2f8)) return 0;

#endif

  /*Try looking in the root complex */
  if (!serial_scan_pci (s, 0)) return 0;

  return 1;
}

int serial_have_data (serial_t *s)
{
  if (!s) s = &default_port;

  if (!s->present) return 0;

  return !! (serial_read_reg (s, LSR) & LSR_DA) ;
}


static int check_port_sane (serial_t *s)
{
  int i;
  uint16_t cmd;



  if (s->sick) {

    /*If it's PCI try turning it on */
    if (s->pci) {
      cmd = pci_conf1_read_16 (s->b, s->d, s->f, PCI_COMMAND);

      if (! (cmd & 1)) {
        cmd |= 1;
        pci_conf1_write_16 (s->b, s->d, s->f, PCI_COMMAND, cmd);
      }
    }

    if (serial_read_reg (s, LSR) & LSR_THRE) {
      /*seems well again try using it*/
      s->sick = 0;
      return 0;
    }


    return 1;
  }



  for (i = 0; i < 10000; ++i)
    if (serial_read_reg (s, LSR) & LSR_THRE) return 0;

  s->sick = 1;

  return 0;
}


int
serial_getc (serial_t *s)
{

  if (!s) s = &default_port;

  if (!s->present) return -1;

  if (check_port_sane (s)) return -1;


  if (serial_read_reg (s, LSR) & LSR_DA)
    return serial_read_reg (s, RXR);

  return -1;
}



void
serial_putc (serial_t *s, char c)
{
  if (!s) s = &default_port;

  if (!s->present) return;

  if (check_port_sane (s)) return;

  serial_write_reg (s, TXR, c);

}

void
serial_puts (serial_t *s, char *str)
{
  if (!s) s = &default_port;

  if (!s->present);

  while (*str) {
    if (*str == '\n') serial_putc (s, '\r');

    serial_putc (s, * (str++));
  }
}

int serial_present (serial_t *s)
{
  if (!s) s = &default_port;

  return s->present;
}

const char *serial_status (serial_t *s)
{
  if (!s) s = &default_port;

  return s->status;
}


void
serial_init (int enabled)
{

  if (!enabled) return;

  if (serial_scan (&default_port)) return;

  serial_setup (&default_port);
  serial_puts (&default_port, "\nSerial port initialized\n");

  while (serial_getc (&default_port) != -1);
}






