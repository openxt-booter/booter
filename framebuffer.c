#include "project.h"

static EFI_GUID  efi_graphics_output_protocol_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;


static int mask_to_pos (uint32_t m)
{
  uint32_t c;
  int i;

  for (i = 0, c = 1; i < 32; c <<= 1, i++) {
    if (c & m) return i;
  }


  return 0;
}

void  framebuffer_init (void)
{
  EFI_STATUS st;
  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

  st = uefi_call_wrapper (BS->LocateProtocol, 3, &efi_graphics_output_protocol_guid,
                          NULL,
                          (VOID **) &gop);

  if (st != EFI_SUCCESS) return;

  if (!gop->Mode) return;

  if (!gop->Mode->Info) return;

  if (gop->Mode->Info->PixelFormat == PixelBltOnly) return;

  mb_info.flags |= MULTIBOOT_INFO_FRAMEBUFFER_INFO;

  mb_info.framebuffer_type = MULTIBOOT_FRAMEBUFFER_TYPE_RGB;
  mb_info.framebuffer_width = gop->Mode->Info->HorizontalResolution;
  mb_info.framebuffer_height = gop->Mode->Info->VerticalResolution;
  mb_info.framebuffer_pitch = gop->Mode->Info->PixelsPerScanLine;
  mb_info.framebuffer_addr = gop->Mode->FrameBufferBase;


  mb_info.framebuffer_red_mask_size = 8;
  mb_info.framebuffer_green_mask_size = 8;
  mb_info.framebuffer_blue_mask_size = 8;

  switch (gop->Mode->Info->PixelFormat) {
  case PixelRedGreenBlueReserved8BitPerColor:
    mb_info.framebuffer_red_field_position = 0;
    mb_info.framebuffer_green_field_position = 8;
    mb_info.framebuffer_blue_field_position = 16;
    break;

  case PixelBlueGreenRedReserved8BitPerColor:
    mb_info.framebuffer_red_field_position = 16;
    mb_info.framebuffer_green_field_position = 8;
    mb_info.framebuffer_blue_field_position = 0;
    break;

  case PixelBitMask:
    mb_info.framebuffer_red_field_position = mask_to_pos (gop->Mode->Info->PixelInformation.RedMask);
    mb_info.framebuffer_green_field_position = mask_to_pos (gop->Mode->Info->PixelInformation.GreenMask);
    mb_info.framebuffer_blue_field_position = mask_to_pos (gop->Mode->Info->PixelInformation.BlueMask);
    break;

  default:
    break;

  }


  Print (L"FB Info: %llx %d %d %d %d,%d %d,%d %d,%d\n",
         (long long unsigned) mb_info.framebuffer_addr,
         mb_info.framebuffer_width, mb_info.framebuffer_height, mb_info.framebuffer_pitch,
         mb_info.framebuffer_red_field_position, mb_info.framebuffer_red_mask_size,
         mb_info.framebuffer_green_field_position, mb_info.framebuffer_green_mask_size,
         mb_info.framebuffer_blue_field_position, mb_info.framebuffer_blue_mask_size);

}
