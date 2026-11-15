#include "Mrow.h"

#include <stdint.h>

void MrowSyscallExitGroup(uc_engine *Unicorn, MrowProcess *Process) {
  (void)Process;

  uint64_t Status;

  uc_reg_read(Unicorn, UC_ARM64_REG_X0, &Status);

  // Stop execution of the process.
  uc_emu_stop(Unicorn);

  // Store the exit status.
  // TODO: Add an ExitStatus field to MrowProgram.
  (void)Status;
}
