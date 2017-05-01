#define GNUEFI30

#include <efi.h>
#include <efilib.h>

#include <stdint.h>

typedef long int ssize_t;
typedef long unsigned int size_t;

#include "multiboot.h"
#include "serial.h"
#include "io.h"


#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a):(b))
#endif

#include "prototypes.h"
