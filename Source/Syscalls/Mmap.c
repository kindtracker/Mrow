#include "Mrow.h"

#include <errno.h>
#include <stdint.h>

void MrowSyscallMmap(uc_engine *Unicorn) {
  uint64_t Address;
  uint64_t Length;
  uint64_t Protection;
  uint64_t Flags;
  uint64_t Fd;
  uint64_t Offset;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &Address);
  uc_reg_read(Unicorn, UC_ARM64_REG_X1, &Length);
  uc_reg_read(Unicorn, UC_ARM64_REG_X2, &Protection);
  uc_reg_read(Unicorn, UC_ARM64_REG_X3, &Flags);
  uc_reg_read(Unicorn, UC_ARM64_REG_X4, &Fd);
  uc_reg_read(Unicorn, UC_ARM64_REG_X5, &Offset);

  (void)Fd;
  (void)Offset;

  if (Length == 0) {
    Address = (uint64_t)-EINVAL;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Address);
    return;
  }

  // Anonymous mappings only for now.
  if (!(Flags & 0x20)) { // MAP_ANONYMOUS
    Address = (uint64_t)-ENOSYS;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Address);
    return;
  }

  // TODO:
  // 1. Find an available virtual address.
  // 2. Allocate page-aligned memory.
  // 3. Map it using uc_mem_map().
  // 4. Apply Protection using Unicorn permissions.

  Address = (uint64_t)-ENOMEM;
  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Address);
}
