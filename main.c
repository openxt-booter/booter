#include "project.h"

multiboot_info_t mb_info;




EFI_STATUS EFIAPI
efi_main (EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table)
{
  EFI_STATUS ret;
  uint64_t entry;

  InitializeLib (image_handle, system_table);

  serial_init (1);

  Print (L"OpenXT EFI booter @ %llx\n", (unsigned long long) (size_t) efi_main);

  framebuffer_init();

  ret = ebs (image_handle, system_table);

  if (ret != EFI_SUCCESS) {
    Print (L"OpenXT EFI booter ebs returned 0x%x\n", ret);

    uefi_call_wrapper (system_table->BootServices->Stall, 1, 1000000);
    uefi_call_wrapper (system_table->BootServices->Stall, 1, 1000000);
    uefi_call_wrapper (system_table->BootServices->Stall, 1, 1000000);
    uefi_call_wrapper (system_table->BootServices->Stall, 1, 1000000);
    uefi_call_wrapper (system_table->BootServices->Stall, 1, 1000000);
    uefi_call_wrapper (system_table->BootServices->Stall, 1, 1000000);
    uefi_call_wrapper (system_table->BootServices->Stall, 1, 1000000);
    uefi_call_wrapper (system_table->BootServices->Stall, 1, 1000000);
    uefi_call_wrapper (system_table->BootServices->Stall, 1, 1000000);
    uefi_call_wrapper (system_table->BootServices->Stall, 1, 1000000);

    return ret;
  }

  payload_init();


  entry = elf_deploy ((void *) (size_t) payload[0].mod_start, payload[0].mod_end - payload[0].mod_start);

  if (n_payload > 1) {
    mb_info.flags |= MULTIBOOT_INFO_MODS;
    mb_info.mods_count = n_payload - 1;
    mb_info.mods_addr = (uint32_t) (size_t) &payload[1];
  }

  if (payload[0].cmdline) {
    mb_info.cmdline = payload[0].cmdline;
    mb_info.flags |= MULTIBOOT_INFO_CMDLINE;
  }

  make_ebda (system_table);

  boot (entry);

  for (;;);
}
