#include "project.h"


static unsigned efi_type_to_mb (unsigned type)
{
  switch (type) {
  case EfiLoaderCode:
  case EfiLoaderData:
  case EfiBootServicesCode:
  case EfiBootServicesData:
  case EfiConventionalMemory:
    return MULTIBOOT_MEMORY_AVAILABLE;
    break;

  case EfiACPIReclaimMemory:
    return  MULTIBOOT_MEMORY_ACPI_RECLAIMABLE;
    break;

  case EfiACPIMemoryNVS:
    return MULTIBOOT_MEMORY_NVS;
    break;
  }

  return MULTIBOOT_MEMORY_RESERVED;
}



static unsigned translate_memory_map (void *map, size_t dsize, multiboot_memory_map_t *mb, unsigned n)
{
  unsigned i, m = 1;
  EFI_MEMORY_DESCRIPTOR *efi;
  uint64_t end;
  unsigned type;

  efi = map;
  map += dsize;

  mb->size = sizeof (mb[i]) - sizeof (mb[i].size);
  mb->addr = efi->PhysicalStart;
  mb->len = efi->NumberOfPages << 12;
  mb->type = efi_type_to_mb (efi->Type);

  for (i = 1; i < n; ++i) {

    efi = map;
    map += dsize;

    type = efi_type_to_mb (efi->Type);

    /*This is a continuation */
    if ((mb->type == type) && (efi->PhysicalStart == (mb->len + mb->addr)))
      mb->len += efi->NumberOfPages << 12;
    else {
      mb++;
      m++;

      mb->size = sizeof (mb[i]) - sizeof (mb[i].size);
      mb->addr = efi->PhysicalStart;
      mb->len = efi->NumberOfPages << 12;
      mb->type = type;
    }

  }


  return m * sizeof (multiboot_memory_map_t);
}

EFI_STATUS ebs (EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table)
{
  UINTN efi_map_size = 0, map_key, descriptor_size;
  UINT32 descriptor_ver;
  EFI_MEMORY_DESCRIPTOR *efi_memory_map = NULL;
  EFI_STATUS ret;
  unsigned n;
  multiboot_memory_map_t *mb_memory_map;
  unsigned mb_map_size;

  ret = uefi_call_wrapper (system_table->BootServices->GetMemoryMap, 5, &efi_map_size, efi_memory_map, NULL, &descriptor_size, NULL);

  if (ret != EFI_BUFFER_TOO_SMALL) return ret;

  efi_map_size += descriptor_size * 10; /* AllocatePools below may fragment memory more 2*n +1 */

  n = efi_map_size / descriptor_size;

  ret = uefi_call_wrapper (system_table->BootServices->AllocatePool, 3, EfiLoaderData, efi_map_size, (void **)&efi_memory_map);

  if (ret != EFI_SUCCESS) return ret;

#if 0
  ret = uefi_call_wrapper (system_table->BootServices->AllocatePool, 3, EfiLoaderData, n * sizeof (multiboot_memory_map_t), (void **)&mb_memory_map);

  if (ret != EFI_SUCCESS) return ret;

#endif

  mb_memory_map = (void *) (size_t) 0x6000;

  ret = uefi_call_wrapper (system_table->BootServices->GetMemoryMap, 5, &efi_map_size, efi_memory_map, &map_key, &descriptor_size, &descriptor_ver);

  if (ret != EFI_SUCCESS) return ret;

  n = efi_map_size / descriptor_size;

  ret = uefi_call_wrapper (system_table->BootServices->ExitBootServices, 2, image_handle, map_key);

  if (ret != EFI_SUCCESS) return ret;

  /* We now ownz all the memories! */

  mb_map_size = translate_memory_map (efi_memory_map, descriptor_size,  mb_memory_map, n);

  mb_info.mmap_addr = (uint32_t) (size_t) mb_memory_map;
  mb_info.mmap_length = mb_map_size;
  mb_info.flags |= MULTIBOOT_INFO_MEM_MAP;

  return EFI_SUCCESS;
}


