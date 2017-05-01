/* boot.c */
extern void boot(uint64_t entry);
/* ebda.c */
extern void make_ebda(EFI_SYSTEM_TABLE *s);
/* ebs.c */
extern EFI_STATUS ebs(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table);
/* elf.c */
extern uint64_t elf_deploy(uint8_t *image, size_t len);
/* main.c */
extern multiboot_info_t mb_info;
extern EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table);
/* payload.c */
extern multiboot_module_t *payload;
extern unsigned n_payload;
extern void payload_init(void);
/* util.c */
extern int memcmp(const void *_s1, const void *_s2, size_t n);
extern void *memcpy(void *_d, const void *_s, size_t n);
extern void *memset(void *_d, int c, size_t n);
/* framebuffer.c */
extern void framebuffer_init(void);
/* pci.c */
extern uint8_t pci_conf1_read_8(int bus, int slot, int fn, int off);
extern uint16_t pci_conf1_read_16(int bus, int slot, int fn, int off);
extern uint32_t pci_conf1_read_32(int bus, int slot, int fn, int off);
extern void pci_conf1_write_8(int bus, int slot, int fn, int off, uint8_t val);
extern void pci_conf1_write_16(int bus, int slot, int fn, int off, uint16_t val);
extern void pci_conf1_write_32(int bus, int slot, int fn, int off, uint32_t val);
/* serial.c */
extern void serial_dell_power_on(serial_t *s);
extern void serial_write_reg(serial_t *s, unsigned reg, uint8_t val);
extern uint8_t serial_read_reg(serial_t *s, unsigned reg);
extern void serial_set_baud(serial_t *s, unsigned baud);
extern int serial_check_baud(serial_t *s, unsigned baud);
extern void serial_setup(serial_t *s);
extern int serial_have_data(serial_t *s);
extern int serial_getc(serial_t *s);
extern void serial_putc(serial_t *s, char c);
extern void serial_puts(serial_t *s, char *str);
extern int serial_present(serial_t *s);
extern const char *serial_status(serial_t *s);
extern void serial_init(int enabled);
