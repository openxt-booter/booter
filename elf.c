#include "project.h"

#include "elf.h"


uint64_t elf_deploy (uint8_t *image, size_t len)
{
  Elf32_Ehdr *eh = (void *) image;
  size_t ph_off = eh->e_phoff;

  unsigned i;

  /*Load any sections */

  for (i = 0; i < eh->e_phnum; i++, ph_off += eh->e_phentsize) {
    Elf32_Phdr *ph = (Elf32_Phdr *) &image[ph_off];


    if (ph->p_type != PT_LOAD && ph->p_type != PT_PHDR) continue;

    uint64_t addr = ph->p_paddr;
    uint64_t msize = ph->p_memsz;
    uint64_t dsize = MIN (msize, ph->p_filesz);

    if (ph->p_filesz)
      memcpy ((void *) (size_t) addr, &image[ph->p_offset], dsize);


    if (msize > dsize)
      memset ((void *) (size_t) (addr + dsize), 0, msize - dsize);


  }

  /*We could load the symbol table here but nothing uses it*/

  return eh->e_entry;
}


