#include "Mrow.h"

#include <errno.h>
#include <stdint.h>

#define MROW_PAGE_SIZE 0x1000ULL
#define MROW_PROT_MASK 0x7ULL

void MrowSyscallMprotect(uc_engine *Unicorn, MrowProcess *Process) {
  uint64_t Address;
  uint64_t Size;
  uint64_t Protection;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &Address);
  uc_reg_read(Unicorn, UC_ARM64_REG_X1, &Size);
  uc_reg_read(Unicorn, UC_ARM64_REG_X2, &Protection);

  // TODO: Validate that the requested range belongs to mapped guest memory.
  // TODO: Track memory regions and their current permissions in MrowProcess.
  // TODO: Reject ranges that overlap unmapped memory.
  // TODO: Support partial-region protection changes.
  // TODO: Consider W^X policy if required.

  (void)Process;

  if (Size == 0 || (Protection & ~MROW_PROT_MASK)) {
    uint64_t Result = (uint64_t)-EINVAL;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    return;
  }

  uint64_t Start = Address & ~(MROW_PAGE_SIZE - 1);

  if (Size > UINT64_MAX - (Address - Start) - (MROW_PAGE_SIZE - 1)) {
    uint64_t Result = (uint64_t)-ENOMEM;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    return;
  }

  uint64_t End = (Address + Size + MROW_PAGE_SIZE - 1) & ~(MROW_PAGE_SIZE - 1);

  uint32_t UnicornProtection = UC_PROT_NONE;

  if (Protection & 1)
    UnicornProtection |= UC_PROT_READ;

  if (Protection & 2)
    UnicornProtection |= UC_PROT_WRITE;

  if (Protection & 4)
    UnicornProtection |= UC_PROT_EXEC;

  uc_err Error = uc_mem_protect(Unicorn, Start, End - Start, UnicornProtection);

  uint64_t Result = Error == UC_ERR_OK ? 0 : (uint64_t)-ENOMEM;

  uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
}
