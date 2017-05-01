static inline void
out_8 (uint16_t port, uint8_t value)
{
  __asm__ __volatile__ ("out" "b" " %" "b" "0,%" "w" "1"::"a" (value),
                        "Nd" (port));
}

static inline void
out_16 (uint16_t port, uint16_t value)
{
  __asm__ __volatile__ ("out" "w" " %" "w" "0,%" "w" "1"::"a" (value),
                        "Nd" (port));
}

static inline void
out_32 (uint16_t port, uint32_t value)
{
  __asm__ __volatile__ ("out" "l" " %" "0,%" "w" "1"::"a" (value),
                        "Nd" (port));
}

static inline uint8_t
in_8 (uint16_t port)
{
  uint8_t _v;
  __asm__ __volatile__ ("in" "b" " %" "w" "1,%" "" "0":"=a" (_v):"Nd" (port));
  return _v;
}

static inline uint16_t
in_16 (uint16_t port)
{
  uint16_t _v;
  __asm__ __volatile__ ("in" "w" " %" "w" "1,%" "" "0":"=a" (_v):"Nd" (port));
  return _v;
}

static inline uint32_t
in_32 (uint16_t port)
{
  uint32_t _v;
  __asm__ __volatile__ ("in" "l" " %" "w" "1,%" "" "0":"=a" (_v):"Nd" (port));
  return _v;
}


#if 0

#if 0
# define __iomem        __attribute__((noderef, address_space(2)))
# define __force        __attribute__((force))
#else
#define __iomem
#define __force
#endif



static inline uint8_t
read_8 (const volatile void __iomem *addr)
{
  uint8_t ret;
  asm volatile ("mov" "b" " %1,%0":"=q" (ret):"m" (* (volatile uint8_t __force *) addr):"memory");
  return ret;
}

static inline uint16_t
read_16 (const volatile void __iomem *addr)
{
  uint16_t ret;
  asm volatile ("mov" "w" " %1,%0":"=r" (ret):"m" (* (volatile uint16_t __force *) addr):"memory");
  return ret;
}

static inline uint32_t
read_32 (const volatile void __iomem *addr)
{
  uint32_t ret;
  asm volatile ("mov" "l" " %1,%0":"=r" (ret):"m" (* (volatile uint32_t __force *) addr):"memory");
  return ret;
}




static inline void
write_8 (volatile void __iomem *addr, uint8_t val)
{
  asm volatile ("mov" "b" " %0,%1"::"q" (val), "m" (* (volatile uint8_t __force *) addr):"memory");
}

static inline void
write_16 (volatile void __iomem *addr, uint16_t val)
{
  asm volatile ("mov" "w" " %0,%1"::"r" (val), "m" (* (volatile uint16_t __force *) addr):"memory");
}

static inline void
write_32 (volatile void __iomem *addr, uint32_t val)
{
  asm volatile ("mov" "l" " %0,%1"::"r" (val), "m" (* (volatile uint32_t __force *) addr):"memory");
}

#endif

static inline uint64_t
rdmsr (unsigned int msr)
{
  uint32_t low, high;
  asm volatile ("rdmsr":"=a" (low), "=d" (high):"c" (msr));
  return (low) | ((uint64_t) (high) << 32);
}


#if 0
static inline void
wrmsr (unsigned int msr, uint64_t val)
{
  uint32_t low, high;

  low = val & 0xfffffffful;
  high = val >> 32;

  asm volatile ("wrmsr"::"c" (msr), "a" (low), "d" (high):"memory");
}
#endif



static inline void cpuid (uint64_t *rax, uint64_t *rbx, uint64_t *rcx, uint64_t *rdx)
{
  asm volatile ("cpuid" : "+a" (*rax), "=b" (*rbx), "+c" (*rcx), "=d" (*rdx) ::"cc");
}

#if 0
static inline uint32_t str (void)
{
  uint32_t ret = 0;
  asm volatile ("str %%ax" : "+a" (ret) ::"cc");

  return ret;
}

#endif

