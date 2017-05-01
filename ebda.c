#include "project.h"

#define EBDA_SEG_ADDR   0x40e
#define LOW_MEM_ADDR    0x413
#define FAKE_EBDA_SEG   0x7000
#define FAKE_SMBIOS_SEG   0x8000

#define ACPI_SIZE 0x10000 /*One segment */


static EFI_GUID acpi_guid = ACPI_TABLE_GUID;
static EFI_GUID acpi2_guid = ACPI_20_TABLE_GUID;
static EFI_GUID smbios_guid = SMBIOS_TABLE_GUID;

static uint64_t find_smbios (EFI_SYSTEM_TABLE *s)
{
  unsigned i;

  for (i = 0; i < s->NumberOfTableEntries; i++) {
    void *guid = &s->ConfigurationTable[i].VendorGuid;

    if (!memcmp (guid, &smbios_guid, sizeof (EFI_GUID)))
      return (size_t) s->ConfigurationTable[i].VendorTable;
  }

  return 0;
}


static uint64_t find_acpi (EFI_SYSTEM_TABLE *s)
{
  unsigned i;

  uint64_t acpi2 = 0, acpi = 0;

  for (i = 0; i < s->NumberOfTableEntries; i++) {
    void *guid = &s->ConfigurationTable[i].VendorGuid;

    if (!memcmp (guid, &acpi2_guid, sizeof (EFI_GUID)))
      acpi2 = (size_t) s->ConfigurationTable[i].VendorTable;

    if (!memcmp (guid, &acpi_guid, sizeof (EFI_GUID)))
      acpi = (size_t) s->ConfigurationTable[i].VendorTable;
  }

  return acpi2 ? acpi2 : acpi;
}



void make_ebda (EFI_SYSTEM_TABLE *s)
{
  uint16_t *ebda_seg_ptr, *low_mem_ptr;
  uint64_t t;

  t = find_acpi (s);

  ebda_seg_ptr = (uint16_t *) EBDA_SEG_ADDR;
  low_mem_ptr = (uint16_t *) LOW_MEM_ADDR;

  if (t) {
    *ebda_seg_ptr = FAKE_EBDA_SEG;
    *low_mem_ptr = FAKE_EBDA_SEG >> 6;

    memcpy ((void *) (FAKE_EBDA_SEG << 4), (void *)t, ACPI_SIZE);
  }

  t = find_smbios (s);

  if (t)
    memcpy ((void *) (FAKE_SMBIOS_SEG << 4), (void *) t, 0x20);

}
