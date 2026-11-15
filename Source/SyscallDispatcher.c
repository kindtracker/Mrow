#include <errno.h>
#include <stdint.h>
#include <unistd.h>

#include "Mrow.h"

void MrowSyscall(uc_engine *Unicorn) {
  uint64_t Number;
  uc_reg_read(Unicorn, UC_ARM64_REG_X8, &Number);

  switch (Number) {
  case 64:
    MrowSyscallWrite(Unicorn);
    break;

  case 93:
    MrowSyscallExit(Unicorn);
    break;

  default: {
    uint64_t Result = (uint64_t)-ENOSYS;
    uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    break;
  }
  }
}
