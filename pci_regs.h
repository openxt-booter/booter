#ifndef _SHARED_PCI_REGS_H_
#define _SHARED_PCI_REGS_H_

#define PCI_VENDOR_ID                   0x00
#define PCI_DEVICE_ID                   0x02
#define PCI_COMMAND                     0x04
#define PCI_BASE_ADDRESS_0              0x10
#define PCI_CLASS_DEVICE      0x0a

#define  PCI_BASE_ADDRESS_SPACE_IO      0x01
#define  PCI_BASE_ADDRESS_IO_MASK       (~0x07UL)
#define  PCI_BASE_ADDRESS_MEM_MASK      (~0x0fUL)

#endif
