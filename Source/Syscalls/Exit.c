#include "Mrow.h"

void MrowSyscallExit(uc_engine *Unicorn, MrowProcess *Process) {
  (void)Process;

  uint64_t Status;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &Status);
  uc_emu_stop(Unicorn);
}
