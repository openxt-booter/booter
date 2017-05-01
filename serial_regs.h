
#define BAUD_BASE     1843200


#define RXR       ((0))
#define TXR       ((0))
#define DLLR      ((0))

#define IER       ((1))
#define DLHR      ((1))

#define IIR       ((2))
#define FCR       ((2))

#define LCR       ((3))
#define LCR_WS0           (1<<0)
#define LCR_WS1           (1<<1)
#define LCR_NSTOP         (1<<2)
#define LCR_PARITY_ENABLE (1<<3)
#define LCR_EVEN_PARITY   (1<<4)
#define LCR_STUCK_PARITY  (1<<5)
#define LCR_BREAK         (1<<6)
#define LCR_DLAB          (1<<7)

#define MCR      ((4))
#define MCR_DTR        (1<<0)
#define MCR_RTS        (1<<1)
#define MCR_AUX1       (1<<2)
#define MCR_AUX2       (1<<3)
#define MCR_LOOP       (1<<4)
#define MCR_AUTOFLOW   (1<<5)

#define LSR      ((5))
#define LSR_DA         (1<<0)
#define LSR_OR         (1<<1)
#define LSR_PE         (1<<2)
#define LSR_FE         (1<<3)
#define LSR_BRK        (1<<4)
#define LSR_THRE       (1<<5)
#define LSR_THREI      (1<<6)
#define LSR_FIFOE      (1<<7)

#define MSR ((6))

