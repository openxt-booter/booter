#include "project.h"
#include "payload.h"

static multiboot_module_t local_payload[PAYLOAD_NUM];

multiboot_module_t *payload;

unsigned n_payload = PAYLOAD_NUM;

void payload_init (void)
{
  unsigned i;
  uint64_t  base = 16ULL << 20;

  PAYLOAD_INIT (local_payload, base);

  payload = local_payload;
}



