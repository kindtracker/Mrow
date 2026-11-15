#include <errno.h>
#include <stdint.h>
#include <unistd.h>

#include "Mrow.h"

void MrowSyscall(uc_engine *Unicorn, MrowProcess *Process) {
  uint64_t Number;
  uc_reg_read(Unicorn, UC_ARM64_REG_X8, &Number);

  switch (Number) {
  case 64:
    MrowSyscallWrite(Unicorn, Process);
    break;

  case 93:
    MrowSyscallExit(Unicorn, Process);
    break;

  case 214:
    MrowSyscallBrk(Unicorn, Process);
    break;

  case 222:
    MrowSyscallMmap(Unicorn, Process);
    break;

  case 94:
    MrowSyscallExitGroup(Unicorn, Process);
    break;

  default: {
    Crash("Unknown svc: Number=%lu", Number);

    // uint64_t Result = (uint64_t)-ENOSYS;
    // uc_reg_write(Unicorn, UC_ARM64_REG_X0, &Result);
    break;
  }
  }
}
